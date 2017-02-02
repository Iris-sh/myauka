/*
    Файл:    command.h
    Создан:  13 декабря 2015г. в 09:05 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <cstddef>
/* В этом файле определяются команды, в которые транслируются
 * регулярные выражения. */

enum Command_name {
    Cmd_or,             Cmd_concat,   Cmd_Kleene,
    Cmd_positive,       Cmd_optional, Cmd_char_def,
    Cmd_char_class_def, Cmd_unknown,  Cmd_multior,
    Cmd_multiconcat
};

enum Char_class {
    C_Latin,    C_Letter,       C_Russian,
    C_bdigits,  C_digits,       C_latin,
    C_letter,   C_odigits,      C_russian,
    C_xdigits,  C_ndq,          C_nsq
};

struct Command {
    size_t action_name; /* Индекс идентификатора, являющегося
                         * именем действия, в префиксном дереве
                         * идентификаторов. */
    Command_name name;
    union{
        struct {
            size_t first, second;
        } args;
        char32_t   c;
        Char_class cls;
    };
};

using Command_buffer = std::vector<Command>;

#endif