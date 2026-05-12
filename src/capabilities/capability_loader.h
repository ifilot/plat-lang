#pragma once

#include "ast.h"
#include "builtins/builtin_registry.h"
#include "diagnostics.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace plat {

class CanvasService;

struct CapabilityManifest {
    std::string name;
    std::string version;
    int abi = 0;
    std::string library;
    std::vector<std::string> provides;
    std::filesystem::path path;
};

/**
 * Loads native host capabilities declared by top-level `inlaaje` statements.
 */
class CapabilityLoader {
private:
    DiagnosticReporter *diagnostics_;
    std::vector<std::filesystem::path> search_paths_;
    std::unordered_set<std::string> loaded_;
    std::unique_ptr<CanvasService> canvas_service_;

public:
    CapabilityLoader(DiagnosticReporter &diagnostics,
                     std::vector<std::filesystem::path> search_paths);
    ~CapabilityLoader();

    void load(const std::string &name, BuiltinRegistry &registry,
              SourceLocation location);

private:
    std::optional<CapabilityManifest> find_manifest(const std::string &name,
                                                    SourceLocation location);
    CapabilityManifest read_manifest(const std::filesystem::path &path,
                                     SourceLocation location);
    void register_manifest(const CapabilityManifest &manifest,
                           const std::string &requested,
                           BuiltinRegistry &registry,
                           SourceLocation location);
};

} // namespace plat
