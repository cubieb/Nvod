#ifndef _Common_h_
#define _Common_h_

/******************Read from / Write to packet buffer******************/
template <typename T>
typename std::enable_if<std::is_integral<T>::value, size_t>::type
ReadBuffer(uchar_t *buf, T &value)
{
    value = 0;
    size_t size = sizeof(T);
    for (size_t i = 0; i < size; ++i)
    {
        value = (value << 8) | buf[i];
    }

    return size;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, size_t>::type
WriteBuffer(uchar_t *buf, T value)
{
    size_t size = sizeof(T);
    for (size_t i = 0; i < size; ++i)
    {
        buf[size - i - 1] = value & 0xff;
        value = (value >> 4) >> 4;  // value >> 8 will cause C4333 warning in VC 2012.
    }
    return size;
}

template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value, size_t>::type
WriteBuffer(uchar_t *buf, T value)
{
    size_t size = value.size();
    memcpy(buf, value.c_str(), value.size());

    return size;
}

/**********************class WriteHelper**********************/
template <typename T>
class WriteHelper
{
public:
    /*
    Parameter:
        destination: we will write length value to this place.
        start: we will calculate the length value by (end - length)
    */
    WriteHelper(uchar_t *destination, uchar_t *start)
        : lengthField(destination), start(start)
    {}

    size_t Write(T orBits, uchar_t *end)
    {
        size_t size = end - start;
        WriteBuffer(destination, (T)(orBits | size));

        return size;
    }

private:
    uchar_t *destination;
    uchar_t *start;
};

#endif
