#include "test_helpers.h"

#include "diagnostics.h"

#include <sstream>

namespace plat::test {

void run_diagnostics_tests(TestContext &context) {
    std::ostringstream english;
    DiagnosticReporter english_reporter(Language::English, english);
    english_reporter.error(DiagnosticId::UnknownFunction, SourceLocation(2, 5),
                           {DiagnosticArg("name", "missing")});

    EXPECT_TRUE(context, english_reporter.has_error());
    EXPECT_EQ(context, english.str(),
              "Line 2, Column 5: error: unknown function 'missing'\n");

    std::ostringstream limburgish;
    DiagnosticReporter limburgish_reporter(Language::Limburgish, limburgish);
    limburgish_reporter.error(DiagnosticId::UnknownFunction,
                              SourceLocation(2, 5),
                              {DiagnosticArg("name", "missing")});

    EXPECT_TRUE(context, limburgish_reporter.has_error());
    EXPECT_EQ(context, limburgish.str(),
              "Line 2, Column 5: fout: onbekende funksie 'missing'\n");

    EXPECT_TRUE(context, parse_language_tag("li").has_value());
    EXPECT_TRUE(context, parse_language_tag("li-sit").has_value());
    EXPECT_FALSE(context, parse_language_tag("xx").has_value());
}

} // namespace plat::test
