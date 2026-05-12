#pragma once

namespace plat {

class BuiltinRegistry;

/**
 * Registers the language's always-available core built-ins.
 */
void register_core_builtins(BuiltinRegistry &registry);

} // namespace plat
