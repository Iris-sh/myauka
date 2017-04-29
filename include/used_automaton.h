/*
    File:    used_automaton.h
    Created: 02 February 2017 at 12:08 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef USED_AUTOMATON_H
#define USED_AUTOMATON_H
/**
 * \brief This enumeration consists of the names of the generated automata.
 */
enum Used_automaton{
    Start_aut,     Unknown_aut,   Id_aut,     Keyword_aut,
    IdKeyword_aut, Delimiter_aut, Number_aut, String_aut,
    Comment_aut
};
#endif