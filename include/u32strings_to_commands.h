/*
    File:    u32strings_to_commands.h
    Created: 05 February 2017 at 17:52 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/
#ifndef U32STRINGS_TO_COMMANDS_H
#define U32STRINGS_TO_COMMANDS_H
#include "../include/unwrapped_command.h"
#include <vector>
#include <string>
Unwrapped_commands u32strings_to_commands(const std::vector<std::u32string>& s);
#endif