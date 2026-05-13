#include "builtins/canvas_builtins.h"

#include "builtins/builtin_registry.h"
#include "canvas/canvas_service.h"
#include "canvas/sdl_canvas_backend.h"

#include <memory>

namespace {

std::unique_ptr<plat::CanvasService> canvas_service;

} // namespace

extern "C" bool platlang_register_capability_v1(
    plat::BuiltinRegistry *registry) {
    if (registry == nullptr) {
        return false;
    }

    if (canvas_service == nullptr) {
        canvas_service = std::make_unique<plat::CanvasService>(
            std::make_unique<plat::SdlCanvasBackend>());
    }

    plat::register_canvas_builtins(*registry, *canvas_service);
    return true;
}
