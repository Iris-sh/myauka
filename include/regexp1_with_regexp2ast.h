/*
    File:    regexp1_with_regexp2ast.h
    Created: 05 февраля 2017г. в 11:15 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/
#ifndef REGEXP1_WITH_REGEXP2AST_H
#define REGEXP1_WITH_REGEXP2AST_H
#include "../include/unwrapped_command.h"
/* Построение по командам, отвечающим регуляркам a и b,
 * команд, отвечающих регулярке ab*. */
Unwrapped_commands regexp1_with_regexp2ast(const Unwrapped_commands& a,
                                           const Unwrapped_commands& b);
#endif