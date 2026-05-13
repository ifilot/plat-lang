#include "capabilities/capability_loader.h"

#include "builtins/canvas_builtins.h"
#include "canvas/canvas_service.h"
#include "canvas/recording_canvas_backend.h"

#include <cctype>
#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <utility>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace plat {

struct CapabilityLoader::NativeLibrary {
#ifdef _WIN32
    HMODULE handle = nullptr;
#else
    void *handle = nullptr;
#endif
};

namespace {

constexpr int kCapabilityAbiVersion = 1;

class ManifestParseError final : public std::runtime_error {
public:
    explicit ManifestParseError(const std::string &message)
        : std::runtime_error(message) {}
};

class JsonReader {
private:
    std::string_view input_;
    std::size_t pos_ = 0;

public:
    explicit JsonReader(std::string_view input) : input_(input) {}

    CapabilityManifest read_manifest() {
        CapabilityManifest manifest;

        expect('{');
        skip_ws();
        while (!consume('}')) {
            const std::string key = read_string();
            expect(':');

            if (key == "name") {
                manifest.name = read_string();
            } else if (key == "version") {
                manifest.version = read_string();
            } else if (key == "abi") {
                manifest.abi = read_int();
            } else if (key == "library") {
                manifest.library = read_string();
            } else if (key == "provides") {
                manifest.provides = read_string_array();
            } else {
                skip_value();
            }

            skip_ws();
            if (consume('}')) {
                break;
            }
            expect(',');
        }

        skip_ws();
        if (pos_ != input_.size()) {
            throw ManifestParseError("unexpected content after JSON object");
        }

        return manifest;
    }

private:
    void skip_ws() {
        while (pos_ < input_.size() &&
               std::isspace(static_cast<unsigned char>(input_[pos_]))) {
            ++pos_;
        }
    }

    bool consume(char expected) {
        skip_ws();
        if (pos_ >= input_.size() || input_[pos_] != expected) {
            return false;
        }
        ++pos_;
        return true;
    }

    void expect(char expected) {
        if (!consume(expected)) {
            std::string message = "expected '";
            message.push_back(expected);
            message.push_back('\'');
            throw ManifestParseError(message);
        }
    }

    std::string read_string() {
        skip_ws();
        if (pos_ >= input_.size() || input_[pos_] != '"') {
            throw ManifestParseError("expected string");
        }

        ++pos_;
        std::string result;
        while (pos_ < input_.size()) {
            const char ch = input_[pos_++];
            if (ch == '"') {
                return result;
            }

            if (ch != '\\') {
                result.push_back(ch);
                continue;
            }

            if (pos_ >= input_.size()) {
                throw ManifestParseError("unterminated escape sequence");
            }

            const char escaped = input_[pos_++];
            switch (escaped) {
            case '"':
            case '\\':
            case '/':
                result.push_back(escaped);
                break;
            case 'b':
                result.push_back('\b');
                break;
            case 'f':
                result.push_back('\f');
                break;
            case 'n':
                result.push_back('\n');
                break;
            case 'r':
                result.push_back('\r');
                break;
            case 't':
                result.push_back('\t');
                break;
            default:
                throw ManifestParseError("unsupported string escape");
            }
        }

        throw ManifestParseError("unterminated string");
    }

    int read_int() {
        skip_ws();
        const std::size_t begin = pos_;
        if (pos_ < input_.size() && input_[pos_] == '-') {
            ++pos_;
        }
        while (pos_ < input_.size() &&
               std::isdigit(static_cast<unsigned char>(input_[pos_]))) {
            ++pos_;
        }
        if (begin == pos_) {
            throw ManifestParseError("expected integer");
        }

        return std::stoi(std::string(input_.substr(begin, pos_ - begin)));
    }

    std::vector<std::string> read_string_array() {
        std::vector<std::string> values;
        expect('[');
        skip_ws();
        while (!consume(']')) {
            values.push_back(read_string());
            skip_ws();
            if (consume(']')) {
                break;
            }
            expect(',');
        }
        return values;
    }

    void skip_value() {
        skip_ws();
        if (pos_ >= input_.size()) {
            throw ManifestParseError("expected value");
        }

        if (input_[pos_] == '"') {
            (void)read_string();
            return;
        }

        if (input_[pos_] == '{') {
            skip_compound('{', '}');
            return;
        }

        if (input_[pos_] == '[') {
            skip_compound('[', ']');
            return;
        }

        while (pos_ < input_.size() && input_[pos_] != ',' &&
               input_[pos_] != '}' && input_[pos_] != ']') {
            ++pos_;
        }
    }

    void skip_compound(char open, char close) {
        expect(open);
        int depth = 1;
        while (pos_ < input_.size() && depth > 0) {
            if (input_[pos_] == '"') {
                (void)read_string();
                continue;
            }

            if (input_[pos_] == open) {
                ++depth;
            } else if (input_[pos_] == close) {
                --depth;
            }
            ++pos_;
        }

        if (depth != 0) {
            throw ManifestParseError("unterminated JSON value");
        }
    }
};

bool contains_name(const std::vector<std::string> &names,
                   const std::string &name) {
    for (const std::string &candidate : names) {
        if (candidate == name) {
            return true;
        }
    }
    return false;
}

std::string native_library_error() {
#ifdef _WIN32
    const DWORD code = GetLastError();
    if (code == 0) {
        return "unknown native loader error";
    }

    LPSTR message = nullptr;
    const DWORD size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&message), 0, nullptr);

    if (size == 0 || message == nullptr) {
        return "native loader error " + std::to_string(code);
    }

    std::string result(message, size);
    LocalFree(message);
    while (!result.empty() &&
           (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
    }
    return result;
#else
    const char *error = dlerror();
    if (error == nullptr) {
        return "unknown native loader error";
    }
    return error;
#endif
}

