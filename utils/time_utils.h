/**
 * grabbed from https://en.wikipedia.org/wiki/Unix_time
 */


#ifndef __TIME_UTILS_H
#define __TIME_UTILS_H


#include <stdint.h>
#include <stdbool.h>


typedef struct {
    uint8_t tm_sec;
    uint8_t tm_min;
    uint8_t tm_hour;
    uint8_t tm_mday;
    uint8_t tm_mon;
    uint16_t tm_year;
} date_time_t;


/**
 *
 */
void unixt_to_date(const uint32_t unixt, date_time_t * const t);


/**
 *
 */
uint32_t date_to_unixt(date_time_t * const t);



#endif /* __TIME_UTILS_H */
