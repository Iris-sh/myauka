/*
    File:    generalized_char.cpp
    Created: 13 December 2015 at 09:05 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/generalized_char.h"
#include "../include/char_conv.h"
#include <cstdio>

static const char* gc_groups[] = {
    "G_Latin",    "G_Letter",  "G_Russian",
    "G_bdigits",  "G_digits",  "G_latin",
    "G_letter",   "G_odigits", "G_russian",
    "G_xdigits",  "G_ndq",     "G_nsq",
    "G_all"
};

void print_generalized_char(const Generalized_char& gc){
    switch(gc.kind){
        case Epsilon:
            printf("epsilon");
            break;
        case Char:
            {
                auto s = char32_to_utf8(gc.c);
                printf("'%s'",s.c_str());
            }
            break;
        case Char_class:
            printf("%s",gc_groups[gc.cls]);
            break;
    }
}

void print_size_t(const size_t& x){
    printf("%zu",x);
}

bool operator < (const Generalized_char gc1, const Generalized_char gc2){
    if(gc1.kind != gc2.kind){
        return gc1.kind < gc2.kind;
    }

    switch(gc1.kind){
        case Char:
            return gc1.c < gc2.c;

        case Char_class:
            return gc1.cls < gc2.cls;

        default:
            return false;
    }
}