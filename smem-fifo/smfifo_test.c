/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */



#include "smfifo.h"

#include <v_printf.h>
#include <shared_utils.h>


#include "proto/b3_proto.h"

static void assert(bool value, const char *error) {
    if (!value) {
        v_printf("Assert error:%s\r\n", error);

        while(1);
    }
}


#define PRINT_TEST_NAME(s)        v_printf(#s, 1)
#define FIFO_HEAP_SIZE            5



static void smem_fifo_push_byte_test(__smem_fifo_handle *handle);
static void smem_fifo_push_data_test(__smem_fifo_handle *handle);
static void smem_fifo_pop_byte_test(__smem_fifo_handle *handle);
static void smem_fifo_pop_data_test(__smem_fifo_handle *handle);
static void smem_fifo_replace_test(__smem_fifo_handle *handle);
static void smem_fifo_cursor_test(__smem_fifo_handle *handle);
static void smem_fifo_boundary_test(__smem_fifo_handle *handle);
static void smem_fifo_final_test(__smem_fifo_handle *handle);



/**
 *
 */
void smem_fifo_run_tests(void)
{
    uint8_t heap[FIFO_HEAP_SIZE];

    __smem_fifo_handle smfifo = {.fifo_buf = heap, .fifo_size = FIFO_HEAP_SIZE};

    SMEM_FIFO_FLUSH(&smfifo);

    /*******/
    smem_fifo_push_byte_test(&smfifo);
    SMEM_FIFO_FLUSH(&smfifo);

    /*******/
    smem_fifo_push_data_test(&smfifo);
    SMEM_FIFO_FLUSH(&smfifo);

    /*******/
    smem_fifo_pop_byte_test(&smfifo);
    SMEM_FIFO_FLUSH(&smfifo);

    /*******/
    smem_fifo_pop_data_test(&smfifo);
    SMEM_FIFO_FLUSH(&smfifo);

    /*******/
    smem_fifo_replace_test(&smfifo);
    SMEM_FIFO_FLUSH(&smfifo);

    /*******/
    smem_fifo_cursor_test(&smfifo);
    SMEM_FIFO_FLUSH(&smfifo);

    /*******/
    smem_fifo_boundary_test(&smfifo);
    SMEM_FIFO_FLUSH(&smfifo);

    /*******/
    smem_fifo_final_test(&smfifo);
    SMEM_FIFO_FLUSH(&smfifo);

    v_printf("SMEM Fifo tests have finished successfully\r\n", 1);
}


/**
 *
 */
static void smem_fifo_push_byte_test(__smem_fifo_handle *handle)
{
    uint16_t result;

    PRINT_TEST_NAME(smem_fifo_push_byte_test\r\n);

    assert(!SMEM_FIFO_FILLED_SPACE(handle), "Fifo should be empty");
    assert(SMEM_FIFO_AVAILABLE_SPACE(handle) == FIFO_HEAP_SIZE, "Fifo should not be empty");

    result = 0;

    if (SMEM_FIFO_PUSH_BYTE(handle, '0'))
        result++;

    if (SMEM_FIFO_PUSH_BYTE(handle, '1'))
        result++;

    if (SMEM_FIFO_PUSH_BYTE(handle, '2'))
        result++;

    if (SMEM_FIFO_PUSH_BYTE(handle, '3'))
        result++;

    if (SMEM_FIFO_PUSH_BYTE(handle, '5'))
        result++;

    assert(result == 5, "Should be 5");

    /* fifo is full and should to discard a byte */
    if (SMEM_FIFO_PUSH_BYTE(handle, '6'))
        result++;

    assert(result == 5, "Should be 5");
}


/**
 *
 */
static void smem_fifo_push_data_test(__smem_fifo_handle *handle)
{
    PRINT_TEST_NAME(smem_fifo_push_data_test\r\n);

    assert(!SMEM_FIFO_FILLED_SPACE(handle), "Fifo should be empty");

    SMEM_FIFO_PUSH_DATA(handle, (const uint8_t * const)"test1", 5);

    assert(SMEM_FIFO_FILLED_SPACE(handle) == 5, "Fifo should not be empty");

    bool result = SMEM_FIFO_PUSH_DATA(handle, (const uint8_t * const)"123", 3);

    assert(result == false, "Should be false");
}


/**
 *
 */
static void smem_fifo_pop_byte_test(__smem_fifo_handle *handle)
{
    uint8_t buf[5];

    PRINT_TEST_NAME(smem_fifo_pop_byte_test\r\n);

    assert(!SMEM_FIFO_FILLED_SPACE(handle), "Fifo should be empty");

    SMEM_FIFO_PUSH_DATA(handle, (const uint8_t * const)"test", 4);

    buf[0] = SMEM_FIFO_POP_BYTE(handle);
    buf[1] = SMEM_FIFO_POP_BYTE(handle);
    buf[2] = SMEM_FIFO_POP_BYTE(handle);
    buf[3] = SMEM_FIFO_POP_BYTE(handle);
    buf[4] = 0;

    assert(str_cmp((const char *)buf, "test"), "Pop byte is worked wrong");
}


/**
 *
 */
static void smem_fifo_pop_data_test(__smem_fifo_handle *handle)
{
    uint8_t buf[FIFO_HEAP_SIZE];

    PRINT_TEST_NAME(smem_fifo_pop_data_test\r\n);

    assert(!SMEM_FIFO_FILLED_SPACE(handle), "Fifo should be empty");

    SMEM_FIFO_PUSH_DATA(handle, (const uint8_t * const)"test", 4);

    SMEM_FIFO_POP_DATA(handle, buf, 4);
    buf[4] = 0;

    assert(str_cmp((const char *)buf, "test"), "Pop data is worked wrong");
}