std::string slurp(const std::filesystem::path &path) {
    std::ifstream file(path);
    if (!file) {
        throw ManifestParseError("could not open manifest");
    }

    std::ostringstream out;
    out << file.rdbuf();
    return out.str();
}

} // namespace

CapabilityLoader::CapabilityLoader(
    DiagnosticReporter &diagnostics,
    std::vector<std::filesystem::path> search_paths)
    : diagnostics_(&diagnostics), search_paths_(std::move(search_paths)) {}

CapabilityLoader::~CapabilityLoader() = default;

void CapabilityLoader::load(const std::string &name, BuiltinRegistry &registry,
                            SourceLocation location) {
    if (loaded_.contains(name)) {
        return;
    }

    std::optional<CapabilityManifest> manifest = find_manifest(name, location);
    if (!manifest.has_value()) {
        diagnostics_->fatal(
            DiagnosticId::RuntimeError, location,
            {DiagnosticArg("message",
                           "capability '" + name + "' is not installed")});
    }

    register_manifest(*manifest, name, registry, location);
}

std::optional<CapabilityManifest> CapabilityLoader::find_manifest(
    const std::string &name, SourceLocation location) {
    for (const std::filesystem::path &search_path : search_paths_) {
        const std::filesystem::path manifest_path =
            search_path / name / "manifest.json";
        if (std::filesystem::exists(manifest_path)) {
            return read_manifest(manifest_path, location);
        }
    }

    return std::nullopt;
}

CapabilityManifest CapabilityLoader::read_manifest(
    const std::filesystem::path &path, SourceLocation location) {
    try {
        CapabilityManifest manifest = JsonReader(slurp(path)).read_manifest();
        manifest.path = path;

        if (manifest.name.empty()) {
            throw ManifestParseError("missing required field 'name'");
        }
        if (manifest.abi == 0) {
            throw ManifestParseError("missing required field 'abi'");
        }
        if (manifest.library.empty()) {
            throw ManifestParseError("missing required field 'library'");
        }
        if (manifest.provides.empty()) {
            throw ManifestParseError("missing required field 'provides'");
        }

        return manifest;
    } catch (const ManifestParseError &error) {
        diagnostics_->fatal(
            DiagnosticId::RuntimeError, location,
            {DiagnosticArg("message",
                           "invalid capability manifest '" + path.string() +
                               "': " + error.what())});
    }
}

void CapabilityLoader::register_manifest(const CapabilityManifest &manifest,
                                         const std::string &requested,
                                         BuiltinRegistry &registry,
                                         SourceLocation location) {
    if (manifest.abi != kCapabilityAbiVersion) {
        diagnostics_->fatal(
            DiagnosticId::RuntimeError, location,
            {DiagnosticArg("message",
                           "capability '" + requested +
                               "' is incompatible with this plat-lang version")});
    }

    if (manifest.name != requested && !contains_name(manifest.provides, requested)) {
        diagnostics_->fatal(
            DiagnosticId::RuntimeError, location,
            {DiagnosticArg("message",
                           "capability manifest '" + manifest.path.string() +
                               "' does not provide '" + requested + "'")});
    }

    if (manifest.library == "builtin:canvas") {
        if (canvas_service_ == nullptr) {
            canvas_service_ = std::make_unique<CanvasService>(
                std::make_unique<RecordingCanvasBackend>());
            register_canvas_builtins(registry, *canvas_service_);
        }
    } else {
        load_native_library(manifest, requested, registry, location);
    }

    loaded_.insert(manifest.name);
    for (const std::string &provided : manifest.provides) {
        loaded_.insert(provided);
    }
}

void CapabilityLoader::load_native_library(const CapabilityManifest &manifest,
                                           const std::string &requested,
                                           BuiltinRegistry &registry,
                                           SourceLocation location) {
    const std::filesystem::path library_path =
        manifest.path.parent_path() / manifest.library;

#ifdef _WIN32
    HMODULE handle = LoadLibraryExA(library_path.string().c_str(), nullptr,
                                    LOAD_WITH_ALTERED_SEARCH_PATH);
    if (handle == nullptr) {
#else
    dlerror();
    void *handle = dlopen(library_path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle == nullptr) {
#endif
        diagnostics_->fatal(
            DiagnosticId::RuntimeError, location,
            {DiagnosticArg("message",
                           "could not load capability '" + requested +
                               "' library '" + library_path.string() +
                               "': " + native_library_error())});
    }

    using RegisterCapability = bool (*)(BuiltinRegistry *);

#ifdef _WIN32
    auto register_capability = reinterpret_cast<RegisterCapability>(
        GetProcAddress(handle, "platlang_register_capability_v1"));
    if (register_capability == nullptr) {
#else
    dlerror();
    auto register_capability = reinterpret_cast<RegisterCapability>(
        dlsym(handle, "platlang_register_capability_v1"));
    if (register_capability == nullptr) {
#endif
        diagnostics_->fatal(
            DiagnosticId::RuntimeError, location,
            {DiagnosticArg("message",
                           "capability '" + requested + "' library '" +
                               library_path.string() +
                               "' does not export platlang_register_capability_v1: " +
                               native_library_error())});
    }

    if (!register_capability(&registry)) {
        diagnostics_->fatal(
            DiagnosticId::RuntimeError, location,
            {DiagnosticArg("message",
                           "capability '" + requested +
                               "' failed to register its built-ins")});
    }

    native_libraries_.push_back(NativeLibrary{handle});
}

} // namespace plat
