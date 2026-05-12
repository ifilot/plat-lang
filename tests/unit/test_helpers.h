#pragma once

#include <iostream>
#include <string>

namespace plat::test {

/**
 * Stores aggregate unit-test results.
 */
class TestContext {
private:
    int failures_ = 0;

public:
    /**
     * Records an expectation result.
     *
     * @param passed Whether the expectation passed.
     * @param message Failure message.
     * @param file Source file of the expectation.
     * @param line Source line of the expectation.
     */
    void expect(bool passed, const std::string &message, const char *file,
                int line) {
        if (passed) {
            return;
        }

        ++failures_;
        std::cerr << file << ":" << line << ": " << message << '\n';
    }

    /**
     * Returns the number of failed expectations.
     *
     * @return Failure count.
     */
    int failures() const { return failures_; }
};

/**
 * Runs value-related unit tests.
 *
 * @param context Test context.
 */
void run_value_tests(TestContext &context);

/**
 * Runs diagnostic-related unit tests.
 *
 * @param context Test context.
 */
void run_diagnostics_tests(TestContext &context);

/**
 * Runs built-in registry unit tests.
 *
 * @param context Test context.
 */
void run_builtin_registry_tests(TestContext &context);

/**
 * Runs canvas subsystem unit tests.
 *
 * @param context Test context.
 */
void run_canvas_tests(TestContext &context);

} // namespace plat::test

#define EXPECT_TRUE(context, expr) \
    (context).expect(static_cast<bool>(expr), "expected true: " #expr, __FILE__, __LINE__)

#define EXPECT_FALSE(context, expr) \
    (context).expect(!(expr), "expected false: " #expr, __FILE__, __LINE__)

#define EXPECT_EQ(context, left, right) \
    (context).expect((left) == (right), "expected equality: " #left " == " #right, __FILE__, __LINE__)
