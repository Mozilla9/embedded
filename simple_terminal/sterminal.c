/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */


#include "sterminal.h"


#ifndef NUUL
#define NULL ((void *)0)
#endif



static int32_t strcmp(const int8_t * s1, const int8_t * s2);
static void parse_command(struct __sterminal_handle * const trh);
static void push_byte(struct __sterminal_handle * const trh, const uint8_t byte);



void create_sterminal(__sterminal_handle * const trh,
        const __sterminal_descriptor * descriptor,
        __sterminal_buffer * tbuf)
{
    trh->descriptor = descriptor;
    trh->tbuf = tbuf;

    /* interface */
    trh->push_byte = push_byte;
}


/**
 * @brief Simple implementation of 'strcmp'.
 *        (we not linking the standard 'string.h' to make less footprint size)
 *
 * @param s1 - This is the first string to be compared.
 * @prama s2 - This is the second string to be compared.
 * @return val = 0 -> equals; val < 0 ->  s1 is less s2; val > 0 -> s2 less s1
 */
static int32_t strcmp(const int8_t * s1, const int8_t * s2)
{
    const uint8_t *p1 = ( const uint8_t * )s1;
    const uint8_t *p2 = ( const uint8_t * )s2;

    while ( *p1 && *p1 == *p2 ) ++p1, ++p2;

    return ( *p1 > *p2 ) - ( *p2  > *p1 );
}


/**
 * @brief Performs a parsing of received commands
 *
 * @param trh - pointer of '__sterminal_handle'
 */
static void parse_command(struct __sterminal_handle * const trh)
{
    uint8_t * param = 0;
    __sterminal_cmd * tcmd;
    uint32_t idx;

    /* Search the first blank char and replace them to the '\0' */
    if (trh->tbuf->blank_counter) {
        idx = 0;

        for (; idx < trh->tbuf->index; idx++) {
            if (trh->tbuf->buf[idx] == ' ') {
                trh->tbuf->buf[idx] = 0;
                param = (trh->tbuf->buf + idx + 1);
                break;
            }
        }
    }

    idx = 0;

    do {
        tcmd = trh->descriptor->commands[idx++];

        if (tcmd != NULL) {
            if (strcmp(trh->tbuf->buf, tcmd->cmd) == 0) {
                tcmd->callback(param);
                tcmd = NULL;
            }
        }
    } while (tcmd != NULL);
}


/**
 * @brief Inner pre-parsing function.
 *        Stores data in buffer and determines end of command.
 *
 * @param trh - pointer of '__sterminal_handle'
 * @param byte - input data byte
 */
static void push_byte(struct __sterminal_handle * const trh, const uint8_t byte)
{
    if (trh->tbuf->index < trh->tbuf->size) {

        if (trh->tbuf->index > 0) {
            if (byte == trh->descriptor->END_TOKEN) {
                /* check last char, if it equals the blank remove it */
                if (trh->tbuf->buf[trh->tbuf->index - 1] == ' ') {
                    trh->tbuf->index--;
                    trh->tbuf->buf[trh->tbuf->index] = 0;
                } else {
                    trh->tbuf->buf[trh->tbuf->index] = 0;
                }

                parse_command(trh);

                /* cleanup */
                trh->tbuf->blank_counter = 0;
                trh->tbuf->index = 0;
            } else {
                if (byte == ' ') {
                    /* check previously char on blank, if true - ignore it */
                    if (trh->tbuf->buf[trh->tbuf->index - 1] == ' ') {
                        return;
                    }
                    trh->tbuf->blank_counter++;
                }

                trh->tbuf->buf[trh->tbuf->index++] = byte;
            }
        } else if (byte == trh->descriptor->END_TOKEN || byte == ' ') {
            /* do nothing */
        } else {
            if (byte == trh->descriptor->START_TOKEN || trh->descriptor->START_TOKEN == 0xff) {
                trh->tbuf->buf[trh->tbuf->index++] = byte;
                trh->tbuf->buf[trh->tbuf->index] = 0;
            }
        }
    }
}
