/*
    File:    generalized_char.h
    Created: 13 December 2015г. в 09:05 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef GENERALIZED_CHAR_H
#define GENERALIZED_CHAR_H

#include <cstddef>
enum Char_group {
    G_ndq,      G_nsq,    G_all
};

enum Kind_of_char {
    Epsilon, Char, Char_class
};

struct Generalized_char {
    Kind_of_char kind;
    union{
        char32_t   c;
        Char_group cls;
    };
};

bool operator < (const Generalized_char gc1, const Generalized_char gc2);

void print_generalized_char(const Generalized_char& gc);

void print_size_t(const size_t& x);
#endif