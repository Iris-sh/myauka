/* 
    Файл:    search_char.h
    Создан:  13 декабря 2015г. в 09:05 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef SEARCH_CHAR_H
#define SEARCH_CHAR_H

#define THERE_IS_NO_CHAR (-1)
/**
 * \function search_char 
 * Данная функция ищет заданный символ типа char32_t в строке,
 * состоящей из символов такого типа и завершающейся нулевым
 * символом.
 * 
 * \param [in] c --- искомый символ
 * \param [in] array --- строка в которой ищется символ
 * \return смещение (в символах) от начала строки, если 
 * искомый символ в строке есть, и (-1) в противном случае
 */
int search_char(char32_t c, const char32_t* array);
#endif