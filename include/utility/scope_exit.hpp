// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1 2022-02
// v0.2 2025-10

#ifndef scope_exit_hpp__
#define scope_exit_hpp__

#include "config.h"
#include <functional>

namespace UTILITY_NAMESPACE {

class scope_exit {
protected:
    std::function<void()> _closure;

private:
    scope_exit(scope_exit const&) = delete;
    scope_exit(scope_exit&& other) = delete;
    scope_exit& operator=(scope_exit const&) = delete;
    scope_exit& operator=(scope_exit&& other) = delete;

public:
    explicit scope_exit(const std::function<void()> const& closure)
        : _closure(closure) {
    }

    ~scope_exit() noexcept {
        if (is_active())
            invoke();
    }

    void assign(const std::function<void()> const& closure) noexcept {
        _closure = closure;
    }

    // Deactivated without execution
    void dismiss() noexcept {
        _closure = {};
    }

    void invoke() noexcept {
        if (is_active()) {
            try {
                _closure();
            } catch (...) {
            }
        }
        dismiss();
    }

    bool is_active() const noexcept {
        return !!_closure;
    }
};

} // UTILITY_NAMESPACE

#define UTILITY_SCOPE_EXIT              UTILITY_NAMESPACE::scope_exit ANONYMOUS_SCOPE_EXIT
#define UTILITY_SCOPE_EXIT_DISMISS      ANONYMOUS_SCOPE_EXIT.dismiss

#endif // scope_exit_hpp__
