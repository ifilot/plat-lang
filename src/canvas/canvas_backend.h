#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace plat {

using CanvasId = std::uint64_t;

struct CanvasColor {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;

    bool operator==(const CanvasColor &other) const = default;
};

struct CanvasDrawOptions {
    std::optional<CanvasColor> fill;
    std::optional<CanvasColor> stroke;
    std::optional<CanvasColor> color;
    double width = 1.0;
    double size = 16.0;
};

class CanvasBackend {
public:
    virtual ~CanvasBackend() = default;

    virtual CanvasId create(double width, double height,
                            const std::string &title) = 0;
    virtual void clear(CanvasId canvas, CanvasColor color) = 0;
    virtual void present(CanvasId canvas) = 0;
    virtual void close(CanvasId canvas) = 0;
    virtual void wait_until_closed(CanvasId canvas) = 0;
    virtual void pause(CanvasId canvas, double milliseconds) = 0;
    virtual bool is_open(CanvasId canvas) const = 0;
    virtual void line(CanvasId canvas, double x1, double y1, double x2,
                      double y2, const CanvasDrawOptions &options) = 0;
    virtual void rect(CanvasId canvas, double x, double y, double width,
                      double height, const CanvasDrawOptions &options) = 0;
    virtual void circle(CanvasId canvas, double x, double y, double radius,
                        const CanvasDrawOptions &options) = 0;
    virtual void text(CanvasId canvas, double x, double y,
                      const std::string &text,
                      const CanvasDrawOptions &options) = 0;
};

} // namespace plat
