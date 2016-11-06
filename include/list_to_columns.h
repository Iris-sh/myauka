/*
    Файл:    list_to_columns.h
    Создан:  06 ноября 2016г. в 12:20 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
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