/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */

#include "shared_utils.h"



/**
 *
 */
void mem_copy(void *dst, const void *src, uint32_t cnt)
{
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    while (cnt--) *d++ = *s++;
}


/**
 *
 */
void mem_set(void *dst, uint8_t val, uint32_t cnt)
{
    uint8_t *d = (uint8_t *)dst;
    while (cnt--) *d++ = (uint8_t)val;
}


/**
 *
 */
bool mem_cmp(const void *dst, const void *src, uint32_t cnt)
{
    const uint8_t *d = (const uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    uint32_t r = 0;

    while (cnt-- && (r = *d++ - *s++) == 0);

    return r == 0;
}


/**
 *
 */
const char *find_char_pos(const char *str, char chr)
{
    while (*str && *str != chr) str++;

    return str;
}


/**
 *
 */
bool check_char(const char *str, char chr)
{
    while (*str && *str != chr) str++;

    return *str != 0;
}


/**
 *
 */
bool str_cmp(const char *str, const char *src)
{
    while (*str) {
        if (*str++ != *src++) return false;
    }

    return true;
}


uint32_t str_len(const char *str)
{
    uint32_t len = 0;

    while (*str++) len++;

    return len;
}


/**
 *
 */
uint32_t str_copy(uint8_t *dst, const char *src)
{
    uint32_t len = 0;

    while (*src) {
        *dst++ = *src++;
        len++;
    }
    *dst = 0;
    return len;
}


/**
 *
 */
bool check_ip4_addr(const char * str)
{
    uint32_t count = 0;
    uint32_t idx = 0;

    while (*str && idx++ < 3) {
        str = find_char_pos(str, '.');

        if (*str) {
            count++;
        }
    }

    return count == 3;
}


/**
 *
 */
uint32_t str_to_int(const char *str)
{
    uint32_t value;

    value = 0;
    while (*str) {
        value *= 10;
        value += *str - '0';

        str++;
    }

    return value;
}

