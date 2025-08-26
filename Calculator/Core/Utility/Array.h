//
// Created by scion on 6/7/2025.
//

#pragma once

#include "Macro.h"
#include "Math.h"

#include <assert.h>
#include <ranges>

template<class T>
using Range = std::ranges::subrange<T>;

template<typename T>
class Array
{
public:
	static constexpr bool IS_VOID = std::is_same_v<T, void>;
	using I = std::conditional_t<IS_VOID, std::byte, T>;

	constexpr Array() = default;

	Array(const std::initializer_list<I>& init) : _size(init.size())
	{
		if(_size) _t = new I[_size];
		for(size_t i = 0; i < _size; i++)
			_t[i] = *(init.begin() + i);
	}

	template<typename... ARGS>
	explicit Array(size_t count, ARGS&&... args) : _size(count), _t(nullptr)
	{
		if(_size) _t = new I[count];
		if constexpr (sizeof...(ARGS) && !IS_VOID)
			for(size_t i = 0; i < count; i++) _t[i] = I(std::forward<ARGS>(args)...);
	}

	Array(size_t count, const T* t, bool createBacking = true) : _size(count), _t(nullptr)
	{
		if(t) assert(createBacking);
		if(_size && createBacking) _t = new I[count];

		if(!t || !createBacking) return;
		if constexpr(std::is_trivially_copyable_v<I>)
			memcpy(_t, t, count * sizeof(I));
		else
			for(size_t i = 0; i < count; i++)
				_t[i] = t[i];
	}

	OPERATOR_COPY_NOSUPER(Array, Free,
	{
		_size = o._size;

		if(!_size || !o._t) return;

		_t = new I[_size];
		if constexpr(std::is_trivially_copyable_v<I>)
			memcpy(_t, o._t, _size * sizeof(I));
		else
			for(size_t i = 0; i < _size; i++)
				_t[i] = o._t[i];
	});

	OPERATOR_MOVE_NOSUPER(Array, Free,
	{
		_size = o._size;
		_t = o._t;
		o._t = nullptr;
	});

	size_t CopyToCArray(I* arr, size_t arrSize) const;

	template<size_t COUNT>
	ALWAYS_INLINE size_t CopyToCArray(I(& arr)[COUNT]) const { return CopyToCArray(arr, COUNT); }

	NODISCARD ALWAYS_INLINE u64 Size() const { return _size; }
	NODISCARD ALWAYS_INLINE u64 ByteSize() const { return _size * sizeof(T); }
	NODISCARD ALWAYS_INLINE I* Data() { return _t; }
	NODISCARD ALWAYS_INLINE const I* Data() const { return _t; }

	NODISCARD ALWAYS_INLINE I& operator[](u64 i) const { assert(_t); return _t[i]; }

	NODISCARD ALWAYS_INLINE bool IsFree() const { return !_t; }
	ALWAYS_INLINE void Free() { delete[] _t; _t = nullptr; }

	ALWAYS_INLINE operator bool() const { return _t; }
	ALWAYS_INLINE T* operator*() const { return _t; }

	T* begin() const { return _t; }
	T* end() const { return _t + _size; }

	~Array() { Free(); }

private:
	u64 _size = 0;
	I* _t = nullptr;
};

template<typename T>
size_t Array<T>::CopyToCArray(I* arr, size_t arrSize) const
{
	size_t copyCount = std::min(arrSize, _size);

	if constexpr(std::is_trivially_copyable_v<I>)
		memcpy(arr, _t, copyCount * sizeof(I));
	else
		for(size_t i = 0; i < copyCount; i++)
			arr[i] = _t[i];

	return copyCount;
}