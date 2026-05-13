#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace plat {

using CanvasId = std::uint64_t;

/**
 * RGBA color used by canvas drawing operations.
 */
struct CanvasColor {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;

    /**
     * Compares two colors by component value.
     */
    bool operator==(const CanvasColor &other) const = default;
};

/**
 * Optional drawing attributes shared by shape and text operations.
 */
struct CanvasDrawOptions {
    std::optional<CanvasColor> fill;
    std::optional<CanvasColor> stroke;
    std::optional<CanvasColor> color;
    double width = 1.0;
    double size = 16.0;
};

/**
 * Abstract rendering backend used by the canvas service.
 */
class CanvasBackend {
public:
    /**
     * Destroys the backend and releases any owned rendering resources.
     */
    virtual ~CanvasBackend() = default;

    /**
     * Creates a canvas and returns its backend-local identifier.
     *
     * @param width Requested canvas width.
     * @param height Requested canvas height.
     * @param title Window or surface title.
     *
     * @throws CanvasError when the backend cannot create a canvas.
     */
    virtual CanvasId create(double width, double height,
                            const std::string &title) = 0;

    /**
     * Clears an open canvas to a solid color.
     *
     * @param canvas Canvas identifier to clear.
     * @param color Color to fill the canvas with.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    virtual void clear(CanvasId canvas, CanvasColor color) = 0;

    /**
     * Presents pending drawing work to the visible canvas.
     *
     * @param canvas Canvas identifier to present.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    virtual void present(CanvasId canvas) = 0;

    /**
     * Closes an open canvas.
     *
     * @param canvas Canvas identifier to close.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    virtual void close(CanvasId canvas) = 0;

    /**
     * Blocks until the user or backend closes the canvas.
     *
     * @param canvas Canvas identifier to monitor.
     *
     * @throws CanvasError when the canvas is invalid or event waiting fails.
     */
    virtual void wait_until_closed(CanvasId canvas) = 0;

    /**
     * Keeps the canvas responsive while pausing execution.
     *
     * @param canvas Canvas identifier to service.
     * @param milliseconds Duration to pause, clamped by the backend as needed.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    virtual void pause(CanvasId canvas, double milliseconds) = 0;

    /**
     * Checks whether a canvas identifier still refers to an open canvas.
     *
     * @param canvas Canvas identifier to check.
     */
    virtual bool is_open(CanvasId canvas) const = 0;

    /**
     * Draws a line segment on an open canvas.
     *
     * @param canvas Canvas identifier to draw on.
     * @param x1 Start x coordinate.
     * @param y1 Start y coordinate.
     * @param x2 End x coordinate.
     * @param y2 End y coordinate.
     * @param options Drawing color and stroke width.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    virtual void line(CanvasId canvas, double x1, double y1, double x2,
                      double y2, const CanvasDrawOptions &options) = 0;

    /**
     * Draws a rectangle on an open canvas.
     *
     * @param canvas Canvas identifier to draw on.
     * @param x Left coordinate.
     * @param y Top coordinate.
     * @param width Rectangle width.
     * @param height Rectangle height.
     * @param options Fill, stroke, and stroke-width attributes.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    virtual void rect(CanvasId canvas, double x, double y, double width,
                      double height, const CanvasDrawOptions &options) = 0;

    /**
     * Draws a circle on an open canvas.
     *
     * @param canvas Canvas identifier to draw on.
     * @param x Center x coordinate.
     * @param y Center y coordinate.
     * @param radius Circle radius.
     * @param options Fill, stroke, and stroke-width attributes.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    virtual void circle(CanvasId canvas, double x, double y, double radius,
                        const CanvasDrawOptions &options) = 0;

    /**
     * Draws bitmap text on an open canvas.
     *
     * @param canvas Canvas identifier to draw on.
     * @param x Text origin x coordinate.
     * @param y Text origin y coordinate.
     * @param text Text to draw.
     * @param options Text color and size attributes.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    virtual void text(CanvasId canvas, double x, double y,
                      const std::string &text,
                      const CanvasDrawOptions &options) = 0;
};

} // namespace plat
