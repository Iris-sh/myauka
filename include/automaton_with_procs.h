/*
    File:    automaton_with_procs.h
    Created: 05 February 2017 at 21:21 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef AUTOMATON_WITH_PROCS_H
#define AUTOMATON_WITH_PROCS_H
#include <string>
struct Automaton_with_procs{
    std::string name;
    std::string proc_proto;
    std::string fin_proc_proto;
    std::string proc_ptr;
    std::string fin_proc_ptr;

    Automaton_with_procs()                            = default;
    ~Automaton_with_procs()                           = default;
    Automaton_with_procs(const Automaton_with_procs&) = default;
};
#endif