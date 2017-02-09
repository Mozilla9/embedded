/**
 * Author: Serge Maslyakov, rusoil.9@gmail.com
 */


#ifndef __SHARED_UTILS_H
#define __SHARED_UTILS_H



#include <stdint.h>
#include <stdbool.h>



/**
 *
 */
void mem_copy(void *dst, const void *src, uint32_t cnt);


/**
 *
 */
void mem_set(void *dst, uint8_t val, uint32_t cnt);


/**
 *
 */
bool mem_cmp(const void *dst, const void *src, uint32_t cnt);


/**
 *
 */
const char *find_char_pos(const char *str, char chr);


/**
 *
 */
bool check_char(const char *str, char chr);


/**
 *
 */
bool str_cmp(const char *str, const char *src);


/**
 *
 */
uint32_t str_len(const char *str);


/**
 *
 */
uint32_t str_copy(uint8_t *dst, const char *src);


/**
 *
 */
bool check_ip4_addr(const char * str);


/**
 *
 */
uint32_t str_to_int(const char *str);


/**
 *
 */
void utils_run_tests(void);


#endif /* __SHARED_UTILS_H */
