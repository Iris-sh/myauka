/*
    File:    regexp1_with_regexp2ast.cpp
    Created: 05 February 2017г. в 11:20 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#include "../include/regexp1_with_regexp2ast.h"

Unwrapped_commands regexp1_with_regexp2ast(const Unwrapped_commands& a,
                                           const Unwrapped_commands& b)
{
    Unwrapped_commands x                 = a;
    Unwrapped_commands y                 = b;
    size_t             num_commands_in_a = x.size();
    size_t             last_idx_in_a     = num_commands_in_a -1;

    for(auto& c : y){
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
    x.insert(x.end(), y.begin(), y.end());

    size_t         last_idx_in_a_appended_with_b = x.size() - 1;
    Unwrapped_command com;
    com.action_name = 0;
    com.kind        = Unwrapped_kind::Kleene;
    com.first_arg   = last_idx_in_a_appended_with_b;
    com.second_arg  = 0;

    x.push_back(com);

    com.action_name = 0;
    com.kind        = Unwrapped_kind::Concat;
    com.first_arg   = last_idx_in_a;
    com.second_arg  = x.size() - 1;

    x.push_back(com);

    return x;
}