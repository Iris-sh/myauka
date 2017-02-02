/*
    Файл:    unwrapped_command.h
    Создан:  24 января 2017г. в 17:29 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#ifndef UNWRAPPED_COMMAND_H
#define UNWRAPPED_COMMAND_H
#include <set>
#include <vector>
#include <cstdint>
#include <cstddef>
enum class Unwrapped_kind : uint32_t {
    Or,       Concat,     Kleene,
    Positive, Optional,   Char_def,
    Multior,  Ndq,        Nsq,
    Unkwnown, Multiconcat
};

struct Unwrapped_command{
    Unwrapped_kind     kind;
    char32_t           c           = 0;
    size_t             action_name = 0;
    size_t             first_arg   = 0;
    size_t             second_arg  = 0;
    std::set<char32_t> s;

    Unwrapped_command()                         = default;
    ~Unwrapped_command()                        = default;
    Unwrapped_command(const Unwrapped_command&) = default;
};

using Unwrapped_commands = std::vector<Unwrapped_command>;
#endif