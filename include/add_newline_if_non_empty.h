/*
    File:    add_newline_if_non_empty.h
    Created: 06 June 2017 at 20:56 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef ADD_NEWLINE_IF_NON_EMPTY_H
#define ADD_NEWLINE_IF_NON_EMPTY_H
#include <string>
inline std::string add_newline_if_non_empty(const std::string& s)
{
    return s.empty() ? s : (s + '\n');
}
#endif