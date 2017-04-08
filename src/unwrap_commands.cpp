/*
    File:    unwrap_commands.cpp
    Created: 24 January 2017 at 16:17 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#include "../include/unwrap_commands.h"
#include "../include/sets_for_classes.h"
#include <string>
#include <map>

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