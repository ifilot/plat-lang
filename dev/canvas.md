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

## Implementation Steps

The recommended implementation path should keep the interpreter changes small
and make the GUI backend the last part of the first milestone, not the first.

### Step 1: Extract built-ins into a registry

Move the current protected built-ins out of the interpreter and into a small
`src/builtins/` subsystem:

```text
src/
  builtins/
    builtin_registry.h
    builtin_registry.cpp
    core_builtins.h
    core_builtins.cpp
```

The first version of the registry should preserve the existing behavior of
`aafdrokke`, `aafdrökke`, `invuier`, and `waatis`. This step should not add
canvas behavior yet. Its goal is to make built-ins pluggable while keeping
existing e2e tests green.

### Step 2: Add a native handle value

Extend the runtime `Value` model so built-ins can return opaque host-backed
handles. Canvas handles should not be modeled as numbers or tables. They should
have identity semantics, a stable debug string, and a localized type name for
`waatis`.

This gives `canvas(...)` a real return value without exposing SDL or backend
objects directly to user programs.

### Step 3: Define the canvas backend interface

Add the canvas subsystem without linking SDL:

```text
src/
  canvas/
    canvas_backend.h
    canvas_service.h
    canvas_service.cpp
```

The backend interface should describe the chosen first-version behavior:

```text
create(width, height, title)
clear(canvas, color)
present(canvas)
close(canvas)
wait_until_closed(canvas)
pause(canvas, milliseconds)
is_open(canvas)
line(canvas, x1, y1, x2, y2, options)
rect(canvas, x, y, width, height, options)
circle(canvas, x, y, radius, options)
text(canvas, x, y, text, options)
```

The service can enforce one active canvas in the first implementation. It
should also own the "canvas support is not available in this build" error path
for builds without a real backend.

### Step 4: Implement a recording backend

Add `RecordingCanvasBackend` before the SDL backend:

```text
src/
  canvas/
    recording_canvas_backend.h
    recording_canvas_backend.cpp
```

This backend should store structured operations in memory. Unit tests can then
verify argument conversion, option parsing, strict unknown-key errors, color
parsing, and operation order without opening a GUI window.

### Step 5: Register canvas built-ins

Add the canvas built-in module:

```text
src/
  builtins/
    canvas_builtins.h
    canvas_builtins.cpp
```

The final user-facing names should be Limburgish. English names can remain in
design notes or temporary development-only aliases, but they should not become
the committed public API unless that decision is made explicitly.

The built-ins should validate:

- arity
- canvas-handle arguments
- numeric coordinates and sizes
- string colors and titles
- optional `portefeuil` option tables
- unknown option keys as runtime errors

At the end of this step, canvas calls can be fully tested through the recording
backend.

### Step 6: Add build configuration

Add CMake targets and options for canvas support:

```text
PLATLANG_ENABLE_CANVAS
PLATLANG_ENABLE_SDL_CANVAS
```

The canvas core and recording backend can build everywhere. The SDL backend
should be compiled only when SDL2 and SDL_ttf are available and the flag is
enabled.

During this phase, the canvas implementation may still be linked normally into
the interpreter for development. SDL headers and SDL lifecycle code must remain
outside the interpreter.

### Step 7: Implement the SDL backend

Add the real windowing backend:

```text
src/
  canvas/
    sdl_canvas_backend.h
    sdl_canvas_backend.cpp
```

The SDL backend should:

- initialize SDL lazily on the first canvas operation that needs it
- create and show the window immediately when `canvas(...)` is called
- keep drawing buffered until the explicit present built-in
- keep the window responsive during wait and pause operations
- support one active canvas for the first version
- report clear runtime errors for closed or invalid canvases

Primitive support should include clear, line, rectangle, circle, and text.
Circles can use a small local rasterization helper rather than an additional
dependency.

### Step 8: Parse colors and drawing options

Centralize option parsing in the canvas subsystem. The first version should
support predefined color names and hex strings such as `"#ff0000"`.

Initial option keys:

```text
fill
stroke
color
width
size
```

Unknown keys should fail loudly. Missing options should use simple defaults
that make each primitive visible.

### Step 9: Embed Space Mono

Add Space Mono as the default text font:

```text
third_party/fonts/space_mono/
  SpaceMono-Regular.ttf
  OFL.txt

generated/
  space_mono_regular_ttf.h
  space_mono_regular_ttf.cpp
```

A build step should generate the C++ byte array. The SDL backend should load
the font from memory with SDL_ttf so `canvas_text` works without requiring a
font path from the user.

### Step 10: Add tests and examples

Add focused unit tests for:

- built-in registry lookup and protected names
- native handle values
- canvas arity and type errors
- strict option parsing
- color parsing
- recording backend operation order

Add e2e tests for disabled-canvas diagnostics and basic canvas call failures.
GUI behavior can be tested manually at first, with an opt-in SDL smoke test
later.

### Step 11: Split canvas into a native capability module

Once the linked implementation is stable, add the native capability ABI and
loader:

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

Build canvas as:

```text
platlang_canvas_sdl.dll
libplatlang_canvas_sdl.so
libplatlang_canvas_sdl.dylib
```

Load it from a `capabilities/canvas_sdl/` directory using a manifest. This
phase should produce clear diagnostics for missing and ABI-incompatible canvas
support.

### Step 12: Add an optional continuously updated develop release

Add a CI release channel that publishes a fresh downloadable build whenever a
commit lands on the `develop` branch. This should be treated as an unstable
developer build, not as the stable release line.

A possible shape:

```text
develop branch commit
  -> CI builds platlang for supported platforms
  -> CI packages the executable and native capabilities
  -> CI updates a moving "develop" release or build artifact
  -> users can download the latest development build
```

The package should include everything needed to try canvas out of the box:

```text
platlang
capabilities/
  canvas_sdl/
    manifest.json
    platlang_canvas_sdl.dll / libplatlang_canvas_sdl.so / libplatlang_canvas_sdl.dylib
    SDL runtime files when needed
```

The CI job should make the channel clearly identifiable:

- version output should include the branch and commit hash
- download names should include `develop` and the target platform
- documentation should call it unstable and suitable for testing
- stable tagged releases should remain separate from the moving develop build

This gives users an easy way to try the newest canvas work without waiting for
a formal versioned release.

### Step 13: Defer source modules

Do not add source-level import syntax for canvas yet. Canvas should remain a
host capability until the language and standard library need user-facing source
modules.

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
