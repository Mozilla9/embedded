
#ifndef __V_PRINTF_H
#define __V_PRINTF_H


#include <stdint.h>


typedef void (* __PUTCHAR_FUNC) (uint8_t);

void serprintf(__PUTCHAR_FUNC __pchar, const int8_t * format, ...);
void serial_putch(__uint8_t _c);
__PUTCHAR_FUNC get_str_putch(int8_t * _str);

#define v_printf(format,...)             serprintf(serial_putch,(format),__VA_ARGS__)
#define v_sprintf(format,str,...)        serprintf(get_str_putch((str)),(format),__VA_ARGS__)
#define v_eprintf(format,ext, ...)       serprintf((ext),(format),__VA_ARGS__)


#endif /* __V_PRINTF_H */
