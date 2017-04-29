/*
    File:    regexp1_with_regexp2ast.h
    Created: 05 February 2017 at 11:15 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/
#ifndef REGEXP1_WITH_REGEXP2AST_H
#define REGEXP1_WITH_REGEXP2AST_H
#include "../include/unwrapped_command.h"
/**
 *  \brief Returns commands corresponding to regexp ab*.
 *  \param [in] a The commands that define the regular expression a.
 *  \param [in] b The commands that define the regular expression b.
 *  \return       The commands that define the regular expression ab*.
 */
Unwrapped_commands regexp1_with_regexp2ast(const Unwrapped_commands& a,
                                           const Unwrapped_commands& b);
#endif