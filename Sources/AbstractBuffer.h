#ifndef ABSTRACTBUFFER_H
#define ABSTRACTBUFFER_H
#pragma once

class AbstractBuffer {
public:
    void * buffer();
    int size();
    /*there better use private, but it need add set of 'setters' */
protected:
    void *d_buffer;
    int d_size;

};
#endif // ABSTRACTBUFFER_H
