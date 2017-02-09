/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */



#include "fifotxt.h"

#include <v_printf.h>
#include <shared_utils.h>


static void assert(bool value, const char *error) {
    if (!value) {
        v_printf("Assert error:%s\r\n", error);

        while(1);
    }
}



#define FIFOTXT_TEST_MSG_SIZE     5
#define FIFOTXT_TEST_QUEUE_SIZE   5
#define FIFOTXT_TERMINATOR        '\r'
#define FIFOTXT_IGNORED           '\n'

#define PRINT_TEST_NAME(s)        v_printf(#s, 1)



static void fifotxt_push_byte_test(__fifotxt_handle *handle);
static void fifotxt_push_msg_test(__fifotxt_handle *handle);
static void fifotxt_flush_test(__fifotxt_handle *handle);
static void fifotxt_cmp_last_test(__fifotxt_handle *handle);
static void fifotxt_cmp_first_test(__fifotxt_handle *handle);
static void fifotxt_pop_msg_test(__fifotxt_handle *handle);
static void fifotxt_discard_msg_test(__fifotxt_handle *handle);


/**
 *
 */
void fifotxt_run_tests(void)
{
    uint8_t heap[FIFOTXT_TEST_MSG_SIZE * (FIFOTXT_TEST_QUEUE_SIZE + 1)];

    __fifotxt_queue test_fqueue = {
        .queue = heap,
        .size = FIFOTXT_TEST_QUEUE_SIZE
    };

    __fifotxt_buffer test_fbuf = {
        .buf = heap + (FIFOTXT_TEST_QUEUE_SIZE * FIFOTXT_TEST_MSG_SIZE),
        .size = FIFOTXT_TEST_MSG_SIZE
    };

    __fifotxt_handle test_fifotxt = {
        .END_TOKEN = FIFOTXT_TERMINATOR,
        .IGNORED_TOKEN = FIFOTXT_IGNORED,
        .fqueue = &test_fqueue,
        .fbuf = &test_fbuf
    };

    create_fifotxt(&test_fifotxt);

    /*******/
    fifotxt_push_byte_test(&test_fifotxt);
    FIFOTXT_FLUSH_FIFO(&test_fifotxt);

    /*******/
    fifotxt_push_msg_test(&test_fifotxt);
    FIFOTXT_FLUSH_FIFO(&test_fifotxt);

    /*******/
    fifotxt_flush_test(&test_fifotxt);
    FIFOTXT_FLUSH_FIFO(&test_fifotxt);

    /*******/
    fifotxt_cmp_last_test(&test_fifotxt);
    FIFOTXT_FLUSH_FIFO(&test_fifotxt);

    /*******/
    fifotxt_cmp_first_test(&test_fifotxt);
    FIFOTXT_FLUSH_FIFO(&test_fifotxt);

    /*******/
    fifotxt_pop_msg_test(&test_fifotxt);
    FIFOTXT_FLUSH_FIFO(&test_fifotxt);

    /*******/
    fifotxt_discard_msg_test(&test_fifotxt);
    FIFOTXT_FLUSH_FIFO(&test_fifotxt);

    v_printf("Fifotxt tests have finished successfully\r\n", 1);
}


/**
 *
 */
static void fifotxt_push_byte_test(__fifotxt_handle *handle)
{
    PRINT_TEST_NAME(fifotxt_push_byte_test\r\n);

    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty");

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');

    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty");

    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    assert(FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should not be empty");
}


/**
 *
 */
static void fifotxt_push_msg_test(__fifotxt_handle *handle)
{
    PRINT_TEST_NAME(fifotxt_push_msg_test\r\n);

    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty");

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');

    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty");

    FIFOTXT_PUSH_MSG(handle);

    assert(FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should not be empty");
}


/**
 *
 */
static void fifotxt_flush_test(__fifotxt_handle *handle)
{
    PRINT_TEST_NAME(fifotxt_flush_test\r\n);

    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty");

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    FIFOTXT_PUSH_BYTE(handle, 'B');
    FIFOTXT_PUSH_BYTE(handle, 'C');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    assert(FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should not be empty");

    FIFOTXT_FLUSH_FIFO(handle);

    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty");
}


/**
 *
 */
static void fifotxt_cmp_last_test(__fifotxt_handle *handle)
{
    PRINT_TEST_NAME(fifotxt_cmp_test\r\n);

    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty");

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, '1');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, '3');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    assert(FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should not be empty");

    assert(FIFOTXT_CMP_LAST_MSG(handle, "AT3"), "Result AT3 should be true");
    assert(!FIFOTXT_CMP_LAST_MSG(handle, "AT1"), "Result AT1 should be false");

    FIFOTXT_DISCARD_MSG(handle);

    assert(FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should not be empty");

    assert(FIFOTXT_CMP_LAST_MSG(handle, "AT3"), "Result AT3 should be true");
    assert(!FIFOTXT_CMP_LAST_MSG(handle, "AT1"), "Result AT1 should be false");
}


/**
 *
 */
static void fifotxt_cmp_first_test(__fifotxt_handle *handle)
{
    PRINT_TEST_NAME(fifotxt_cmp_test\r\n);

    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty");

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, '1');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, '3');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    assert(FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should not be empty");

    assert(!FIFOTXT_CMP_FIRST_MSG(handle, "AT3"), "Result AT3 should be false");
    assert(FIFOTXT_CMP_FIRST_MSG(handle, "AT1"), "Result AT1 should be true");

    FIFOTXT_DISCARD_MSG(handle);

    assert(FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should not be empty");

    assert(FIFOTXT_CMP_FIRST_MSG(handle, "AT3"), "Result AT3 should be true");
    assert(!FIFOTXT_CMP_FIRST_MSG(handle, "AT1"), "Result AT1 should be false");
}


/**
 *
 */
static void fifotxt_pop_msg_test(__fifotxt_handle *handle)
{
    uint8_t buf[FIFOTXT_TEST_MSG_SIZE];
    uint16_t len;

    PRINT_TEST_NAME(fifotxt_pop_msg_test\r\n);

    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty");

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, '1');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, '3');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, '5');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    len = FIFOTXT_POP_MSG(handle, buf);
    assert(str_cmp((const char *)buf, "AT1") && len == 3, "Pop msg is worked wrong");

    len = FIFOTXT_POP_MSG(handle, buf);
    assert(str_cmp((const char *)buf, "AT3") && len == 3, "Pop msg is worked wrong");

    len = FIFOTXT_POP_MSG(handle, buf);
    assert(str_cmp((const char *)buf, "AT5") && len == 3, "Pop msg is worked wrong");
}


/**
 *
 */
static void fifotxt_discard_msg_test(__fifotxt_handle *handle)
{
    PRINT_TEST_NAME(fifotxt_discard_msg_test\r\n);

    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty");

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, '1');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, '3');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    FIFOTXT_PUSH_BYTE(handle, 'A');
    FIFOTXT_PUSH_BYTE(handle, 'T');
    FIFOTXT_PUSH_BYTE(handle, '5');
    FIFOTXT_PUSH_BYTE(handle, FIFOTXT_TERMINATOR);

    assert(FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should not be empty");

    FIFOTXT_DISCARD_MSG(handle);
    assert(FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should not be empty 1");

    FIFOTXT_DISCARD_MSG(handle);
    assert(FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should not be empty 2");

    FIFOTXT_DISCARD_MSG(handle);
    assert(!FIFOTXT_IS_FIFO_NOT_EMPTY(handle), "Fifo should be empty 3");
}




