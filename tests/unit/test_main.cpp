#include "test_helpers.h"

/**
 * Runs all unit tests.
 *
 * @return Zero when all tests pass.
 */
int main() {
    plat::test::TestContext context;

    plat::test::run_value_tests(context);
    plat::test::run_diagnostics_tests(context);

    return context.failures() == 0 ? 0 : 1;
}
