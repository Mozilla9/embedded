/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */


#ifndef __FIFOTXT_H
#define __FIFOTXT_H


#include <stdint.h>
#include <stdbool.h>



/**
 * @brief Public API macros.
 *
 * @param fh - pointer to the "__fifotxt_handle" structure
 * @param buf - pointer to the buffer were was stored the popped message
 * @param byte - a byte for push in the fifo
 */
#define FIFOTXT_IS_FIFO_NOT_EMPTY(fh)    is_fifotxt_not_empty((fh))
#define FIFOTXT_POP_MSG(fh,buf)          fifotxt_pop_msg((fh),(buf))
#define FIFOTXT_DISCARD_MSG(fh)          fifotxt_discard_msg((fh))
#define FIFOTXT_CMP_LAST_MSG(fh,msg)     fifotxt_cmp_last_msg((fh),(msg))
#define FIFOTXT_CMP_FIRST_MSG(fh,msg)    fifotxt_cmp_first_msg((fh),(msg))
#define FIFOTXT_PUSH_BYTE(fh,byte)       fifotxt_push_byte((fh),(byte))
#define FIFOTXT_FLUSH_FIFO(fh)           fifotxt_flush_fifo((fh))
#define FIFOTXT_PUSH_MSG(fh)             fifotxt_push_msg((fh))



/**
 * @brief Fifo queue
 *
 */
typedef struct {
    uint8_t * const queue;
    uint16_t index;
    uint16_t first;
    const uint16_t size;
} __fifotxt_queue;


/**
 * @brief Fifo buffer
 *
 */
typedef struct {
    uint8_t * const buf;
    uint16_t index;
    const uint16_t size;
} __fifotxt_buffer;



/**
 * @brief Main managed structure
 *
 */
typedef struct __fifotxt_handle {
    const uint8_t END_TOKEN;
    const uint8_t IGNORED_TOKEN;

    __fifotxt_queue * const fqueue;
    __fifotxt_buffer * const fbuf;

} __fifotxt_handle;


/**
 * @brief Performs initialization of "__fifotxt_handle" structure.
 *
 * @param fh - pointer on "__fifotxt_handle" structure which need to initialize.
 *
 */
void create_fifotxt(__fifotxt_handle * const fh);


/**
 *
 */
uint16_t fifotxt_pop_msg(__fifotxt_handle * const fh, uint8_t * in);


/**
 *
 */
void fifotxt_discard_msg(__fifotxt_handle * const fh);


/**
 *
 */
bool is_fifotxt_not_empty(__fifotxt_handle * const fh);


/**
 *
 */
bool fifotxt_cmp_last_msg(__fifotxt_handle * const fh, const char * msg);


/**
 *
 */
bool fifotxt_cmp_first_msg(__fifotxt_handle * const fh, const char * msg);


/**
 *
 */
void fifotxt_flush_fifo(__fifotxt_handle * const fh);


/**
 *
 */
bool fifotxt_push_byte(__fifotxt_handle * const fh, const uint8_t byte);


/**
 *
 */
void fifotxt_push_msg(__fifotxt_handle * const fh);


/**
 *
 */
uint16_t fifotxt_pop_msg(__fifotxt_handle * const fh, uint8_t * in);


/**
 * @brief Tests
 */
void fifotxt_run_tests(void);



#endif /* __FIFOTXT_H */
