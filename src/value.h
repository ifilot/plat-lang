#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace plat {

/**
 * Represents the runtime `niks` value.
 */
class NilValue {};

class TableValue;
class NativeHandleValue;

/**
 * Runtime value used by the interpreter.
 */
class Value {
private:
    using Data = std::variant<NilValue, double, bool, std::shared_ptr<std::string>,
                              std::shared_ptr<TableValue>,
                              std::shared_ptr<NativeHandleValue>>;

    Data data_;

public:
    /**
     * Creates a `niks` value.
     */
    Value();

    /**
     * Creates a number value.
     *
     * @param value Number value.
     */
    explicit Value(double value);

    /**
     * Creates a boolean value.
     *
     * @param value Boolean value.
     */
    explicit Value(bool value);

    /**
     * Creates a mutable string value.
     *
     * @param value String value.
     */
    explicit Value(std::string value);

    /**
     * Creates a table value.
     *
     * @param value Table storage.
     */
    explicit Value(std::shared_ptr<TableValue> value);

    /**
     * Creates a native host-backed handle value.
     *
     * @param value Native handle storage.
     */
    explicit Value(std::shared_ptr<NativeHandleValue> value);

    /**
     * Returns true when this value is `niks`.
     *
     * @return True for `niks`.
     */
    bool is_nil() const;

    /**
     * Returns true when this value is a number.
     *
     * @return True for numbers.
     */
    bool is_number() const;

    /**
     * Returns true when this value is a boolean.
     *
     * @return True for booleans.
     */
    bool is_bool() const;

    /**
     * Returns true when this value is a string.
     *
     * @return True for strings.
     */
    bool is_string() const;

    /**
     * Returns true when this value is a table.
     *
     * @return True for tables.
     */
    bool is_table() const;

    /**
     * Returns true when this value is a native host-backed handle.
     *
     * @return True for native handles.
     */
    bool is_native_handle() const;

    /**
     * Returns the number value.
     *
     * @return Number value.
     */
    double as_number() const;

    /**
     * Returns the boolean value.
     *
     * @return Boolean value.
     */
    bool as_bool() const;

    /**
     * Returns the string storage.
     *
     * @return Mutable string storage.
     */
    const std::shared_ptr<std::string> &as_string() const;

    /**
     * Returns the table storage.
     *
     * @return Mutable table storage.
     */
    const std::shared_ptr<TableValue> &as_table() const;

    /**
     * Returns the native handle storage.
     *
     * @return Native handle storage.
     */
    const std::shared_ptr<NativeHandleValue> &as_native_handle() const;

    /**
     * Returns whether the value is truthy.
     *
     * @return Truthiness according to plat-lang rules.
     */
    bool is_truthy() const;

    /**
     * Converts the value to a user-facing string.
     *
     * @return String representation.
     */
    std::string to_string() const;
};

/**
 * Table key type accepted by plat-lang tables.
 */
class TableKey {
private:
    std::variant<double, std::string> value_;

public:
    /**
     * Creates a numeric zero table key.
     */
    TableKey();

    /**
     * Creates a numeric table key.
     *
     * @param value Numeric key.
     */
    explicit TableKey(double value);

    /**
     * Creates a string table key.
     *
     * @param value String key.
     */
    explicit TableKey(std::string value);

    /**
     * Returns the underlying key variant.
     *
     * @return Key variant.
     */
    const std::variant<double, std::string> &value() const;

    /**
     * Compares two table keys for equality.
     *
     * @param other Key to compare with.
     * @return True when equal.
     */
    bool operator==(const TableKey &other) const;
};

/**
 * Hash implementation for table keys.
 */
class TableKeyHash {
public:
    /**
     * Computes a hash code for a table key.
     *
     * @param key Key to hash.
     * @return Hash code.
     */
    std::size_t operator()(const TableKey &key) const;
};

/**
 * Opaque native host-backed value with identity semantics.
 */
class NativeHandleValue {
private:
    std::string kind_;
    std::string type_name_;
    std::size_t id_;

public:
    NativeHandleValue(std::string kind, std::string type_name, std::size_t id);

    const std::string &kind() const;
    const std::string &type_name() const;
    std::size_t id() const;
    std::string debug_string() const;
};

/**
 * Mutable table storage with identity semantics.
 */
class TableValue {
private:
    std::unordered_map<TableKey, Value, TableKeyHash> entries_;

public:
    /**
     * Stores a value under a key.
     *
     * @param key Table key.
     * @param value Value to store.
     */
    void set(const TableKey &key, Value value);

    /**
     * Looks up a value by key.
     *
     * @param key Table key.
     * @return Stored value, or `niks` when missing.
     */
    Value get(const TableKey &key) const;

    /**
     * Returns whether the table is empty.
     *
     * @return True when the table has no entries.
     */
    bool empty() const;

    /**
     * Returns all stored entries.
     *
     * @return Table entries.
     */
    const std::unordered_map<TableKey, Value, TableKeyHash> &entries() const;
};

/**
 * Converts a runtime value to a valid table key when possible.
 *
 * @param value Runtime value.
 * @param key Output table key.
 * @return True when conversion succeeds.
 */
bool value_to_table_key(const Value &value, TableKey &key);

/**
 * Compares two runtime values for plat-lang equality.
 *
 * @param left Left value.
 * @param right Right value.
 * @return True when values are equal.
 */
bool values_equal(const Value &left, const Value &right);

} // namespace plat
