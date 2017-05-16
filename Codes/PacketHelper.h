#ifndef _Common_h_
#define _Common_h_

#include <cassert>
#include <typeinfo>
#include <string>
#include "BaseType.h" /* uchar_t */

template<typename T>
T Exp2M1(size_t exponent)
{
    return ~(numeric_limits<T>::max() << exponent);
}

/******************Read from / Write to packet buffer******************/
template <typename T>
size_t ReadBuffer(T *buf, T &value)
{
    uchar_t *ptr = (uchar_t *)buf;
    value = 0;
    size_t size = sizeof(T);
    for (size_t i = 0; i < size; ++i)
    {
        value = (value << 8) | ptr[i];
    }

    return size;
}

template <typename T>
T ReadBuffer(T *buf, size_t shift, size_t bits)
{
	T value;
	ReadBuffer(buf, value);
    return (value >> shift) & Exp2M1<T>(bits);
}

template <typename T>
size_t WriteBuffer(T *buf, T value)
{
    uchar_t *ptr = (uchar_t *)buf;
    size_t size = sizeof(T);
    for (size_t i = 0; i < size; ++i)
    {
        ptr[size - i - 1] = value & 0xff;
        value = (value >> 4) >> 4;  // value >> 8 will cause C4333 warning in VC 2012.
    }
    return size;
}

template <typename T>
void WriteBuffer(T *buf, T value, size_t shift, size_t bits)
{
	T result;
	ReadBuffer(buf, result);
    result = (result & ~((Exp2M1<T>(bits) << shift))) | ((value & Exp2M1<T>(bits)) << shift);

	WriteBuffer(buf, result);
}

inline size_t Write(void *dest, size_t destSize, const void *src, size_t count)
{
    memcpy(dest, src, count);
    return count;
}

#endif
