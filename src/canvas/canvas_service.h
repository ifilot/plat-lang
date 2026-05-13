#pragma once

#include "canvas/canvas_backend.h"
#include "value.h"

#include <memory>
#include <stdexcept>
#include <string>

namespace plat {

/**
 * Exception type used for recoverable canvas runtime failures.
 */
class CanvasError final : public std::runtime_error {
public:
    /**
     * Creates a canvas error with a user-facing message.
     *
     * @param message Description of the canvas failure.
     */
    explicit CanvasError(const std::string &message);
};

/**
 * Owns the configured canvas backend and validates language-level handles.
 */
class CanvasService {
private:
    std::unique_ptr<CanvasBackend> backend_;
    CanvasId active_canvas_ = 0;

public:
    /**
     * Creates a canvas service without an active backend.
     */
    CanvasService();

    /**
     * Creates a canvas service with an initial backend.
     *
     * @param backend Backend used to implement canvas operations.
     */
    explicit CanvasService(std::unique_ptr<CanvasBackend> backend);

    /**
     * Replaces the active backend and clears the active canvas handle.
     *
     * @param backend Backend to install.
     */
    void set_backend(std::unique_ptr<CanvasBackend> backend);

    /**
     * Checks whether canvas operations have a backend available.
     */
    bool has_backend() const;

    /**
     * Creates the single active canvas and returns its native handle.
     *
     * @param width Requested canvas width.
     * @param height Requested canvas height.
     * @param title Window or surface title.
     *
     * @throws CanvasError when no backend is available or a canvas is open.
     */
    std::shared_ptr<NativeHandleValue> create(double width, double height,
                                             const std::string &title);

    /**
     * Clears a canvas handle to a solid color.
     *
     * @param canvas Native handle produced by create().
     * @param color Color to fill with.
     *
     * @throws CanvasError when the handle is invalid or the backend fails.
     */
    void clear(const NativeHandleValue &canvas, CanvasColor color);

    /**
     * Presents pending drawing for a canvas handle.
     *
     * @param canvas Native handle produced by create().
     *
     * @throws CanvasError when the handle is invalid or the backend fails.
     */
    void present(const NativeHandleValue &canvas);

    /**
     * Closes a canvas handle and clears it as the active canvas.
     *
     * @param canvas Native handle produced by create().
     *
     * @throws CanvasError when the handle is invalid or the backend fails.
     */
    void close(const NativeHandleValue &canvas);

    /**
     * Waits until a canvas is closed and clears it as the active canvas.
     *
     * @param canvas Native handle produced by create().
     *
     * @throws CanvasError when the handle is invalid or the backend fails.
     */
    void wait_until_closed(const NativeHandleValue &canvas);

    /**
     * Pauses execution while keeping the canvas backend responsive.
     *
     * @param canvas Native handle produced by create().
     * @param milliseconds Pause duration.
     *
     * @throws CanvasError when the handle is invalid or the backend fails.
     */
    void pause(const NativeHandleValue &canvas, double milliseconds);

    /**
     * Checks whether a canvas handle is still open.
     *
     * @param canvas Native handle produced by create().
     *
     * @throws CanvasError when the handle is invalid or no backend is available.
     */
    bool is_open(const NativeHandleValue &canvas) const;

    /**
     * Draws a line on a canvas handle.
     *
     * @throws CanvasError when the handle is invalid or the backend fails.
     */
    void line(const NativeHandleValue &canvas, double x1, double y1, double x2,
              double y2, const CanvasDrawOptions &options);

    /**
     * Draws a rectangle on a canvas handle.
     *
     * @throws CanvasError when the handle is invalid or the backend fails.
     */
    void rect(const NativeHandleValue &canvas, double x, double y, double width,
              double height, const CanvasDrawOptions &options);

    /**
     * Draws a circle on a canvas handle.
     *
     * @throws CanvasError when the handle is invalid or the backend fails.
     */
    void circle(const NativeHandleValue &canvas, double x, double y,
                double radius, const CanvasDrawOptions &options);

    /**
     * Draws text on a canvas handle.
     *
     * @throws CanvasError when the handle is invalid or the backend fails.
     */
    void text(const NativeHandleValue &canvas, double x, double y,
              const std::string &text, const CanvasDrawOptions &options);

private:
    /**
     * Returns the mutable backend or reports that canvas support is unavailable.
     *
     * @throws CanvasError when no backend is configured.
     */
    CanvasBackend &backend();

    /**
     * Returns the backend or reports that canvas support is unavailable.
     *
     * @throws CanvasError when no backend is configured.
     */
    const CanvasBackend &backend() const;

    /**
     * Extracts a backend canvas identifier from a native handle.
     *
     * @param canvas Native handle to validate.
     *
     * @throws CanvasError when the handle is not a canvas handle.
     */
    CanvasId checked_id(const NativeHandleValue &canvas) const;
};

} // namespace plat
