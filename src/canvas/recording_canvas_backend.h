#pragma once

#include "canvas/canvas_backend.h"

#include <string>
#include <vector>

namespace plat {

/**
 * Kind of canvas operation captured by the recording backend.
 */
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
    Path,
    Text,
};

/**
 * Recorded representation of one canvas backend call.
 */
struct CanvasOperation {
    CanvasOperationKind kind;
    CanvasId canvas = 0;
    std::vector<double> numbers;
    std::string text;
    CanvasColor color;
    CanvasDrawOptions options;
    std::vector<CanvasPath> paths;
};

/**
 * Test backend that records canvas operations without rendering them.
 */
class RecordingCanvasBackend final : public CanvasBackend {
private:
    std::vector<CanvasOperation> operations_;
    CanvasId next_id_ = 1;
    CanvasId open_canvas_ = 0;

public:
    /**
     * Records a canvas creation and returns a synthetic canvas identifier.
     *
     * @throws CanvasError when another recorded canvas is still open.
     */
    CanvasId create(double width, double height,
                    const std::string &title) override;

    /**
     * Records a clear operation for an open canvas.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void clear(CanvasId canvas, CanvasColor color) override;

    /**
     * Records a present operation for an open canvas.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void present(CanvasId canvas) override;

    /**
     * Records a close operation and marks the canvas closed.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void close(CanvasId canvas) override;

    /**
     * Records a wait operation and marks the canvas closed.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void wait_until_closed(CanvasId canvas) override;

    /**
     * Records a pause operation for an open canvas.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void pause(CanvasId canvas, double milliseconds) override;

    /**
     * Checks whether the synthetic canvas identifier is open.
     */
    bool is_open(CanvasId canvas) const override;

    /**
     * Records a line drawing operation.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void line(CanvasId canvas, double x1, double y1, double x2, double y2,
              const CanvasDrawOptions &options) override;

    /**
     * Records a rectangle drawing operation.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void rect(CanvasId canvas, double x, double y, double width, double height,
              const CanvasDrawOptions &options) override;

    /**
     * Records a circle drawing operation.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void circle(CanvasId canvas, double x, double y, double radius,
                const CanvasDrawOptions &options) override;

    /**
     * Records a path drawing operation.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void path(CanvasId canvas, const std::vector<CanvasPath> &paths,
              const CanvasDrawOptions &options) override;

    /**
     * Records a text drawing operation.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void text(CanvasId canvas, double x, double y, const std::string &text,
              const CanvasDrawOptions &options) override;

    /**
     * Returns the operations recorded by this backend.
     */
    const std::vector<CanvasOperation> &operations() const;

private:
    /**
     * Validates that the requested synthetic canvas is currently open.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void require_open(CanvasId canvas) const;
};

} // namespace plat
