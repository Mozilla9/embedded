/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */

#include "shared_utils.h"
#include "time_utils.h"
#include "crc8.h"

#include <v_printf.h>


static void assert(bool value, const char *error) {
    if (!value) {
        v_printf("Assert error:%s\r\n", error);

        while(1);
    }
}



static void unixt_to_date_test(void);
static void crc8_test(void);




/**
 *
 */
void utils_run_tests(void)
{
    /*******/
    unixt_to_date_test();

    /*******/
    crc8_test();

    v_printf("Utils tests have finished successfully\r\n", 1);
}


/**
 *
 */
static void unixt_to_date_test(void)
{
    uint32_t unixt;

    /*
     * 06/02/2017 13:33:33  = 1486388013
     *
     */

    /* 1 */
    date_time_t t = {
        .tm_sec = 33,
        .tm_min = 33,
        .tm_hour = 13,
        .tm_mday = 6,
        .tm_mon = 2,
        .tm_year = 2017
    };

    unixt = date_to_unixt(&t);

    assert(unixt == 1486388013, "Should be = 1486388013");

    /* 2 */

    date_time_t t1;

    unixt_to_date(1486388013, &t1);

    assert(t1.tm_hour == 13, "Should be 13");
    assert(t1.tm_min == 33, "Should be 33");
    assert(t1.tm_sec == 33, "Should be 33");
    assert(t1.tm_mday == 6, "Should be 6");
    assert(t1.tm_mon == 2, "Should be 2");
    assert(t1.tm_year == 2017, "Should be 2017");
}


/**
 *
 */
static void crc8_test(void)
{
    uint8_t buf[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    uint8_t crc;

    /* 1 */
    crc = crc8_dallas(buf, 10);

    assert(crc == 0x75, "Crc should be = 0x75");

    /* 2 */
    crc8_dallas_composite(buf, 5, true);
    crc = crc8_dallas_composite(buf + 5, 5, false);

    assert(crc == 0x75, "Composite crc should be = 0x75");
}
