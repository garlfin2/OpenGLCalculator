//
// Created by scion on 6/7/2025.
//

#pragma once

#include <Calculator/Core/Utility/Macro.h>
#include <Calculator/Core/Utility/Math.h>

#define GLOBJECT_CONSTRUCTOR(TYPE) \
    TYPE() = default; \
    TYPE(const TYPE&) = delete; \
    TYPE(TYPE&&) = default; \
    TYPE& operator=(const TYPE&) = delete; \
    TYPE& operator=(TYPE&&) = default;

class GLObject
{
public:
    GLObject() = default;
    GLObject(const GLObject&) = delete;
    GLObject& operator=(const GLObject&) = delete;

    GLObject(GLObject&& b) noexcept : ID(b.ID) { b.ID = 0; }
    GLObject& operator=(GLObject&& b) noexcept
    {
        if(&b == this)
            return *this;

        ID = b.ID;
        b.ID = 0;

        return *this;
    }


    GET(u32, ID, ID);

    virtual ~GLObject() { ID = 0; }

protected:
    u32 ID = 0;
};
