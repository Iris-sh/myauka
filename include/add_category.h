/*
    Файл:    add_category.h
    Создан:  04 февраля 2017г. в 13:01 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef ADD_CATEGORY_H
#define ADD_CATEGORY_H
#include <set>
#include <utility>
#include <string>
#include "../include/info_for_constructing.h"
/*
 * Добавляет в ассоциативный массив std::map<size_t, std::string> category_name
 * категорию (множество) символов с именем default_name. Если такого множества ещё
 * нет, то добавляем с таким названием, и возвращаем его и true. Если же есть, то
 * возвращаем имеющееся название и false.
*/
std::pair<bool, std::string> add_category(Info_for_constructing&    info,
                                          const std::set<char32_t>& s,
                                          const std::string&        default_name);
#endif