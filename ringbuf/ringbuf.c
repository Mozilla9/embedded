/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */


#include "ringbuf.h"



static void inc_tile(__ringbuf * const r) {
    if (r->tile > r->head) {
        if (r->tile == r->sz - 1) {
            if (r->head > 0) {
                r->tile = 0;
            }
        }
        else {
            r->tile++;
        }
    }
    else if (r->tile < r->head) {
        if (r->tile != r->head - 1) {
            r->tile++;
        }
    }
    else {
        r->tile++;
    }
}


static void inc_head(__ringbuf * const r) {
    if (r->head == r->sz - 1) {
        r->head = 0;
    }
    else {
        r->head++;
    }
    
    if (r->tile == r->head) {
        r->tile = 0;
        r->head = 0;
    }
}


static void __put(void * const r, const uint8_t c) {
    __ringbuf * const _r = (__ringbuf *)r;
    
    _r->data[_r->tile] = c;
    inc_tile(_r);
}


static uint8_t __get(void * const r) {
    __ringbuf * const _r = (__ringbuf *)r;
    const uint8_t byte = _r->data[_r->head];
    
    inc_head(_r);
    
    return byte;
}

static uint16_t __size(void * const r) {
    __ringbuf * const _r = (__ringbuf *)r;
    
    if (_r->head > _r->tile) {
        return _r->sz - _r->head + _r->tile;
    }
    else if (_r->head < _r->tile) {
        return _r->tile - _r->head;
    }
    else {
        return 0;
    }
}


static void __reset(void * const r) {
    __ringbuf * const _r = (__ringbuf *)r;
    
    _r->tile = 0;
    _r->head = 0;
}


void init_ringbuf(__ringbuf * const r, uint8_t * const data, const uint16_t size) {
    r->data = data;
    r->sz = size;
    r->tile = 0;
    r->head = 0;
    
    /* initialize interface */
    r->reset = __reset;
    r->put = __put;
    r->get = __get;
    r->size = __size;
}
