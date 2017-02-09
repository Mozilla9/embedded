### Организация embedded проектов на C/C++ (структура/code-stye и т.п.)

Ссылки на другие стандарты

[http://www.maultech.com/chrislott/resources/cstyle/indhill-cstyle.pdf](http://www.maultech.com/chrislott/resources/cstyle/indhill-cstyle.pdf)

[https://www.gnu.org/prep/standards/standards.html#Formatting](https://www.gnu.org/prep/standards/standards.html#Formatting)

[https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html#pnames](https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html#pnames)
##### 1 Именование файлов/папок в проекте
- имена файлов/папок в проекте дожны быть на английском, в нижнем регистре. Разделитель между словами символ подчеркивания `_`
```
src\
src\modem

modem_routines.c    /* c source file */
modem_routines.h    /* c header file */
```

##### 2 Заголовки

- в начало каждого файла помещается заголовок с названием/авторством/датой-создания/копирайтом. Современные *IDE* позволяют настроить шаблоны для автоматического добавления заголовков.

- Для `Eclipse`, `'Window'->'Preferences'->'C/C++'->'Code Style'->'Code Templates'`

```
/*
 * Copyright (c) ${year} {company_name}. All Rights Reserved.
 * {company_name} proprietary and confidential.
 * Use is subject to license terms.
 *
 * @Project: ${project_name}
 * @File:    ${file_name}
 *
 * @Author:  ${user}
 * @Created: ${date}
 *
 */
```
##### 3 Типы данных

- Используем типы из страндартной библиотеки С. Подключаем через:

```
#include <stdint.h>
#include <stdbool.h>

/**
 * uint8_t
 * uint16_t
 * uint32_t
 * ...
 * bool
 */

```

##### 4 Именование переменных и функций

1) Имена переменных и функций в нижнем регистре, слова разделяются через символ подчеркивания  **_**

```
uint32_t mmu_value;

void * pointer_on_void;

void write_data_to_modem(uint8_t *buf, const uint16_t data_len);
```

2) Имена новых типов на основе структур в нижнем регистре и начинаются с двух символов подчеркивания

```
typedef struct __my_new_type {
    uint32_t fff;
    uint32_t ggg;

} __my_new_type;

```

3) Имена констант, дефайнов, типов данных на основе функций

```
#define GGGGGG   1
static const uint32_t GGGGGG = 100;
typedef void (*TERMINAL_CALLBACK)(void * arg);

```

4) Локальные переменные

- локальные переменные всегда должны быть определены в начале тела функции.

5) Глобальные переменные и функции

- все переменные и функции, которые видимы вне файла, должны начинаться с имени модуля, относящегося к ним. Так проще знать, где искать определения для функций и переменных.

```

extern char * dcp_router;
void dcp_global_func(char * buf);

```

6) Статические функции

- имя не должно предварительно начинаться с имени модуля

```

static void some_static_func(char * buf);

```

##### 5 Форматирование кода

1) Отступы, пробелы табуляции

- для отступов используем только пробелы (табуляции не используем)

- стандартный отступ равен 4 пробела

2) Определение функций

- открывающая скобка с новой строки

```
static uint32_t my_func(uint8_t *param1, uint32_t param2)
{
    ...
}
```

3) Определение указателей

```

uint32_t * pointer;   /* допустимо */
uint32_t *pointer;    /* допустимо */

uint32_t* pointer;    /* не допустимо */


```

4) Управляющие конструкции (**for**/**while**/**if**)

- после ключевого слова пробел
- открывающая фигурная скобка на той же строке

```
/* цикл for */
for (...) {

}

/* условие if */
if (...) {

} else if () {

} else {

}

/* цикл do-while */
do {

} while (...);


/* цикл while */
while (...) {

}

```

5) Комментарии

```

/* Одиночная строка комментария выглядит примерно как эта строка. */

/**
 * Многострочный коменнтарий должен быть оформлен как показано тут. Комментарии должны
 * предпочительно быть полными предложениями, заполненными так, чтобы быть похожими на
 * реальные параграфы.
 */

```
