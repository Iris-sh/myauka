/*
    File:    grouped_DFA_by_uwrapped_regexp.h
    Created: 2 февраля 2017г. в 10:14 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef GROUPED_DFA_BY_UWRAPPED_REGEXP_H
#define GROUPED_DFA_BY_UWRAPPED_REGEXP_H
#include "../include/unwrapped_command.h"
#include "../include/groupped_dfa.h"
/*
 * @param gdfa         результирующий детерминированный конечный автомат со
 *                     сгруппированными переходами;
 * @param buf          буфер с развёрнутыми командами;
 * @param optimize_com если данный аргумент равен true, то соседние команды Or,
 *                     аргументами которых являются либо Multior, либо Char_def,
 *                     склеиваются в одну команду; иначе склейки не происходит.
 */
void grouped_DFA_by_uwrapped_regexp(G_DFA& gdfa, const Unwrapped_commands& buf,
                                    bool optimize_com);
#endif