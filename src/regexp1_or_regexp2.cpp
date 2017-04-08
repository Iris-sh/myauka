/*
    File:    regexp1_or_regexp2.cpp
    Created: 05 February 2017 at 17:41 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#include "../include/regexp1_or_regexp2.h"

Unwrapped_commands regexp1_or_regexp2(const Unwrapped_commands& a,
                                      const Unwrapped_commands& b)
{
    if(a.empty()){
        return b;
    }
    if(b.empty()){
        return a;
    }

    Unwrapped_commands result            = a;
    size_t             num_commands_in_a = a.size();
    size_t             or_first_arg      = num_commands_in_a - 1;

    Unwrapped_commands temp              = b;
    for(auto& c : temp){
        switch(c.kind){
            case Unwrapped_kind::Or:      case Unwrapped_kind::Concat:
            case Unwrapped_kind::Multior: case Unwrapped_kind::Multiconcat:
                c.first_arg += num_commands_in_a; c.second_arg += num_commands_in_a;
                break;
            case Unwrapped_kind::Kleene:  case Unwrapped_kind::Positive:
            case Unwrapped_kind::Optional:
                c.first_arg += num_commands_in_a;
                break;
            default:
                ;
        }
    }

    result.insert(result.end(), temp.begin(), temp.end());
    size_t         or_second_arg     = result.size() - 1;

    Unwrapped_command command;
    command.action_name     = 0;
    command.kind            = Unwrapped_kind::Or;
    command.first_arg       = or_first_arg;
    command.second_arg = or_second_arg;
    result.push_back(command);

    return result;
}