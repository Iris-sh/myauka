/*
    File:    init_and_final_acts.h
    Created: 04 February 2017 at 14:39 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef INIT_AND_FINAL_ACTS_H
#define INIT_AND_FINAL_ACTS_H
/* User actions performed during initialization and at the completion of
 * the automaton of string handling (or numbers handling) which is generated
 * from the description of the scanner.
*/

struct Init_and_final_acts{
    size_t init_acts = 0;
    size_t fin_acts  = 0;
};
#endif