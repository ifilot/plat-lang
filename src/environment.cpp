#include "environment.h"

#include <utility>

namespace plat {

Environment::Environment(
    std::shared_ptr<const std::unordered_set<std::string>> protected_names,
    DiagnosticReporter &diagnostics)
    : protected_names_(std::move(protected_names)), diagnostics_(&diagnostics) {}

Environment::Environment(std::shared_ptr<Environment> parent)
    : parent_(std::move(parent)),
      protected_names_(parent_->protected_names_),
      diagnostics_(parent_->diagnostics_) {}

void Environment::define(const std::string &name, Value value,
                         SourceLocation location) {
    if (is_protected(name)) {
        report_protected(name, location);
    }

    if (contains_current(name)) {
        diagnostics_->fatal(
            DiagnosticId::RedeclaredName, location,
            {DiagnosticArg("name", name)});
    }

    values_[name] = std::move(value);
}

void Environment::assign(const std::string &name, Value value,
                         SourceLocation location) {
    if (is_protected(name)) {
        report_protected(name, location);
    }

    auto found = values_.find(name);
    if (found != values_.end()) {
        found->second = std::move(value);
        return;
    }

    if (parent_ != nullptr) {
        parent_->assign(name, std::move(value), location);
        return;
    }

    diagnostics_->fatal(DiagnosticId::UndeclaredVariable, location,
                        {DiagnosticArg("name", name)});
}

Value Environment::lookup(const std::string &name, SourceLocation location) const {
    const auto found = values_.find(name);
    if (found != values_.end()) {
        return found->second;
    }

    if (parent_ != nullptr) {
        return parent_->lookup(name, location);
    }

    diagnostics_->fatal(DiagnosticId::UndeclaredVariable, location,
                        {DiagnosticArg("name", name)});
}

bool Environment::contains_current(const std::string &name) const {
    return values_.contains(name);
}

bool Environment::contains_anywhere(const std::string &name) const {
    if (contains_current(name)) {
        return true;
    }

    return parent_ != nullptr && parent_->contains_anywhere(name);
}

bool Environment::is_protected(std::string_view name) const {
    return protected_names_ != nullptr &&
           protected_names_->contains(std::string(name));
}

std::shared_ptr<Environment> Environment::make_child() {
    return std::make_shared<Environment>(shared_from_this());
}

[[noreturn]] void Environment::report_protected(
    const std::string &name, SourceLocation location) const {
    diagnostics_->fatal(DiagnosticId::ProtectedName, location,
                        {DiagnosticArg("name", name)});
}

} // namespace plat
