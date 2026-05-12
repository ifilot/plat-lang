#pragma once

#include "canvas/canvas_backend.h"

#include <cstdint>
#include <string>

struct SDL_Renderer;
struct SDL_Window;

namespace plat {

class SdlCanvasBackend final : public CanvasBackend {
private:
    SDL_Window *window_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;
    CanvasId canvas_ = 0;
    std::uint32_t window_id_ = 0;
    bool sdl_initialized_ = false;
    bool open_ = false;

public:
    ~SdlCanvasBackend() override;

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

private:
    void initialize_sdl();
    void require_open(CanvasId canvas) const;
    void pump_events();
    void destroy_window();
};

} // namespace plat
