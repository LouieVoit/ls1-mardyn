/**
 * \file
 * \brief AlignedArray.h
 */

#ifndef ALIGNEDARRAY_H_
#define ALIGNEDARRAY_H_

#ifdef __SSE3__
#include <xmmintrin.h>
#endif
#include <malloc.h>
#include <new>
#include <cstring>
#include <cassert>

#define CACHE_LINE_SIZE 64

// TODO: managing this class is becoming too complicated
// mainly because of the need to remember what happens when,
// since our functions don't follow standard naming rules
// and conventions stemming from std::vector.
//
// Switch to std::vector with a custom allocator
// so that it is at least clear what happens when.

/**
 * \brief An aligned array.
 * \details Has pointer to T semantics.
 * \tparam T The type of the array elements.
 * \tparam alignment The alignment restriction. Must be a power of 2, should not be 8.
 * \author Johannes Heckl, Nikola Tchipev
 */
template<class T, size_t alignment = CACHE_LINE_SIZE>
class AlignedArray {
public:
	/**
	 * \brief Construct an empty array.
	 */
	AlignedArray() :
			_capacity(0), _p(nullptr) {
	}

	/**
	 * \brief Construct an array of n elements.
	 */
	AlignedArray(size_t n) :
			_capacity(n), _p(_allocate(n)) {
		if (!_p)
			throw std::bad_alloc();
	}

	/**
	 * \brief Construct a copy of another AlignedArray.
	 */
	AlignedArray(const AlignedArray & a) :
			_capacity(a._capacity), _p(_allocate(a._capacity)) {
		if (!_p)
			throw std::bad_alloc();
		_assign(a);
	}

	/**
	 * \brief Assign a copy of another AlignedArray.
	 */
	AlignedArray & operator=(const AlignedArray & a) {
		resize(a._capacity);
		_assign(a);
		return *this;
	}

	/**
	 * \brief Free the array.
	 */
	virtual ~AlignedArray() {
		_free();
	}

	void appendValue(T v, size_t oldNumElements) {
		assert(oldNumElements <= _capacity);
		if(oldNumElements < _capacity) {
			// no need to resize
		} else {
			// shit, we need to resize, but also keep contents
			AlignedArray<T> backupCopy(*this);
			resize_zero_shrink(oldNumElements + 1);
			std::memcpy (_p, backupCopy._p, oldNumElements);
		}
		_p[oldNumElements] = v;
	}

	virtual size_t resize_zero_shrink(size_t exact_size, bool zero_rest_of_CL = false, bool allow_shrink = false) {
		size_t size_rounded_up = _round_up(exact_size);

		bool need_resize = size_rounded_up > _capacity or (allow_shrink and size_rounded_up < _capacity);

		if (need_resize) {
			resize(size_rounded_up);
			// resize zero-s all
		} else {
			// we didn't resize, but we might still need to zero the rest of the Cache Line
			if (zero_rest_of_CL and size_rounded_up > 0) {
				std::memset(_p + exact_size, 0, size_rounded_up - exact_size);
			}
		}

		assert(size_rounded_up <= _capacity);
		return _capacity;
	}

	/**
	 * \brief Reallocate the array. All content may be lost.
	 */
	virtual void resize(size_t n) {
		if (n == _capacity)
			return;
		_free();
		_p = nullptr;
		_p = _allocate(n);
		if (_p == nullptr)
			throw std::bad_alloc();
		_capacity = n;
	}

	virtual void zero(size_t start_idx) {
		if (_capacity > 0) {
			size_t num_to_zero = this->_round_up(start_idx) - start_idx;
			std::memset(_p, 0, num_to_zero * sizeof(T));
		}
	}

	inline size_t get_size() const {
		return this->_capacity;
	}

	/**
	 * \brief Implicit conversion into pointer to T.
	 */
	operator T*() const {
		return _p;
	}

	/**
	 * \brief Return amount of allocated storage + .
	 */
	size_t get_dynamic_memory() const {
		return _capacity * sizeof(T);
	}

	static size_t _round_up(size_t n) {
		size_t ret;
		switch(sizeof(T)) {
		case 1:
			ret = (n + 63) & ~0x3F;
			break;
		case 4:
			ret = (n + 15) & ~0x0F;
			break;
		case 8:
			ret = (n + 7) & ~0x07;
			break;
		default:
			assert(false);
		}
		return ret;
	}

protected:
	void _assign(T * p) const {
		std::memcpy(_p, p, _capacity * sizeof(T));
	}

	static T* _allocate(size_t elements) {

#if defined(__SSE3__) && ! defined(__PGI)
		T* ptr = static_cast<T*>(_mm_malloc(sizeof(T) * elements, alignment));
#else
		T* ptr = static_cast<T*>(memalign(alignment, sizeof(T) * elements));
#endif

		std::memset(ptr, 0, elements * sizeof(T));
		return ptr;

	}

	void _free()
	{
#if defined(__SSE3__) && ! defined(__PGI)
		_mm_free(_p);
#else
		free(_p);
#endif
	}

	size_t _capacity;
	T * _p;
};

#endif
