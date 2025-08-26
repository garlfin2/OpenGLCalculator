//
// Created by scion on 6/7/2025.
//

#pragma once

#ifdef __GNUC__
    #define GE_COMPILER_GCC
#endif

#ifdef _MSC_VER
    #define GE_COMPILER_MSVC
#endif

#ifdef DEBUG
    #define ALWAYS_INLINE inline
#else
    #ifdef GE_COMPILER_MSVC
        #define ALWAYS_INLINE __forceinline
    #endif
    #ifdef GE_COMPILER_GCC
        #define ALWAYS_INLINE inline __attribute__((always_inline))
    #endif
#endif

#define DEFAULT {}

#define NODISCARD [[nodiscard]]

#define GET_CONST(TYPE, ACCESSOR, FIELD) NODISCARD ALWAYS_INLINE TYPE Get##ACCESSOR() const { return FIELD; }
#define GET(TYPE, ACCESSOR, FIELD) \
    NODISCARD ALWAYS_INLINE TYPE Get##ACCESSOR() { return FIELD; } \
    GET_CONST(const TYPE, ACCESSOR, FIELD)

#define SET(TYPE, ACCESSOR, FIELD, ...) ALWAYS_INLINE void Set##ACCESSOR(TYPE ACCESSOR##_) { FIELD = __VA_ARGS__ ACCESSOR##_; }
#define SET_XVAL(TYPE, ACCESSOR, FIELD) ALWAYS_INLINE void Set##ACCESSOR(TYPE&& FIELD##_) { FIELD = std::move(FIELD##_); }

#define GET_SET(TYPE, ACCESSOR, FIELD) \
    GET_CONST(const TYPE, ACCESSOR, FIELD) \
    SET(TYPE, ACCESSOR, FIELD)

#define OPERATOR_MOVE_IMPL(NAMESPACE, TYPE, DESTRUCTOR, CODE) \
    NAMESPACE TYPE(TYPE&& o) noexcept \
    { \
        if(&o == this) return; \
        [&]() { CODE; }(); \
    } \
    TYPE& NAMESPACE operator=(TYPE&& o) noexcept \
    { \
        if(&o == this) return *this; \
        DESTRUCTOR; \
        [&]() { CODE; }(); \
        return* this; \
    }

#define OPERATOR_COPY_IMPL(NAMESPACE, TYPE, DESTRUCTOR, CODE) \
    NAMESPACE TYPE(const TYPE& o) \
    { \
        [&]() { CODE; }(); \
    } \
    TYPE& NAMESPACE operator=(const TYPE& o) \
    { \
        if(&o == this) return *this; \
        DESTRUCTOR; \
        [&]() { CODE; }(); \
        return *this; \
    }

#define OPERATOR_MOVE(TYPE, DESTRUCTOR, SUPER, CODE) \
    OPERATOR_MOVE_IMPL(, TYPE, SUPER::operator=(move(o)); TYPE::DESTRUCTOR(), CODE)

#define OPERATOR_COPY(TYPE, DESTRUCTOR, SUPER, CODE) \
    OPERATOR_COPY_IMPL(, TYPE, SUPER::operator=(o); TYPE::DESTRUCTOR(), CODE)

#define OPERATOR_MOVE_NOSUPER(TYPE, DESTRUCTOR, CODE) \
    OPERATOR_MOVE_IMPL(, TYPE, TYPE::DESTRUCTOR(), CODE)

#define OPERATOR_COPY_NOSUPER(TYPE, DESTRUCTOR, CODE) \
    OPERATOR_COPY_IMPL(, TYPE, TYPE::DESTRUCTOR(), CODE)
