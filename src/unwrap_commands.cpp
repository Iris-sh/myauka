/*
    Файл:    unwrap_commands.cpp
    Создан:  24 января 2017г. в 16:17 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#include "../include/unwrap_commands.h"
#include <string>
#include <map>

static const std::u32string latin_upper_letters   = U"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const std::u32string latin_lower_letters   = U"abcdefghijklmnopqrstuvwxyz";
static const std::u32string russian_upper_letters = U"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
static const std::u32string russian_lower_letters = U"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
static const std::u32string binary_digits         = U"01";
static const std::u32string octal_digits          = U"01234567";
static const std::u32string decimal_digits        = U"0123456789";
static const std::u32string hexadecimal_digits    = U"0123456789ABCDEFabcdef";
static const std::u32string upper_letters         =
    latin_upper_letters + russian_upper_letters;
static const std::u32string lower_letters =
    latin_lower_letters + russian_lower_letters;

static std::set<char32_t> u32string2set(const std::u32string& s){
    std::set<char32_t> result;
    for(const char32_t c : s){
        result.insert(c);
    }
    return result;
}

static const std::set<char32_t> sets_for_char_classes[] = {
    u32string2set(latin_upper_letters),   u32string2set(upper_letters),
    u32string2set(russian_upper_letters), u32string2set(binary_digits),
    u32string2set(decimal_digits),        u32string2set(latin_lower_letters),
    u32string2set(lower_letters),         u32string2set(octal_digits),
    u32string2set(russian_lower_letters), u32string2set(hexadecimal_digits)
};

static const Unwrapped_kind cmd_name2unwr_kind[] = {
    Unwrapped_kind::Or,         Unwrapped_kind::Concat,   Unwrapped_kind::Kleene,
    Unwrapped_kind::Positive,   Unwrapped_kind::Optional, Unwrapped_kind::Char_def,
    Unwrapped_kind::Multior,    Unwrapped_kind::Unkwnown, Unwrapped_kind::Multior,
    Unwrapped_kind::Multiconcat
};

Unwrapped_commands unwrap_commands(const Command_buffer& buf){
    Unwrapped_commands result;
    for(const auto com : buf){
        auto              args = com.args;
        auto              kind = com.name;
        Unwrapped_command uc;
        uc.action_name         = com.action_name;
        uc.kind                = cmd_name2unwr_kind[kind];
        switch(kind){
            case Cmd_or: case Cmd_concat: case Cmd_multior: case Cmd_multiconcat:
                uc.first_arg  = args.first;
                uc.second_arg = args.second;
                break;
            case Cmd_Kleene: case Cmd_positive: case Cmd_optional:
                uc.first_arg  = args.first;
                break;
            case Cmd_char_def:
                uc.c          = com.c;
                break;
            case Cmd_char_class_def:
                switch(com.cls){
                    case C_ndq:
                        uc.kind = Unwrapped_kind::Ndq;
                        break;
                    case C_nsq:
                        uc.kind = Unwrapped_kind::Nsq;
                        break;
                    default:
                        uc.s = sets_for_char_classes[com.cls];
                }
                break;
            default:
                ;
        }
        result.push_back(uc);
    }
    return result;
}