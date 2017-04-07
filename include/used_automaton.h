/*
    File:    used_automaton.h
    Created: 02 February 2017г. в 12:08 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef USED_AUTOMATON_H
#define USED_AUTOMATON_H
enum Used_automaton{
    Start_aut,     Unknown_aut,   Id_aut,     Keyword_aut,
    IdKeyword_aut, Delimiter_aut, Number_aut, String_aut,
    Comment_aut
};/* Данное перечисление состоит из имён порождаемых автоматов. */
#endif