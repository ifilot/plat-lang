#include "canvas/recording_canvas_backend.h"

#include "canvas/canvas_service.h"

#include <utility>

namespace plat {

CanvasId RecordingCanvasBackend::create(double width, double height,
                                        const std::string &title) {
    if (open_canvas_ != 0) {
        throw CanvasError("only one canvas can be open at a time");
    }

    const CanvasId id = next_id_++;
    open_canvas_ = id;
    operations_.push_back(
        {CanvasOperationKind::Create, id, {width, height}, title});
    return id;
}

void RecordingCanvasBackend::clear(CanvasId canvas, CanvasColor color) {
    require_open(canvas);
    CanvasOperation operation{CanvasOperationKind::Clear, canvas};
    operation.color = color;
    operations_.push_back(operation);
}

void RecordingCanvasBackend::present(CanvasId canvas) {
    require_open(canvas);
    operations_.push_back({CanvasOperationKind::Present, canvas});
}

void RecordingCanvasBackend::close(CanvasId canvas) {
    require_open(canvas);
    operations_.push_back({CanvasOperationKind::Close, canvas});
    open_canvas_ = 0;
}

void RecordingCanvasBackend::wait_until_closed(CanvasId canvas) {
    require_open(canvas);
    operations_.push_back({CanvasOperationKind::WaitUntilClosed, canvas});
    open_canvas_ = 0;
}

void RecordingCanvasBackend::pause(CanvasId canvas, double milliseconds) {
    require_open(canvas);
    operations_.push_back(
        {CanvasOperationKind::Pause, canvas, {milliseconds}});
}

bool RecordingCanvasBackend::is_open(CanvasId canvas) const {
    return open_canvas_ == canvas;
}

void RecordingCanvasBackend::line(CanvasId canvas, double x1, double y1,
                                  double x2, double y2,
                                  const CanvasDrawOptions &options) {
    require_open(canvas);
    CanvasOperation operation{CanvasOperationKind::Line,
                              canvas,
                              {x1, y1, x2, y2}};
    operation.options = options;
    operations_.push_back(operation);
}

void RecordingCanvasBackend::rect(CanvasId canvas, double x, double y,
                                  double width, double height,
                                  const CanvasDrawOptions &options) {
    require_open(canvas);
    CanvasOperation operation{CanvasOperationKind::Rect,
                              canvas,
                              {x, y, width, height}};
    operation.options = options;
    operations_.push_back(operation);
}

void RecordingCanvasBackend::circle(CanvasId canvas, double x, double y,
                                    double radius,
                                    const CanvasDrawOptions &options) {
    require_open(canvas);
    CanvasOperation operation{CanvasOperationKind::Circle,
                              canvas,
                              {x, y, radius}};
    operation.options = options;
    operations_.push_back(operation);
}

void RecordingCanvasBackend::path(CanvasId canvas,
                                  const std::vector<CanvasPath> &paths,
                                  const CanvasDrawOptions &options) {
    require_open(canvas);
    CanvasOperation operation{CanvasOperationKind::Path, canvas};
    operation.paths = paths;
    operation.options = options;
    operations_.push_back(operation);
}

void RecordingCanvasBackend::text(CanvasId canvas, double x, double y,
                                  const std::string &text,
                                  const CanvasDrawOptions &options) {
    require_open(canvas);
    CanvasOperation operation{CanvasOperationKind::Text, canvas, {x, y}, text};
    operation.options = options;
    operations_.push_back(operation);
}

const std::vector<CanvasOperation> &RecordingCanvasBackend::operations() const {
    return operations_;
}

void RecordingCanvasBackend::require_open(CanvasId canvas) const {
    if (open_canvas_ != canvas) {
        throw CanvasError("canvas is closed or invalid");
    }
}

} // namespace plat
