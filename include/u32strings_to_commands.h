/*
    Файл:    u32strings_to_commands.h
    Создан:  05 февраля 2017г. в 17:52 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
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