/**
 *
 */
static void smem_fifo_replace_test(__smem_fifo_handle *handle)
{
    uint8_t buf[FIFO_HEAP_SIZE];

    PRINT_TEST_NAME(smem_fifo_replace_test\r\n);

    assert(!SMEM_FIFO_FILLED_SPACE(handle), "Fifo should be empty");

    SMEM_FIFO_PUSH_DATA(handle, (const uint8_t * const)"test", 4);

    SMEM_FIFO_REPLACE(handle, 1, (const uint8_t * const)"12", 2);

    SMEM_FIFO_POP_DATA(handle, buf, 4);
    buf[4] = 0;

    assert(str_cmp((const char *)buf, "t12t"), "Replace data is worked wrong");
}



/**
 *
 */
static void smem_fifo_cursor_test(__smem_fifo_handle *handle)
{
    const uint8_t *buf;
    uint16_t len;

    PRINT_TEST_NAME(smem_fifo_cursor_test\r\n);

    assert(!SMEM_FIFO_FILLED_SPACE(handle), "Fifo should be empty");

    SMEM_FIFO_PUSH_DATA(handle, (const uint8_t * const)"test", 4);

    buf = NULL;
    SMEM_FIFO_GET_CURSOR(handle, &buf, &len);

    assert(len == 4, "Should be 4");
    assert(buf[0] == 't', "Should be t");
    assert(buf[1] == 'e', "Should be e");
    assert(buf[2] == 's', "Should be s");
    assert(buf[3] == 't', "Should be t");

    SMEM_FIFO_SHIFT_CURSOR(handle, len);

    assert(!SMEM_FIFO_FILLED_SPACE(handle), "Fifo should be empty after shifting");
}


/**
 *
 */
static void smem_fifo_boundary_test(__smem_fifo_handle *handle)
{
    uint8_t buf[FIFO_HEAP_SIZE + 1];

    PRINT_TEST_NAME(smem_fifo_boundary_test\r\n);

    assert(!SMEM_FIFO_FILLED_SPACE(handle), "Fifo should be empty");

    SMEM_FIFO_PUSH_DATA(handle, (const uint8_t * const)"test1", 5);

    assert(SMEM_FIFO_FILLED_SPACE(handle) == 5, "Should be 5");

    SMEM_FIFO_POP_DATA(handle, buf, 2);

    assert(SMEM_FIFO_FILLED_SPACE(handle) == 3, "Should be 3");

    SMEM_FIFO_PUSH_DATA(handle, (const uint8_t * const)"89", 2);

    assert(SMEM_FIFO_FILLED_SPACE(handle) == 5, "Should be 5");

    SMEM_FIFO_POP_DATA(handle, buf, 5);
    buf[5] = 0;

    assert(str_cmp((const char *)buf, "st189"), "Pop data is worked wrong");
}


static void smem_fifo_final_test(__smem_fifo_handle *handle)
{
    uint8_t control[40];
    uint16_t clen = 0;

    bool is_ok;
    const uint8_t * buf;
    uint16_t len;

    uint8_t tx_fifo_buf[29];
    __smem_fifo_handle fifo = {
            .fifo_buf = tx_fifo_buf,
            .fifo_size = 29
    };

    PRINT_TEST_NAME(smem_fifo_final_test\r\n);
    SMEM_FIFO_FLUSH(&fifo);
    /* 1 - header */
    __big3_proto_header header;
    big3p_assemble_header(&header, BIG3_PROTO_WATER_COUNTER);

    __big3_cwater_payload payload;
    big3p_assemble_cwater_payload(&payload);

    is_ok = SMEM_FIFO_PUSH_DATA(&fifo, (uint8_t *)&header, sizeof(__big3_proto_header));

    assert(is_ok, "OK");

    is_ok = SMEM_FIFO_PUSH_DATA(&fifo, (uint8_t *)&payload, sizeof(__big3_cwater_payload));

    assert(!is_ok, "NOT OK");

    SMEM_FIFO_GET_CURSOR(&fifo, &buf, &len);

    assert(len == 4, "OK1");

    mem_copy(control + clen, buf, len);
    clen += len;

    SMEM_FIFO_SHIFT_CURSOR(&fifo, len);

    /* 2 - payload */

    is_ok = SMEM_FIFO_PUSH_DATA(&fifo, (uint8_t *)&payload, sizeof(__big3_cwater_payload));

    assert(is_ok, "OK2");

    SMEM_FIFO_GET_CURSOR(&fifo, &buf, &len);

    assert(len == 25, "OK3");

    mem_copy(control + clen, buf, len);
    clen += len;

    SMEM_FIFO_SHIFT_CURSOR(&fifo, len);

    SMEM_FIFO_GET_CURSOR(&fifo, &buf, &len);

    assert(len == 3, "OK4");

    mem_copy(control + clen, buf, len);
    clen += len;

    SMEM_FIFO_SHIFT_CURSOR(&fifo, len);

    assert(clen == 32, "OK5");

    v_printf("\r\n", 1);
    for (uint16_t i = 0; i < clen; i++) {
        v_printf(" 0x%2x", control[i]);
    }
    v_printf("\r\n", 1);
}


