/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 *
 * Simple debug terminal lib.
 * Allows to connect your board to the simple cmd-interface.
 *
 *
 */



#ifndef __STERMINAL_H
#define __STERMINAL_H


#include <stdint.h>


/**
 * @brief Callback function, which will be called
 *
 * @param param - pointer on param if it present
 */
typedef void (*TERMINAL_CMD_CALLBACK) (const uint8_t * const param);


/**
 * @brief Terminal command struct
 *
 * @field cmd - cmd string
 * @field callback - callback of command
 */
typedef struct {
    const uint8_t * const cmd;
    const TERMINAL_CMD_CALLBACK callback;
} __sterminal_cmd;


/**
 * @brief Terminal buffer struct
 *
 * @field buf - pointer on buffer
 * @field index - using for parsing
 * @field size - size of buff
 * @blank_counter - counter of occurrences blank
 */
typedef struct {
    uint8_t * buf;
    uint32_t index;
    const uint32_t size;
    uint32_t blank_counter;
} __sterminal_buffer;


/**
 * @brief Terminal descriptor
 *
 * @field START_TOKEN - start cmd token. Use the '0xff' for any token case
 * @field END_TOKEN - end cmd token, usually '\r' or '\n'
 * @field commands - array of commands, should be NULL terminated.
 */
typedef struct {
    const uint8_t START_TOKEN;
    const uint8_t END_TOKEN;
    __sterminal_cmd * const * commands;
} __sterminal_descriptor;


/**
 * @brief Main managed structure
 *
 * @field tbuf - pointer in the buffer details
 * @field descriptor - pointer on the descriptor
 * @field push_byte - input point of terminal
 */
typedef struct __sterminal_handle {
    __sterminal_buffer *tbuf;
    const __sterminal_descriptor * descriptor;

    /**
     * interface
     */
    void (* push_byte)(struct __sterminal_handle * const, const uint8_t);
} __sterminal_handle;


/**
 * @brief Public API macros.
 *
 * @param trh - pointer to the "__terminal_handle" structure
 * @param b - data byte
 */
#define DEBUG_TERMINAL_SEND(trh,b)          (trh)->push_byte((trh),(b))


/**
 * @brief Performs an initialization of "__sterminal_handle" structure.
 *
 * @param trh - pointer on "__sterminal_handle" structure which need to initialize.
 * @param descriptor - pointer on "__sterminal_descriptor" structure which describes terminal.
 * @param tbuf - pointer on "__sterminal_buffer" structure with buffer details.
 */
void create_sterminal(__sterminal_handle * const trh,
        const __sterminal_descriptor * const descriptor,
        __sterminal_buffer * const tbuf);


#endif /* __STERMINAL_H */

