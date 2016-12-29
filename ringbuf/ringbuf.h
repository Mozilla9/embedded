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


typedef struct __ringbuf {
    uint8_t * data;
    uint16_t sz;

    uint16_t tile;
    uint16_t head;

    /* interface */
    void (* reset)(struct __ringbuf * const r);
    void (* put)(struct __ringbuf * const r, const uint8_t c);
    uint8_t (* get)(struct __ringbuf * const r);
    uint16_t (* size)(struct __ringbuf * const r);
} __ringbuf;


/**
 * @brief Public API macros
 *
 * @param r - pinter on the __ringbuf
 */
#define size_ringbuf(r)    r.size(&r)
#define reset_ringbuf(r)   r.reset(&r)
#define put_ringbuf(r,c)   r.put(&r,c)
#define get_ringbuf(r)     r.get(&r)


/**
 * @brief Initialize new ring buffer
 *
 * @param r - pinter on the __ringbuf struct
 * @param data - pointer on data buffer
 * @param size - size of buffer
 */
void init_ringbuf(__ringbuf * const r, uint8_t * const data, const uint16_t size);


#endif /* __RINGBUF_H */
