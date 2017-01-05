#ifndef ABSTRACTBUFFER_H
#define ABSTRACTBUFFER_H
#pragma once
#include <iosfwd>
class AbstractBuffer {
public:
    void * buffer();
    std::size_t size();
    /*there better use private, but it need add set of 'setters' */
	virtual ~AbstractBuffer() = default;
protected:
    void *d_buffer;
    std::size_t d_size;

};
#endif // ABSTRACTBUFFER_H
