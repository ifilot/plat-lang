#pragma once

#include "canvas/canvas_backend.h"
#include "value.h"

#include <memory>
#include <stdexcept>
#include <string>

namespace plat {

class CanvasError final : public std::runtime_error {
public:
    explicit CanvasError(const std::string &message);
};

class CanvasService {
private:
    std::unique_ptr<CanvasBackend> backend_;
    CanvasId active_canvas_ = 0;

public:
    CanvasService();
    explicit CanvasService(std::unique_ptr<CanvasBackend> backend);

    void set_backend(std::unique_ptr<CanvasBackend> backend);
    bool has_backend() const;

    std::shared_ptr<NativeHandleValue> create(double width, double height,
                                             const std::string &title);
    void clear(const NativeHandleValue &canvas, CanvasColor color);
    void present(const NativeHandleValue &canvas);
    void close(const NativeHandleValue &canvas);
    void wait_until_closed(const NativeHandleValue &canvas);
    void pause(const NativeHandleValue &canvas, double milliseconds);
    bool is_open(const NativeHandleValue &canvas) const;
    void line(const NativeHandleValue &canvas, double x1, double y1, double x2,
              double y2, const CanvasDrawOptions &options);
    void rect(const NativeHandleValue &canvas, double x, double y, double width,
              double height, const CanvasDrawOptions &options);
    void circle(const NativeHandleValue &canvas, double x, double y,
                double radius, const CanvasDrawOptions &options);
    void text(const NativeHandleValue &canvas, double x, double y,
              const std::string &text, const CanvasDrawOptions &options);

private:
    CanvasBackend &backend();
    const CanvasBackend &backend() const;
    CanvasId checked_id(const NativeHandleValue &canvas) const;
};

} // namespace plat
