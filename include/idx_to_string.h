/* 
    File:    idx_to_string.h
    Created: 13 декабря 2015г. в 09:05 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef IDX_TO_STRING_H
#define IDX_TO_STRING_H

# include "../include/char_trie.h"
#include <string>
#include <memory>
/* Следующая функция преобразует индекс idx в префиксном дереве t в
 * значение типа std::string и возвращает это значение. */
std::string idx_to_string(std::shared_ptr<Char_trie> t, size_t idx);
#endif