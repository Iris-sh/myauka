/*
    Файл:    u32strings_to_commands.cpp
    Создан:  05 февраля 2017г. в 17:56 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#include "../include/u32strings_to_commands.h"
#include "../include/regexp1_or_regexp2.h"

static Unwrapped_commands u32string_to_commands(const std::u32string& str){
    Unwrapped_commands result;

    if(str.empty()){
        return result;
    }

    Unwrapped_command command;
    command.action_name = 0;
    command.kind        = Unwrapped_kind::Char_def;
    command.c           = str[0];
    result.push_back(command);

    auto temp = str.substr(1);
    if(temp.empty()){
        return result;
    }

    for(char32_t ch : temp){
        command.action_name      = 0;
        command.kind             = Unwrapped_kind::Char_def;
        command.c                = ch;

        size_t concat_first_arg  = result.size() - 1;
        result.push_back(command);
        size_t concat_second_arg = result.size() - 1;

        command.kind             = Unwrapped_kind::Concat;
        command.first_arg        = concat_first_arg;
        command.second_arg       = concat_second_arg;
        result.push_back(command);
    }
    return result;
}


Unwrapped_commands u32strings_to_commands(const std::vector<std::u32string>& s){
    Unwrapped_commands result;
    for(const auto& str : s){
        auto current_commands = u32string_to_commands(str);
        result                = regexp1_or_regexp2(result, current_commands);
    }
    return result;

}