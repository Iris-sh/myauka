/*
    Файл:    first_chars.cpp
    Создан:  13 января 2016г. в 16:04 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/first_chars.h"
#include "../include/sets_for_classes.h"

Category command2category(const Command& com){
    Category result;
    switch(com.name){
        case Cmd_char_def:
            result.kind = Set_of_cs;
            result.s    = Set_of_char({com.c});
            break;
        case Cmd_char_class_def:
            result.s    = sets_for_char_classes[com.cls];
            switch(com.cls){
                case C_ndq:
                    result.kind = Not_double_quote;
                    break;
                case C_nsq:
                    result.kind = Not_single_quote;
                    break;
                default:
                    result.kind = Set_of_cs;
            }
            break;
        default:
            ;
    }
    return result;
}

Category first_charsR(const Command_buffer& buf, size_t idx){
    Category  categ;
    Command   com    = buf[idx];
    switch(com.name){
        case Cmd_or:
            return first_charsR(buf, com.args.first) +
                   first_charsR(buf, com.args.second);

        case Cmd_concat ... Cmd_optional:  case Cmd_multiconcat:
            return first_charsR(buf, com.args.first);

        case Cmd_char_def: case Cmd_char_class_def:
            return command2category(com);

        case Cmd_multior:
            categ.kind = Set_of_cs;
            for(size_t i = com.args.first; i <= com.args.second; i++){
                categ = categ + command2category(buf[i]);
            }
            return categ;

        default:
            return categ;
    }
}

Category first_chars(const Command_buffer& buf){
    size_t last_elem_idx = buf.size() - 1;
    return first_charsR(buf, last_elem_idx);
}