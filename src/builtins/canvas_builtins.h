#pragma once

namespace plat {

class BuiltinRegistry;
class CanvasService;

/**
 * Registers canvas host-capability built-ins.
 */
void register_canvas_builtins(BuiltinRegistry &registry,
                              CanvasService &canvas_service);

} // namespace plat
