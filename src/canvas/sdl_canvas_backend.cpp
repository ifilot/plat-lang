#include "canvas/sdl_canvas_backend.h"

#include "canvas/canvas_service.h"

#include <SDL.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cctype>
#include <string>

namespace plat {
namespace {

constexpr CanvasId kSdlCanvasId = 1;

/**
 * Rounds a floating-point canvas coordinate to an SDL integer coordinate.
 */
int to_int(double value) {
    return static_cast<int>(std::lround(value));
}

/**
 * Rounds a floating-point value and clamps it to a positive SDL size.
 */
int positive_int(double value, int fallback = 1) {
    return std::max(fallback, to_int(value));
}

/**
 * Raises a CanvasError using the current SDL error string.
 *
 * @throws CanvasError always.
 */
void throw_sdl_error(const std::string &prefix) {
    throw CanvasError(prefix + ": " + SDL_GetError());
}

/**
 * Applies an RGBA color to an SDL renderer with alpha blending enabled.
 */
void set_color(SDL_Renderer *renderer, CanvasColor color) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

/**
 * Draws a line with approximate integer thickness.
 */
void draw_thick_line(SDL_Renderer *renderer, int x1, int y1, int x2, int y2,
                     int width) {
    if (width <= 1) {
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        return;
    }

    const int radius = width / 2;
    for (int offset = -radius; offset <= radius; ++offset) {
        SDL_RenderDrawLine(renderer, x1 + offset, y1, x2 + offset, y2);
        SDL_RenderDrawLine(renderer, x1, y1 + offset, x2, y2 + offset);
    }
}

/**
 * Draws a rectangle outline with approximate integer thickness.
 */
void draw_rect_outline(SDL_Renderer *renderer, SDL_Rect rect, int width) {
    for (int i = 0; i < width; ++i) {
        SDL_Rect outline{rect.x - i, rect.y - i, rect.w + i * 2,
                         rect.h + i * 2};
        SDL_RenderDrawRect(renderer, &outline);
    }
}

/**
 * Fills a circle using horizontal scan lines.
 */
void draw_filled_circle(SDL_Renderer *renderer, int cx, int cy, int radius) {
    for (int y = -radius; y <= radius; ++y) {
        const int x = static_cast<int>(
            std::floor(std::sqrt(static_cast<double>(radius * radius - y * y))));
        SDL_RenderDrawLine(renderer, cx - x, cy + y, cx + x, cy + y);
    }
}

/**
 * Draws a circle outline by filling points in an annulus.
 */
void draw_circle_outline(SDL_Renderer *renderer, int cx, int cy, int radius,
                         int width) {
    const int half = std::max(0, width / 2);
    const int inner = std::max(0, radius - half);
    const int outer = radius + half;

    for (int y = -outer; y <= outer; ++y) {
        for (int x = -outer; x <= outer; ++x) {
            const int dist2 = x * x + y * y;
            if (dist2 >= inner * inner && dist2 <= outer * outer) {
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
}

/**
 * Returns the 5x7 bitmap glyph for a supported character.
 */
std::array<std::uint8_t, 7> glyph_for(char raw) {
    const char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(raw)));
    switch (ch) {
    case 'A':
        return {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
    case 'B':
        return {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E};
    case 'C':
        return {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E};
    case 'D':
        return {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E};
    case 'E':
        return {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F};
    case 'F':
        return {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10};
    case 'G':
        return {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F};
    case 'H':
        return {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
    case 'I':
        return {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F};
    case 'J':
        return {0x01, 0x01, 0x01, 0x01, 0x11, 0x11, 0x0E};
    case 'K':
        return {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11};
    case 'L':
        return {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F};
    case 'M':
        return {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11};
    case 'N':
        return {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11};
    case 'O':
        return {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
    case 'P':
        return {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10};
    case 'Q':
        return {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D};
    case 'R':
        return {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11};
    case 'S':
        return {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E};
    case 'T':
        return {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
    case 'U':
        return {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
    case 'V':
        return {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04};
    case 'W':
        return {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A};
    case 'X':
        return {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11};
    case 'Y':
        return {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04};
    case 'Z':
        return {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F};
    case '0':
        return {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E};
    case '1':
        return {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E};
    case '2':
        return {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F};
    case '3':
        return {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E};
    case '4':
        return {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02};
    case '5':
        return {0x1F, 0x10, 0x10, 0x1E, 0x01, 0x01, 0x1E};
    case '6':
        return {0x0E, 0x10, 0x10, 0x1E, 0x11, 0x11, 0x0E};
    case '7':
        return {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08};
    case '8':
        return {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E};
    case '9':
        return {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x0E};
    case '!':
        return {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04};
    case '.':
        return {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C};
    case ',':
        return {0x00, 0x00, 0x00, 0x00, 0x0C, 0x04, 0x08};
    case '-':
        return {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00};
    case ':':
        return {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00};
    default:
        return {0x1F, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04};
    }
}

/**
 * Draws simple 5x7 bitmap text into an SDL renderer.
 */
void draw_bitmap_text(SDL_Renderer *renderer, int x, int y,
                      const std::string &text, int pixel_size) {
    int cursor = x;
    const int scale = std::max(1, pixel_size / 7);

    for (char ch : text) {
        if (ch == ' ') {
            cursor += scale * 4;
            continue;
        }

        const std::array<std::uint8_t, 7> glyph = glyph_for(ch);
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if ((glyph[row] & (1 << (4 - col))) == 0) {
                    continue;
                }

                SDL_Rect rect{cursor + col * scale, y + row * scale, scale,
                              scale};
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        cursor += scale * 6;
    }
}

} // namespace

SdlCanvasBackend::~SdlCanvasBackend() {
    destroy_window();
    if (sdl_initialized_) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }
}

CanvasId SdlCanvasBackend::create(double width, double height,
                                  const std::string &title) {
    if (open_) {
        throw CanvasError("only one canvas can be open at a time");
    }

    initialize_sdl();

    window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, positive_int(width),
                               positive_int(height), SDL_WINDOW_SHOWN);
    if (window_ == nullptr) {
        throw_sdl_error("could not create canvas window");
    }

    renderer_ = SDL_CreateRenderer(
        window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer_ == nullptr) {
        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
    }
    if (renderer_ == nullptr) {
        destroy_window();
        throw_sdl_error("could not create canvas renderer");
    }

    window_id_ = SDL_GetWindowID(window_);
    canvas_ = kSdlCanvasId;
    open_ = true;
    return canvas_;
}

void SdlCanvasBackend::clear(CanvasId canvas, CanvasColor color) {
    require_open(canvas);
    pump_events();
    set_color(renderer_, color);
    SDL_RenderClear(renderer_);
}

void SdlCanvasBackend::present(CanvasId canvas) {
    require_open(canvas);
    pump_events();
    SDL_RenderPresent(renderer_);
}

void SdlCanvasBackend::close(CanvasId canvas) {
    require_open(canvas);
    destroy_window();
}

void SdlCanvasBackend::wait_until_closed(CanvasId canvas) {
    require_open(canvas);

    while (open_) {
        SDL_Event event;
        if (SDL_WaitEvent(&event) == 0) {
            throw_sdl_error("could not wait for canvas events");
        }

        if (event.type == SDL_QUIT ||
            (event.type == SDL_WINDOWEVENT &&
             event.window.windowID == window_id_ &&
             event.window.event == SDL_WINDOWEVENT_CLOSE)) {
            destroy_window();
        }
    }
}

void SdlCanvasBackend::pause(CanvasId canvas, double milliseconds) {
    require_open(canvas);

    const auto duration =
        std::chrono::milliseconds(std::max(0, to_int(milliseconds)));
    const auto end = std::chrono::steady_clock::now() + duration;
    while (open_ && std::chrono::steady_clock::now() < end) {
        pump_events();
        SDL_Delay(10);
    }
}

bool SdlCanvasBackend::is_open(CanvasId canvas) const {
    return canvas == canvas_ && open_;
}

void SdlCanvasBackend::line(CanvasId canvas, double x1, double y1, double x2,
                            double y2, const CanvasDrawOptions &options) {
    require_open(canvas);
    pump_events();
    set_color(renderer_, options.color.value_or(CanvasColor{0, 0, 0, 255}));
    draw_thick_line(renderer_, to_int(x1), to_int(y1), to_int(x2), to_int(y2),
                    positive_int(options.width));
}

void SdlCanvasBackend::rect(CanvasId canvas, double x, double y, double width,
                            double height, const CanvasDrawOptions &options) {
    require_open(canvas);
    pump_events();

    SDL_Rect rect{to_int(x), to_int(y), positive_int(width), positive_int(height)};
    if (options.fill.has_value()) {
        set_color(renderer_, *options.fill);
        SDL_RenderFillRect(renderer_, &rect);
    }

    if (options.stroke.has_value()) {
        set_color(renderer_, *options.stroke);
        draw_rect_outline(renderer_, rect, positive_int(options.width));
    }
}

void SdlCanvasBackend::circle(CanvasId canvas, double x, double y,
                              double radius,
                              const CanvasDrawOptions &options) {
    require_open(canvas);
    pump_events();

    const int cx = to_int(x);
    const int cy = to_int(y);
    const int r = positive_int(radius);
    if (options.fill.has_value()) {
        set_color(renderer_, *options.fill);
        draw_filled_circle(renderer_, cx, cy, r);
    }

    if (options.stroke.has_value()) {
        set_color(renderer_, *options.stroke);
        draw_circle_outline(renderer_, cx, cy, r, positive_int(options.width));
    }
}

void SdlCanvasBackend::text(CanvasId canvas, double x, double y,
                            const std::string &text,
                            const CanvasDrawOptions &options) {
    require_open(canvas);
    pump_events();
    set_color(renderer_, options.color.value_or(CanvasColor{0, 0, 0, 255}));
    draw_bitmap_text(renderer_, to_int(x), to_int(y), text,
                     positive_int(options.size, 16));
}

void SdlCanvasBackend::initialize_sdl() {
    if (sdl_initialized_) {
        return;
    }

    SDL_SetMainReady();
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        throw_sdl_error("could not initialize SDL video");
    }
    sdl_initialized_ = true;
}

void SdlCanvasBackend::require_open(CanvasId canvas) const {
    if (canvas != canvas_ || !open_) {
        throw CanvasError("canvas is closed or invalid");
    }
}

void SdlCanvasBackend::pump_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT ||
            (event.type == SDL_WINDOWEVENT &&
             event.window.windowID == window_id_ &&
             event.window.event == SDL_WINDOWEVENT_CLOSE)) {
            destroy_window();
            return;
        }
    }
}

void SdlCanvasBackend::destroy_window() {
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    open_ = false;
    window_id_ = 0;
    canvas_ = 0;
}

} // namespace plat
