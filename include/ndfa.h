/*
    Файл:    ndfa.h
    Создан:  13 декабря 2015г. в 09:05 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef NDFA_H
#define NDFA_H

#include <set>
#include <map>
#include <vector>
#include <utility>

#include "../include/command.h"
#include "../include/generalized_char.h"

using Set_of_states         = std::set<size_t>;
using States_with_action    = std::pair<Set_of_states, size_t>;
using NDFA_state_jumps      = std::map<Generalized_char, States_with_action>;
using NDFA_jumps            = std::vector<NDFA_state_jumps>;

struct NDFA{
    NDFA_jumps jumps;
    size_t     begin_state;
    size_t     final_state;

    NDFA()                 = default;
    ~NDFA()                = default;
    NDFA(const NDFA& orig) = default;
};

/*! Данная функция строит по соответствующему регулярному выражению списку команд
   недетерминированный конечный автомат, отвечающий регулярному выражению.  */
void build_NDFA(NDFA& a, const Command_buffer& commands);

/*! Печать недетерминированного конечного автомата a. */
// void print_NDFA(const NDFA& a);
#endif