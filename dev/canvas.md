# Canvas Design Notes

This document captures the canvas design discussion before implementation.

## Conversation Summary

We want users to be able to spawn a canvas from `plat-lang` and draw simple
things on it, including text and basic shapes.

The current language model favors a small set of protected built-in global
functions over new syntax, object methods, callbacks, or first-class function
APIs. Canvas support should follow that shape: a native host-backed canvas
handle returned by a built-in function, then additional global built-ins that
operate on that handle.

Example surface API:

```plat
loat c = canvas(640, 480, "Demo")

canvas_clear(c, "white")
canvas_rect(c, 40, 40, 180, 90, {
  "fill": "lightblue",
  "stroke": "navy"
})
canvas_circle(c, 320, 220, 60, {
  "fill": "tomato"
})
canvas_text(c, 50, 170, "Hello canvas", {
  "size": 28,
  "color": "black"
})
canvas_line(c, 20, 20, 400, 300, {
  "color": "gray",
  "width": 2
})

canvas_present(c)
canvas_wait(c)
```

The preferred host backend is SDL2 plus SDL_ttf. SDL2 gives us a small
cross-platform native window, renderer, event loop, and primitive drawing API.
SDL_ttf covers text rendering. Circles can initially be implemented by our own
small rasterization helper rather than adding another dependency such as
SDL2_gfx.

Canvas code should live in a separate subsystem instead of being mixed into the
interpreter. A light source layout could be:

```text
src/
  builtins/
    builtin_registry.h
    builtin_registry.cpp
    core_builtins.h
    core_builtins.cpp
    canvas_builtins.h
    canvas_builtins.cpp

  canvas/
    canvas_backend.h
    recording_canvas_backend.h
    recording_canvas_backend.cpp
    sdl_canvas_backend.h
    sdl_canvas_backend.cpp
```

The dependency direction should stay clean:

```text
Interpreter
  -> BuiltinRegistry
    -> canvas_builtins
      -> CanvasBackend
        -> SdlCanvasBackend / RecordingCanvasBackend
```

The interpreter should know how to call built-ins, but it should not know how
to draw a rectangle.

Canvas support should be lazy. Normal command-line programs should not
initialize SDL or create SDL objects. A `CanvasService` or lazy backend wrapper
should construct `SdlCanvasBackend` only when the first canvas built-in actually
needs it. SDL support should also be optionally compiled with a CMake flag such
as `PLATLANG_ENABLE_SDL_CANVAS`.

When SDL support is disabled, canvas built-ins should fail with a clear runtime
error such as:

```text
canvas support is not available in this build
```

Testing should not require a GUI. A `RecordingCanvasBackend` should record
canvas operations for unit or golden tests.

## Proposed Built-ins

Initial candidate functions are shown with English placeholder names. The final
user-facing names should be Limburgish because `plat-lang` is a Limburgish
programming language.

```text
canvas(width, height, title) -> canvas handle
canvas_clear(canvas, color) -> niks
canvas_present(canvas) -> niks
canvas_wait(canvas) -> niks
canvas_close(canvas) -> niks

canvas_line(canvas, x1, y1, x2, y2, options?) -> niks
canvas_rect(canvas, x, y, width, height, options?) -> niks
canvas_circle(canvas, x, y, radius, options?) -> niks
canvas_text(canvas, x, y, text, options?) -> niks
```

Options should use `portefeuil` tables:

```plat
canvas_rect(c, 20, 20, 120, 80, {
  "fill": "red",
  "stroke": "black",
  "width": 3
})
```

## Design Questions

1. Should canvas built-in names be English-only at first, or should they also
   get Limburgish aliases immediately?

2. Should `canvas(width, height, title)` immediately show a window, or should
   the window appear only after the first `canvas_present(canvas)` call?

3. Should drawing operations update the visible window immediately, or should
   users explicitly call `canvas_present(canvas)` to display the accumulated
   frame?

4. Should the first version support only one active canvas, or multiple canvas
   windows at the same time?

5. What should `canvas_wait(canvas)` do exactly: wait until the user closes the
   window, wait for any input event, wait for a number of milliseconds, or should
   those be separate functions?

6. Should canvas coordinates be raw pixels with `(0, 0)` at the top-left, or do
   we want a more beginner-friendly coordinate system such as centered or
   bottom-left origin?

7. What color formats should be accepted in the first version: named colors,
   hex strings like `"#ff0000"`, RGB tables like `{ "r": 255, "g": 0, "b": 0 }`,
   or all of these?

8. How strict should option tables be: should unknown keys be ignored for
   forward compatibility, or reported as runtime errors to catch mistakes?

9. Should text rendering use a bundled/default font, require the user to pass a
   font path, or search common system fonts automatically?

