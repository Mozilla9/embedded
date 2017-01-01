/* printf.c: Output format for a controller environment.
 *
 * This implements a limited subset of printf(), with only the
 * format features typically needed in an embedded environment.
 *
 * It's compatible with the AVR, with the format string read from
 * program space / flash, and all parameters from data space / RAM.
 * See 'S' for an optional exception for constant strings.
 *
 * Before "fixing"/extending this implementation, please consider
 * the target use on very small machine.
 *
 * This implementation is only printf().  There is no option for
 * sprintf() or fprintf().  It queues the characters directly to the
 * output device with serial_putch(), with the only additional space
 * used a small buffer for building %d/%x output.
 *
 * Originally written 2010-2011 by Donald Becker and William Carlson
 * for the QAR EV motor controller project.  Released under GPLv2.1
 * Contact the authors for use under other terms.
 *
 * $Id: v_printf.c 153 2013-09-11 23:52:23Z mozilla $
 */

#include <stdarg.h>
#include "v_printf.h"

extern uint32_t uart_putchar(const uint8_t c);

static int8_t buf[12];
static int8_t * str_in;

static void str_putch(uint8_t _c) {
    *str_in++ = _c;
    *str_in = 0;
}

/*
 * Sleazy versions of number to string conversions.  These convert VAL
 * to a string in the local buffer with the specified number of
 * digits. Leading zeros are included, overflow is truncated and the
 * string is not terminated.
 *
 */
static void u32_to_uart(__PUTCHAR_FUNC   __pchar, uint32_t val, int32_t digits) {
    int8_t * str = buf + sizeof(buf) - 1;
    *str-- = 0;

    do {
        *str-- = (uint8_t) (val % 10) + '0';
        val = val / 10;
    } while (--digits > 0 || val > 0);

    ++str;
    do {
        __pchar(*str);
    } while (*++str);
}

/*
 * Same thing in hex.
 *
 */
static void uint_to_hex_uart(__PUTCHAR_FUNC   __pchar, unsigned val,
        uint8_t digits) {
    uint8_t nibble;
    int8_t * str = buf + sizeof(buf) - 1;
    *str-- = 0;

    while (digits-- > 0) {
        nibble = val & 0x000f;

        if (nibble >= 10)
            nibble = (nibble - 10) + 'A';
        else
            nibble = nibble + '0';

        *str-- = nibble;
        val = val >> 4;
    }
    ++str;

    do {
        __pchar(*str);
    } while (*++str);
}

/*
 * Float to uart
 *
 */
static void float_to_uart(__PUTCHAR_FUNC   __pchar, const float val,
        uint8_t digits) {
    const float ff = val >= 0.0 ? val : (-1.0 * val);
    const uint32_t mnt = (uint32_t) ff;
    const uint32_t exp = (uint32_t) ((ff - mnt) * 10000.0);

    /* sign */
    if (val < 0.0) {
        __pchar('-');
    }

    /* mantissa */
    u32_to_uart(__pchar, mnt, digits);
    __pchar('.');

    /* leading zeros */
    if (exp >= 1000 || !exp) {

    } else if (exp >= 100) {
        __pchar('0');
    } else if (exp >= 10) {
        __pchar('0');
        __pchar('0');
    } else {
        __pchar('0');
        __pchar('0');
        __pchar('0');
    }

    /* exp */
    u32_to_uart(__pchar, exp, digits);
}

/*
 * Printf() implementation for serial uart
 *
 */
void serial_putch(uint8_t _c) {
    while (uart_putchar(_c) != 0)
        ;
    /* Returns -1 if full queue. */
    /* We busy-wait. */
}

/*
 *
 *
 *
 */
__PUTCHAR_FUNC get_str_putch(int8_t * _str) {
    str_in = _str;
    *str_in = 0;
    return &str_putch;
}

/*
 * Printf() implementation for serial uart
 *
 */
void serprintf(__PUTCHAR_FUNC   __pchar, const int8_t * format, ...) {
    va_list args;
    uint8_t c, j = 0;
    va_start(args, format);

    while ((c = *format++)) {
        if (j) {
            switch (c) {
            case 's': {

                uint8_t * str = va_arg(args, uint8_t *);
                do {
                    j = *str++;
                    if (j == 0)
                        break;      /* Clears precision */
                    __pchar(j);
                } while (1);

                break;
            }

            case 'l':
                j = sizeof(long);
                continue;

            case 'u':
                u32_to_uart(__pchar,
                        j == sizeof(long) ?
                                va_arg(args, long) : va_arg(args, int), j);
                break;

            case 'd': {
                int32_t val = (
                        j == sizeof(long) ?
                                va_arg(args, long) : va_arg(args, int));
                if (val < 0) {
                    __pchar('-');
                    val = -val;
                }
                u32_to_uart(__pchar, val, j);
                break;
            }

            case 'f':
                float_to_uart(__pchar, va_arg(args, double), j);
                break;

            case 'c':
                __pchar(va_arg(args, int));
                break;

            case 'x':
            case 'X': {
                if (j == 1) {               /* Do "%#x" rather than "%x" */
                    __pchar('0');
                    __pchar(c);             /* Match case 0x or 0X */
                }
                uint_to_hex_uart(__pchar,
                        j == sizeof(long) ?
                                va_arg(args, long) : va_arg(args, int), j);
                break;
            }

            default:
                __pchar(c);
                break;
            }

            j = 0;
        } else {
            if (c == '%') {
                const int8_t d = *format;

                if ('0' <= d && d <= '9') {
                    format++;
                    j = d - '0';
                } else {
                    j = 1;
                }
            } else {
                /* Automatic CR+LF expansion saves space */
                if (c == '\n') {
                    __pchar('\r');
                }
                __pchar(c);
            }
        }
    }
    va_end(args);
}

