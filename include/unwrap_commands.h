/*
    File:    unwrap_commands.h
    Created: 24 January 2017 at 15:58 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#ifndef UNWRAP_COMMANDS_H
#define UNWRAP_COMMANDS_H
#include "../include/unwrapped_command.h"
#include "../include/command.h"
/**
 * \brief This function converts the buffer with the written commands, unwrapping
 *        the symbol classes (except the classes [: nsq:] and [: ndq:]).
 * \param [in] buf Buffer containing commands.
 * \return         Buffer containing unwrapped commands.
 */
Unwrapped_commands unwrap_commands(const Command_buffer& buf);
#endif