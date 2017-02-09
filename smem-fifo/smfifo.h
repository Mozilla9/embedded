/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */


#ifndef __SMEM_FIFO_H
#define __SMEM_FIFO_H


#include <stdint.h>
#include <stdbool.h>


#ifndef NUUL
#define NULL ((void *)0)
#endif



/**
 * @brief Public API macros.
 *
 */
#define SMEM_FIFO_FLUSH(fh)                     smfifo_flush((fh))
#define SMEM_FIFO_AVAILABLE_SPACE(fh)           smfifo_available_space((fh))
#define SMEM_FIFO_FILLED_SPACE(fh)              smfifo_filled_space((fh))
#define SMEM_FIFO_POP_DATA(fh,buf,len)          smfifo_pop_data((fh),(buf),(len))
#define SMEM_FIFO_POP_BYTE(fh)                  smfifo_pop_byte((fh))
#define SMEM_FIFO_PUSH_DATA(fh,buf,len)         smfifo_push_data((fh),(buf),(len))
#define SMEM_FIFO_PUSH_BYTE(fh,byte)            smfifo_push_byte((fh),(byte))
#define SMEM_FIFO_REPLACE(fh,pos,buf,len)       smfifo_replace_data((fh),(pos),(buf),(len))

#define SMEM_FIFO_GET_CURSOR(fh,buf,plen)       smfifo_get_cursor((fh),(buf),(plen))
#define SMEM_FIFO_SHIFT_CURSOR(fh,len)          smfifo_shift_cursor((fh),(len))




/**
 * @brief SMem fifo handle
 */
typedef struct __smem_fifo_handle {

    uint8_t * in_cursor;
    uint8_t * out_cursor;

    uint8_t * const fifo_buf;
    uint16_t data_len;
    const uint16_t fifo_size;

} __smem_fifo_handle;



/**
 *
 */
void smfifo_flush(struct __smem_fifo_handle * const handle);

/**
 *
 */
uint16_t smfifo_available_space(struct __smem_fifo_handle * const handle);

/**
 *
 */
uint16_t smfifo_filled_space(struct __smem_fifo_handle * const handle);

/**
 *
 */
bool smfifo_push_byte(struct __smem_fifo_handle * const handle, const uint8_t byte);

/**
 *
 */
bool smfifo_push_data(struct __smem_fifo_handle * const handle, const uint8_t * const src, const uint16_t len);

/**
 *
 */
uint8_t smfifo_pop_byte(struct __smem_fifo_handle * const handle);

/**
 *
 */
bool smfifo_pop_data(struct __smem_fifo_handle * const handle, uint8_t * const dst, const uint16_t len);

/**
 *
 */
bool smfifo_replace_data(struct __smem_fifo_handle * const handle, const uint16_t pos, const uint8_t * const src, const uint16_t len);

/**
 *
 */
void smfifo_get_cursor(struct __smem_fifo_handle * const handle, const uint8_t ** cursor, uint16_t * len);

/**
 *
 */
bool smfifo_shift_cursor(struct __smem_fifo_handle * const handle, const uint16_t len);

/**
 * @brief Tests
 */
void smem_fifo_run_tests(void);



#endif /* __SMEM_FIFO_H */
