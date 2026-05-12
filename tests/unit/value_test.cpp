#include "test_helpers.h"

#include "value.h"

#include <memory>
#include <string>

namespace plat::test {

void run_value_tests(TestContext &context) {
    Value nil;
    EXPECT_TRUE(context, nil.is_nil());
    EXPECT_FALSE(context, nil.is_truthy());
    EXPECT_EQ(context, nil.to_string(), "niks");

    Value zero(0.0);
    Value one(1.0);
    EXPECT_FALSE(context, zero.is_truthy());
    EXPECT_TRUE(context, one.is_truthy());

    Value empty_string(std::string(""));
    Value text(std::string("hoi"));
    EXPECT_FALSE(context, empty_string.is_truthy());
    EXPECT_TRUE(context, text.is_truthy());
    EXPECT_EQ(context, text.to_string(), "hoi");

    auto table = std::make_shared<TableValue>();
    Value empty_table(table);
    EXPECT_FALSE(context, empty_table.is_truthy());

    table->set(TableKey(0.0), Value(42.0));
    Value non_empty_table(table);
    EXPECT_TRUE(context, non_empty_table.is_truthy());
    EXPECT_TRUE(context, table->get(TableKey(0.0)).is_number());
    EXPECT_EQ(context, table->get(TableKey(0.0)).as_number(), 42.0);

    auto handle = std::make_shared<NativeHandleValue>("canvas",
                                                       "canvas", 7);
    Value handle_value(handle);
    EXPECT_TRUE(context, handle_value.is_native_handle());
    EXPECT_TRUE(context, handle_value.is_truthy());
    EXPECT_EQ(context, handle_value.to_string(), "canvas@7");
    EXPECT_TRUE(context, values_equal(handle_value, Value(handle)));
    EXPECT_FALSE(context, values_equal(handle_value,
                                       Value(std::make_shared<NativeHandleValue>(
                                           "canvas", "canvas", 7))));

    TableKey key;
    EXPECT_TRUE(context, value_to_table_key(Value(std::string("name")), key));
    EXPECT_FALSE(context, value_to_table_key(Value(true), key));
}

} // namespace plat::test
