/*
    File:    list_to_columns.h
    Created: 06 November 2016 at 12:20 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef LIST_TO_COLUMNS_H
#define LIST_TO_COLUMNS_H
#include <cstddef>
#include <string>
#include <vector>

struct Format{
    size_t indent = 0;                  // количество пробелов перед каждой строкой
    size_t number_of_columns = 2;       // требуемое количество колонок
    size_t spaces_between_columns = 0;  // количество пробелов между колонками
};

std::string string_list_to_columns(const std::vector<std::string>& l,
                                   const Format& f, char d = ',');
#endif