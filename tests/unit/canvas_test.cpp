#include "test_helpers.h"

#include "builtins/builtin_registry.h"
#include "builtins/canvas_builtins.h"
#include "canvas/canvas_service.h"
#include "canvas/recording_canvas_backend.h"
#include "diagnostics.h"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace plat::test {
namespace {

Value call_canvas_builtin(BuiltinRegistry &registry, BuiltinContext &context,
                          const std::string &name,
                          const std::vector<Value> &args) {
    return registry.call(context, name, args, SourceLocation(3, 4));
}

std::shared_ptr<TableValue> make_options() {
    auto table = std::make_shared<TableValue>();
    table->set(TableKey(std::string("vulling")), Value(std::string("#ff0000")));
    table->set(TableKey(std::string("rand")), Value(std::string("black")));
    table->set(TableKey(std::string("breide")), Value(3.0));
    return table;
}

} // namespace

void run_canvas_tests(TestContext &context) {
    BuiltinRegistry registry;
    EXPECT_FALSE(context, registry.contains("canvas"));

    std::stringstream input;
    std::stringstream output;
    std::stringstream errors;
    DiagnosticReporter diagnostics(Language::English, errors);

    auto backend = std::make_unique<RecordingCanvasBackend>();
    RecordingCanvasBackend *recording = backend.get();
    CanvasService canvas_service(std::move(backend));
    BuiltinContext builtin_context(diagnostics, input, output);

    register_canvas_builtins(registry, canvas_service);
    EXPECT_TRUE(context, registry.contains("canvas"));
    EXPECT_TRUE(context, registry.contains("canvas_rechhook"));

    Value canvas = call_canvas_builtin(
        registry, builtin_context, "canvas",
        {Value(100.0), Value(80.0), Value(std::string("Demo"))});

    EXPECT_TRUE(context, canvas.is_native_handle());
    EXPECT_EQ(context, canvas.as_native_handle()->type_name(), "canvas");

    call_canvas_builtin(registry, builtin_context, "canvas_wis",
                        {canvas, Value(std::string("white"))});
    call_canvas_builtin(registry, builtin_context, "canvas_rechhook",
                        {canvas, Value(10.0), Value(20.0), Value(30.0),
                         Value(40.0), Value(make_options())});
    call_canvas_builtin(registry, builtin_context, "canvas_pad",
                        {canvas,
                         Value(std::string("m 0,0 c 10,0 10,10 20,10 z")),
                         Value(make_options())});
    Value open = call_canvas_builtin(registry, builtin_context, "canvas_ope",
                                     {canvas});

    EXPECT_TRUE(context, open.is_bool());
    EXPECT_TRUE(context, open.as_bool());

    const std::vector<CanvasOperation> &operations = recording->operations();
    EXPECT_EQ(context, operations.size(), static_cast<std::size_t>(4));
    EXPECT_TRUE(context, operations[0].kind == CanvasOperationKind::Create);
    EXPECT_EQ(context, operations[0].numbers[0], 100.0);
    EXPECT_EQ(context, operations[0].text, "Demo");
    EXPECT_TRUE(context, operations[1].kind == CanvasOperationKind::Clear);
    EXPECT_TRUE(context,
                operations[1].color == (CanvasColor{255, 255, 255, 255}));
    EXPECT_TRUE(context, operations[2].kind == CanvasOperationKind::Rect);
    EXPECT_EQ(context, operations[2].options.width, 3.0);
    EXPECT_TRUE(context, operations[2].options.fill.has_value());
    EXPECT_TRUE(context,
                operations[2].options.fill.value() ==
                    (CanvasColor{255, 0, 0, 255}));
    EXPECT_TRUE(context, operations[3].kind == CanvasOperationKind::Path);
    EXPECT_EQ(context, operations[3].paths.size(), static_cast<std::size_t>(1));
    EXPECT_TRUE(context, operations[3].paths[0].closed);
    EXPECT_TRUE(context, operations[3].paths[0].points.size() > 3);

    auto bad_options = std::make_shared<TableValue>();
    bad_options->set(TableKey(std::string("fill")), Value(1.0));

    try {
        call_canvas_builtin(registry, builtin_context, "canvas_lien",
                            {canvas, Value(1.0), Value(2.0), Value(3.0),
                             Value(4.0), Value(bad_options)});
        EXPECT_TRUE(context, false);
    } catch (const PlatlangError &error) {
        EXPECT_TRUE(context, error.id() == DiagnosticId::RuntimeError);
        EXPECT_TRUE(context,
                    std::string(error.what()).find("unknown canvas option") !=
                        std::string::npos);
    }
}

} // namespace plat::test
