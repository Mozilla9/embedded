/**
 * grabbed from https://en.wikipedia.org/wiki/Unix_time
 */


#include <time_utils.h>



#define TBIAS_DAYS               ((70 * (uint32_t)365) + 17)
#define TBIAS_SECS               (TBIAS_DAYS * (uint32_t)86400)
#define	TBIAS_YEAR               1900
#define MONTAB(year)             ((((year) & 03) || ((year) == 0)) ? mos : lmos)

#define	DAYS_TO_32(year, mon)    (((year - 1) / 4) + MONTAB(year)[mon])




/**
 *
 */
void unixt_to_date(const uint32_t unixt, date_time_t * const t)
{
    const uint16_t lmos[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
    const uint16_t mos[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    uint32_t secs;
    uint32_t days;
    uint32_t mon;
    uint32_t year;
    uint32_t i;
    const uint16_t *pm;

    secs = unixt;
    days = TBIAS_DAYS;

    /* days, hour, min, sec */
    days += secs / 86400;
    secs = secs % 86400;

    t->tm_hour = secs / 3600;
    secs %= 3600;

    t->tm_min = secs / 60;
    t->tm_sec = secs % 60;


    /* determine year */
    year = days / 365;
    for ( ; days < (i = DAYS_TO_32(year, 0) + 365 * year); ) {
        year--;
    }

    days -= i;
    t->tm_year = year + TBIAS_YEAR;

    /* determine month */
    pm = MONTAB(year);
    mon = 12;
    while (days < pm[--mon]);

    t->tm_mon = mon + 1;
    t->tm_mday = days - pm[mon] + 1;
}


/*
 *
 *
 */
uint32_t date_to_unixt(date_time_t * const t)
{
    const uint16_t lmos[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
    const uint16_t mos[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    uint32_t days;
    uint32_t secs;
    uint32_t mon;
    uint32_t year;

    /* calculate number of days */
    mon = t->tm_mon - 1;
    year = t->tm_year - TBIAS_YEAR;
    days  = DAYS_TO_32(year, mon) - 1;
    days += 365 * year;
    days += t->tm_mday;
    days -= TBIAS_DAYS;

    /* calculate number of seconds */
    secs  = 3600 * t->tm_hour;
    secs += 60 * t->tm_min;
    secs += t->tm_sec;

    secs += (days * (uint32_t)86400);

    return secs;
}

