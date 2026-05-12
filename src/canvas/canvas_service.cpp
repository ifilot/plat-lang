#include "canvas/canvas_service.h"

#include <utility>

namespace plat {
namespace {

constexpr const char *kCanvasHandleKind = "canvas";
constexpr const char *kCanvasTypeName = "canvas";

} // namespace

CanvasError::CanvasError(const std::string &message)
    : std::runtime_error(message) {}

CanvasService::CanvasService() = default;

CanvasService::CanvasService(std::unique_ptr<CanvasBackend> backend)
    : backend_(std::move(backend)) {}

void CanvasService::set_backend(std::unique_ptr<CanvasBackend> backend) {
    backend_ = std::move(backend);
    active_canvas_ = 0;
}

bool CanvasService::has_backend() const {
    return backend_ != nullptr;
}

std::shared_ptr<NativeHandleValue> CanvasService::create(
    double width, double height, const std::string &title) {
    if (active_canvas_ != 0 && backend().is_open(active_canvas_)) {
        throw CanvasError("only one canvas can be open at a time");
    }

    const CanvasId id = backend().create(width, height, title);
    active_canvas_ = id;
    return std::make_shared<NativeHandleValue>(kCanvasHandleKind,
                                               kCanvasTypeName, id);
}

void CanvasService::clear(const NativeHandleValue &canvas, CanvasColor color) {
    backend().clear(checked_id(canvas), color);
}

void CanvasService::present(const NativeHandleValue &canvas) {
    backend().present(checked_id(canvas));
}

void CanvasService::close(const NativeHandleValue &canvas) {
    const CanvasId id = checked_id(canvas);
    backend().close(id);
    if (active_canvas_ == id) {
        active_canvas_ = 0;
    }
}

void CanvasService::wait_until_closed(const NativeHandleValue &canvas) {
    const CanvasId id = checked_id(canvas);
    backend().wait_until_closed(id);
    if (active_canvas_ == id) {
        active_canvas_ = 0;
    }
}

void CanvasService::pause(const NativeHandleValue &canvas, double milliseconds) {
    backend().pause(checked_id(canvas), milliseconds);
}

bool CanvasService::is_open(const NativeHandleValue &canvas) const {
    return backend().is_open(checked_id(canvas));
}

void CanvasService::line(const NativeHandleValue &canvas, double x1, double y1,
                         double x2, double y2,
                         const CanvasDrawOptions &options) {
    backend().line(checked_id(canvas), x1, y1, x2, y2, options);
}

void CanvasService::rect(const NativeHandleValue &canvas, double x, double y,
                         double width, double height,
                         const CanvasDrawOptions &options) {
    backend().rect(checked_id(canvas), x, y, width, height, options);
}

void CanvasService::circle(const NativeHandleValue &canvas, double x, double y,
                           double radius, const CanvasDrawOptions &options) {
    backend().circle(checked_id(canvas), x, y, radius, options);
}

void CanvasService::text(const NativeHandleValue &canvas, double x, double y,
                         const std::string &text,
                         const CanvasDrawOptions &options) {
    backend().text(checked_id(canvas), x, y, text, options);
}

CanvasBackend &CanvasService::backend() {
    if (backend_ == nullptr) {
        throw CanvasError("canvas support is not available in this build");
    }

    return *backend_;
}

const CanvasBackend &CanvasService::backend() const {
    if (backend_ == nullptr) {
        throw CanvasError("canvas support is not available in this build");
    }

    return *backend_;
}

CanvasId CanvasService::checked_id(const NativeHandleValue &canvas) const {
    if (canvas.kind() != kCanvasHandleKind) {
        throw CanvasError("expected a canvas handle");
    }

    return static_cast<CanvasId>(canvas.id());
}

} // namespace plat
