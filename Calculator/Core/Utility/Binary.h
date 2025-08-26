//
// Created by scion on 6/10/2025.
//

#pragma once

#include <istream>
#include <type_traits>

#include "Macro.h"
#include "Array.h"

template<class T> requires std::is_trivially_copyable_v<T>
NODISCARD T Read(std::istream& stream)
{
    T t;
    stream.read((char*) &t, sizeof(T));
    return t;
}

template<class T> requires std::is_trivially_copyable_v<T>
NODISCARD Array<T> Read(std::istream& stream, u64 count)
{
    Array<T> arr(count);
    stream.read((char*) arr.Data(), sizeof(T) * count);
    return arr;
}
