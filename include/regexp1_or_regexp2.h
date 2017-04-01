/*
    File:    regexp1_or_regexp2.h
    Created: 05 февраля 2017г. в 17:37 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/
#ifndef REGEXP1_OR_REGEXP2_H
#define REGEXP1_OR_REGEXP2_H
#include "../include/unwrapped_command.h"
/* Возвращает команды, соответствующие регулярке a | b. */
Unwrapped_commands regexp1_or_regexp2(const Unwrapped_commands& a,
                                      const Unwrapped_commands& b);
#endif