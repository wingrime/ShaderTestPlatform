#include "AbstractBuffer.h"


void *AbstractBuffer::buffer()
{
    return d_buffer;
}

int AbstractBuffer::size()
{
    return d_size;
}
