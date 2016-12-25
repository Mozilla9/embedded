/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */

#ifndef __RINGBUF_H
#define __RINGBUF_H


#include <stdint.h>


/**
 * Structure that holds the state of a ring buffer.
 *
 * This structure holds the state of a ring buffer.
 * The actual buffer needs to be defined separately.
 * This struct is an opaque structure with no user-visible elements.
 *
 * !!! Be carefully with race conditions.
 */
typedef struct {
    uint8_t * data;
    uint16_t sz;
    
    uint16_t tile;
    uint16_t head;
    
    /* interface */
    void (* reset)(void * const r);
    void (* put)(void * const r, const uint8_t c);
    uint8_t (* get)(void * const r);
    uint16_t (* size)(void * const r);
} __ringbuf;


#define size_ringbuf(r)    r.size(&r)
#define reset_ringbuf(r)   r.reset(&r)
#define put_ringbuf(r,c)   r.put(&r,c)
#define get_ringbuf(r)     r.get(&r)

void init_ringbuf(__ringbuf * const r, uint8_t * const data, const uint16_t size);


#endif /* __RINGBUF_H */
