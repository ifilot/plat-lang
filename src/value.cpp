#include "value.h"

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace plat {

Value::Value() : data_(NilValue()) {}

Value::Value(double value) : data_(value) {}

Value::Value(bool value) : data_(value) {}

Value::Value(std::string value)
    : data_(std::make_shared<std::string>(std::move(value))) {}

Value::Value(std::shared_ptr<TableValue> value) : data_(std::move(value)) {}

bool Value::is_nil() const {
    return std::holds_alternative<NilValue>(data_);
}

bool Value::is_number() const {
    return std::holds_alternative<double>(data_);
}

bool Value::is_bool() const {
    return std::holds_alternative<bool>(data_);
}

bool Value::is_string() const {
    return std::holds_alternative<std::shared_ptr<std::string>>(data_);
}

bool Value::is_table() const {
    return std::holds_alternative<std::shared_ptr<TableValue>>(data_);
}

double Value::as_number() const {
    return std::get<double>(data_);
}

bool Value::as_bool() const {
    return std::get<bool>(data_);
}

const std::shared_ptr<std::string> &Value::as_string() const {
    return std::get<std::shared_ptr<std::string>>(data_);
}

const std::shared_ptr<TableValue> &Value::as_table() const {
    return std::get<std::shared_ptr<TableValue>>(data_);
}

bool Value::is_truthy() const {
    if (is_nil()) {
        return false;
    }

    if (is_bool()) {
        return as_bool();
    }

    if (is_number()) {
        return as_number() != 0.0;
    }

    if (is_string()) {
        return !as_string()->empty();
    }

    if (is_table()) {
        return !as_table()->empty();
    }

    return true;
}

std::string Value::to_string() const {
    if (is_nil()) {
        return "niks";
    }

    if (is_bool()) {
        return as_bool() ? "woar" : "neetwoar";
    }

    if (is_number()) {
        std::ostringstream out;
        out << as_number();
        return out.str();
    }

    if (is_string()) {
        return *as_string();
    }

    if (is_table()) {
        std::ostringstream out;
        out << "portefeuil@" << as_table().get();
        return out.str();
    }

    throw std::logic_error("unknown value type");
}

TableKey::TableKey() : value_(0.0) {}

TableKey::TableKey(double value) : value_(value) {}

TableKey::TableKey(std::string value) : value_(std::move(value)) {}

const std::variant<double, std::string> &TableKey::value() const {
    return value_;
}

bool TableKey::operator==(const TableKey &other) const {
    return value_ == other.value_;
}

std::size_t TableKeyHash::operator()(const TableKey &key) const {
    if (std::holds_alternative<double>(key.value())) {
        return std::hash<double>()(std::get<double>(key.value()));
    }

    return std::hash<std::string>()(std::get<std::string>(key.value()));
}

void TableValue::set(const TableKey &key, Value value) {
    entries_[key] = std::move(value);
}

Value TableValue::get(const TableKey &key) const {
    const auto found = entries_.find(key);
    if (found == entries_.end()) {
        return Value();
    }

    return found->second;
}

bool TableValue::empty() const {
    return entries_.empty();
}

bool value_to_table_key(const Value &value, TableKey &key) {
    if (value.is_number()) {
        key = TableKey(value.as_number());
        return true;
    }

    if (value.is_string()) {
        key = TableKey(*value.as_string());
        return true;
    }

    return false;
}

bool values_equal(const Value &left, const Value &right) {
    if (left.is_nil() || right.is_nil()) {
        return left.is_nil() && right.is_nil();
    }

    if (left.is_number() && right.is_number()) {
        return left.as_number() == right.as_number();
    }

    if (left.is_bool() && right.is_bool()) {
        return left.as_bool() == right.as_bool();
    }

    if (left.is_string() && right.is_string()) {
        return *left.as_string() == *right.as_string();
    }

    if (left.is_table() && right.is_table()) {
        return left.as_table() == right.as_table();
    }

    return false;
}

} // namespace plat
