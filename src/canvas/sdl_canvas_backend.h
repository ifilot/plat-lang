#pragma once

#include "canvas/canvas_backend.h"

#include <cstdint>
#include <string>

struct SDL_Renderer;
struct SDL_Window;

namespace plat {

/**
 * SDL-backed canvas renderer used when video support is enabled.
 */
class SdlCanvasBackend final : public CanvasBackend {
private:
    SDL_Window *window_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;
    CanvasId canvas_ = 0;
    std::uint32_t window_id_ = 0;
    bool sdl_initialized_ = false;
    bool open_ = false;

public:
    /**
     * Destroys any open SDL window and shuts down the SDL video subsystem.
     */
    ~SdlCanvasBackend() override;

    /**
     * Opens an SDL window and renderer for the canvas.
     *
     * @throws CanvasError when SDL cannot create the window or renderer.
     */
    CanvasId create(double width, double height,
                    const std::string &title) override;

    /**
     * Clears the SDL renderer to a color.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void clear(CanvasId canvas, CanvasColor color) override;

    /**
     * Presents the current SDL back buffer.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void present(CanvasId canvas) override;

    /**
     * Closes the SDL window for the canvas.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void close(CanvasId canvas) override;

    /**
     * Processes SDL events until the canvas window is closed.
     *
     * @throws CanvasError when the canvas is invalid or event waiting fails.
     */
    void wait_until_closed(CanvasId canvas) override;

    /**
     * Delays execution while continuing to process SDL window events.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void pause(CanvasId canvas, double milliseconds) override;

    /**
     * Checks whether the SDL canvas identifier is currently open.
     */
    bool is_open(CanvasId canvas) const override;

    /**
     * Draws a line into the SDL renderer.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void line(CanvasId canvas, double x1, double y1, double x2, double y2,
              const CanvasDrawOptions &options) override;

    /**
     * Draws a rectangle into the SDL renderer.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void rect(CanvasId canvas, double x, double y, double width, double height,
              const CanvasDrawOptions &options) override;

    /**
     * Draws a circle into the SDL renderer.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void circle(CanvasId canvas, double x, double y, double radius,
                const CanvasDrawOptions &options) override;

    /**
     * Draws bitmap text into the SDL renderer.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void text(CanvasId canvas, double x, double y, const std::string &text,
              const CanvasDrawOptions &options) override;

private:
    /**
     * Initializes SDL video support if it is not already active.
     *
     * @throws CanvasError when SDL video initialization fails.
     */
    void initialize_sdl();

    /**
     * Validates that the requested SDL canvas is open.
     *
     * @throws CanvasError when the canvas is invalid or closed.
     */
    void require_open(CanvasId canvas) const;

    /**
     * Processes pending SDL close events for the active window.
     */
    void pump_events();

    /**
     * Releases the SDL renderer and window and clears canvas state.
     */
    void destroy_window();
};

} // namespace plat
