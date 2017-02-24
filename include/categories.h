/*
    Файл:    categories.h
    Создан:  10 января 2015г. в 10:32 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef CATEGORIES_H
#define CATEGORIES_H

#include "../include/generalized_char.h"
#include <set>

using Set_of_char = std::set<char32_t>;

enum Category_kind{
    All_chars, Not_single_quote, Not_double_quote, Set_of_cs
};

struct Category{
    Category_kind kind = Set_of_cs;
    Set_of_char   s;
};

Category gc2category(const Generalized_char& gc);

bool operator < (const Category& c1, const Category& c2);

bool operator == (const Category& c1, const Category& c2);

/* Вычисление объединения категорий символов. */
Category operator + (const Category& c1, const Category& c2);

/* Проверяется, пересекаются ли категории c1 и c2. Выдаётся true, если категории
   пересекаются, и false в противном случае. */
bool operator * (const Category& c1, const Category& c2);

/* Функция ниже проверяет, содержится ли (возможно, нестрого) множество символов,
 * описываемое категорией c1, во множестве символов, описываемых категорией c2.
 * Если содержится --- выдаётся true, иначе --- false.
 */
bool is_subcategory(const Category& c1, const Category& c2);

void print_category(const Category& c);
#endif