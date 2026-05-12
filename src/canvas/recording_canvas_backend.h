#pragma once

#include "canvas/canvas_backend.h"

#include <string>
#include <vector>

namespace plat {

enum class CanvasOperationKind {
    Create,
    Clear,
    Present,
    Close,
    WaitUntilClosed,
    Pause,
    Line,
    Rect,
    Circle,
    Text,
};

struct CanvasOperation {
    CanvasOperationKind kind;
    CanvasId canvas = 0;
    std::vector<double> numbers;
    std::string text;
    CanvasColor color;
    CanvasDrawOptions options;
};

class RecordingCanvasBackend final : public CanvasBackend {
private:
    std::vector<CanvasOperation> operations_;
    CanvasId next_id_ = 1;
    CanvasId open_canvas_ = 0;

public:
    CanvasId create(double width, double height,
                    const std::string &title) override;
    void clear(CanvasId canvas, CanvasColor color) override;
    void present(CanvasId canvas) override;
    void close(CanvasId canvas) override;
    void wait_until_closed(CanvasId canvas) override;
    void pause(CanvasId canvas, double milliseconds) override;
    bool is_open(CanvasId canvas) const override;
    void line(CanvasId canvas, double x1, double y1, double x2, double y2,
              const CanvasDrawOptions &options) override;
    void rect(CanvasId canvas, double x, double y, double width, double height,
              const CanvasDrawOptions &options) override;
    void circle(CanvasId canvas, double x, double y, double radius,
                const CanvasDrawOptions &options) override;
    void text(CanvasId canvas, double x, double y, const std::string &text,
              const CanvasDrawOptions &options) override;

    const std::vector<CanvasOperation> &operations() const;

private:
    void require_open(CanvasId canvas) const;
};

} // namespace plat
