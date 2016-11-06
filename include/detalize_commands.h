/*
    Файл:    detalize_commands.h
    Создан:  29 марта 2016г. в 14:26 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#ifndef DETALIZE_COMMANDS_H
#define DETALIZE_COMMANDS_H

#include "../include/command.h"

void detalize_commands(Command_buffer& buf);
/* Данная функция преобразует буфер с записанными командами, разворачивая
 * классы символов (кроме классов [:nsq:] и [:ndq:]). */
#endif
