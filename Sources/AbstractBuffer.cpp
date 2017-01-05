#include "AbstractBuffer.h"


void *AbstractBuffer::buffer()
{
    return d_buffer;
}

std::size_t AbstractBuffer::size()
{
    return d_size;
}
