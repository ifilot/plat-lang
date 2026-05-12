#include "test_helpers.h"

#include "builtins/builtin_registry.h"
#include "builtins/core_builtins.h"
#include "diagnostics.h"

#include <sstream>
#include <vector>

namespace plat::test {

void run_builtin_registry_tests(TestContext &context) {
    BuiltinRegistry registry;
    register_core_builtins(registry);

    EXPECT_TRUE(context, registry.contains("aafdrokke"));
    EXPECT_TRUE(context, registry.contains("aafdrökke"));
    EXPECT_TRUE(context, registry.contains("waatis"));
    EXPECT_FALSE(context, registry.contains("neit_bestaond"));

    std::stringstream input;
    std::stringstream output;
    std::stringstream errors;
    DiagnosticReporter diagnostics(Language::English, errors);
    BuiltinContext builtin_context(diagnostics, input, output);

    registry.call(builtin_context, "aafdrokke",
                  std::vector<Value>{Value(std::string("hoi"))},
                  SourceLocation());

    EXPECT_EQ(context, output.str(), "hoi\n");
}

} // namespace plat::test