10. Should canvas support be considered part of the default `platlang` binary,
    enabled when SDL is available, or should it be a separate build/profile such
    as `platlang-canvas`?

## Decisions

1. Canvas built-ins should use Limburgish names. English names are acceptable as
   temporary design placeholders only.

2. Creating a canvas should immediately show a window.

3. Drawing operations should not automatically display a new frame. The user
   must explicitly call the canvas-present function.

4. The first implementation should support one canvas.

5. Waiting/event behavior should be split into separate operations:

   ```text
   wait until the canvas closes
   pause for a number of milliseconds while keeping the window responsive
   ask whether the canvas is still open
   ```

6. Coordinates should use raw pixels with `(0, 0)` at the top-left.

7. Colors should support predefined values and hex strings such as `"#ff0000"`.

8. Unknown option-table keys should produce runtime errors.

9. Text rendering should use Space Mono as the recommended default font.

10. Canvas support should be architected as a clean separate native capability
    library rather than being deeply compiled into the interpreter. The exact
    loading/linking model still needs a final decision.

## Native Capability Modules

Canvas support should be treated as a native host capability, not as a
user-facing source module at first. This keeps the language design focused while
still allowing optional native features to be packaged cleanly.

There are two related but distinct concepts:

```text
Native capability modules
  DLL/shared libraries loaded by the interpreter. These expose host features
  such as canvas, audio, networking, or other platform services.

Language source modules
  `plat-lang` source files imported by user programs. These should wait until
  the standard library grows enough to justify user-facing import syntax.
```

Canvas should start as a native capability module. It should register built-ins
with the interpreter, but it should not force an import/module keyword into the
language yet.

Recommended packaging shape:

```text
platlang
capabilities/
  canvas_sdl/
    platlang_canvas_sdl.dll
    SDL2.dll
    SDL2_ttf.dll
```

On Linux and macOS the same concept maps to shared libraries:

```text
capabilities/
  canvas_sdl/
    libplatlang_canvas_sdl.so
```

or:

```text
capabilities/
  canvas_sdl/
    libplatlang_canvas_sdl.dylib
```

The interpreter should load the `platlang_canvas_sdl` module, not SDL directly.
The canvas module owns the SDL dependency and hides SDL details behind the
`plat-lang` capability boundary.

A native capability should expose a small C-compatible ABI rather than C++
classes directly across the DLL boundary. A possible shape:

```cpp
struct PlatlangCapabilityApi {
    uint32_t abi_version;
    const char *name;
    void (*register_builtins)(PlatlangHostApi *host);
    void (*shutdown)();
};

extern "C" PLATLANG_EXPORT PlatlangCapabilityApi *
platlang_capability_init(uint32_t host_abi_version);
```

The interpreter can pair this with a manifest for friendlier discovery and
errors:

```json
{
  "name": "canvas-sdl",
  "version": "0.1.0",
  "abi": 1,
  "library": "platlang_canvas_sdl.dll",
  "provides": ["canvas"]
}
```

This allows clear diagnostics such as:

```text
canvas support is installed but incompatible with this plat-lang version
```

or:

```text
canvas support is not installed
```

The recommended implementation path is phased:

```text
Phase 1
  Add a built-in registry in core.
  Put canvas code in a separate CMake library target.
  Link it normally during development.
  Keep all SDL headers and SDL lifecycle code out of the interpreter.

Phase 2
  Add a native capability loader.
  Build canvas as platlang_canvas_sdl.dll / libplatlang_canvas_sdl.so.
  Load it from a capabilities directory.
  Keep Space Mono embedded inside the canvas library.

Phase 3
  Add source/library modules for `plat-lang` files only when the language and
  standard library need a user-facing import mechanism.
```

The key principle is: canvas is a host capability, not a language module yet.
This gives clean packaging and optional native features without prematurely
adding import syntax to the language.

## Embedded Default Font

The canvas capability should embed Space Mono directly into the native canvas
library. This avoids shipping a separate font file and guarantees that
`canvas_text` works consistently out of the box.

Recommended implementation:

```text
third_party/fonts/space_mono/
  SpaceMono-Regular.ttf
  OFL.txt

generated/
  space_mono_regular_ttf.h
  space_mono_regular_ttf.cpp
```

A build step can generate a C++ byte array from the `.ttf` file. The SDL canvas
backend can then load it from memory with SDL_ttf:

```cpp
SDL_RWops *rw = SDL_RWFromConstMem(space_mono_regular_ttf,
                                   space_mono_regular_ttf_size);
TTF_Font *font = TTF_OpenFontRW(rw, 1, font_size);
```

The Space Mono license should be preserved in the repository and included in
distributions that ship the embedded font.
