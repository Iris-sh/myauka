/*
    File:    ndfa.h
    Created: 13 December 2015 at 09:05 Moscow time
    Author:  Гаврилов Владимир Сергеевич
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
#include "../include/unwrapped_command.h"
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

/**
 * \param [in]  commands The list of commands corresponding to a regular expression.
 * \param [out] a        Corresponding non-deterministic finite automaton.
 */
void build_NDFA(NDFA& a, const Unwrapped_commands& commands);

/**
 * \brief         Prints non-deterministic finite automaton.
 * \param [in] a  Non-deterministic finite automaton.
 */
void print_NDFA(const NDFA& a);
#endif