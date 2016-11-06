/*
    Файл:    first_chars.cpp
    Создан:  13 января 2016г. в 16:04 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/first_chars.h"

Category first_charsR(const Command_buffer& buf, size_t idx){
    Category         categ;
    Generalized_char gc;

    Command com    = buf[idx];
    switch(com.name){
        case Cmd_or:
            return first_charsR(buf, com.args.first) +
                   first_charsR(buf, com.args.second);

        case Cmd_concat ... Cmd_optional:
            return first_charsR(buf, com.args.first);

        case Cmd_char_def:
            categ.kind = Set_of_cs;
            categ.s    = Set_of_char({com.c});
            return categ;

        case Cmd_char_class_def:
            gc.kind = Char_class;
            gc.cls = static_cast<Char_group>(com.cls);
            return gc2category(gc);                         

        default:
            return categ;
    }
}

Category first_chars(const Command_buffer& buf){
    size_t last_elem_idx = buf.size() - 1;
    return first_charsR(buf, last_elem_idx);
}
