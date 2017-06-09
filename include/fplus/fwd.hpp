// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/fplus.hpp>

namespace fplus
{
namespace fwd
{

// Partial currying.
// Allow to generically bind all but parameters except the last one.
// The lambda paramter ist named fplus_fwd_x instead of x
// because gcc can produce unjustified shadow warnings. see:
// http://stackoverflow.com/questions/41208811/parameter-of-returned-generic-lambda-allegedly-shadows-parameter-of-free-functio
#define fplus_fwd_define_fn_0(fplus_fwd_define_fn_0_name) \
inline auto fplus_fwd_define_fn_0_name() \
{ \
    return [](const auto& fplus_fwd_x) \
    { \
        return fplus::fplus_fwd_define_fn_0_name(fplus_fwd_x); \
    }; \
}

#define fplus_fwd_define_fn_1(fplus_fwd_define_fn_1_name) \
template <typename P1> \
auto fplus_fwd_define_fn_1_name(P1 p1) \
{ \
    return [p1](const auto& fplus_fwd_x) \
    { \
        return fplus::fplus_fwd_define_fn_1_name(p1, fplus_fwd_x); \
    }; \
}

#define fplus_fwd_define_fn_2(fplus_fwd_define_fn_2_name) \
template <typename P1, typename P2> \
auto fplus_fwd_define_fn_2_name(P1 p1, P2 p2) \
{ \
    return [p1, p2](const auto& fplus_fwd_x) \
    { \
        return fplus::fplus_fwd_define_fn_2_name(p1, p2, fplus_fwd_x); \
    }; \
}

#define fplus_fwd_define_fn_3(fplus_fwd_define_fn_3_name) \
template <typename P1, typename P2, typename P3> \
auto fplus_fwd_define_fn_3_name(P1 p1, P2 p2, P3 p3) \
{ \
    return [p1, p2, p3](const auto& fplus_fwd_x) \
    { \
        return fplus::fplus_fwd_define_fn_3_name(p1, p2, p3, fplus_fwd_x); \
    }; \
}

#define fplus_fwd_define_fn_4(fplus_fwd_define_fn_4_name) \
template <typename P1, typename P2, typename P3, typename P4> \
auto fplus_fwd_define_fn_4_name(P1 p1, P2 p2, P3 p3, P4 p4) \
{ \
    return [p1, p2, p3, p4](const auto& fplus_fwd_x) \
    { \
        return fplus::fplus_fwd_define_fn_4_name(p1, p2, p3, p4, fplus_fwd_x); \
    }; \
}


#define fplus_fwd_flip_define_fn_1(fplus_fwd_flip_define_fn_1_name) \
namespace flip \
{ \
    template <typename P2> \
    auto fplus_fwd_flip_define_fn_1_name(P2 p2) \
    { \
        return [p2](const auto& fplus_fwd_flip_x) \
        { \
            return fplus::fplus_fwd_flip_define_fn_1_name(fplus_fwd_flip_x, p2); \
        }; \
    } \
} // namespace flip


namespace internal
{
    template<typename F, typename G>
    struct compose_helper{
        compose_helper(F f, G g) : f_(f), g_(g) {}
        template<typename X>
        decltype(auto) operator()(const X& x) const
        {
            return g_(f_(x));
        }
    private:
        F f_;
        G g_;
    };
} // namespace internal
template<typename F, typename G>
auto compose(F f, G g) {
    return internal::compose_helper<F, G> {f, g};
}
template<typename F1, typename... Fs>
auto compose(F1 f, Fs ... args)
{
    return compose(f, compose(args...));
}

template<typename X, typename... Fs>
auto apply(const X& x, Fs ... args)
{
    return compose(args...)(x);
}
template<typename X, typename F>
auto apply(const X& x, F f)
{
    return f(x);
}

#include "fwd_instances.autogenerated_defines"

} // namespace fwd
} // namespace fplus
