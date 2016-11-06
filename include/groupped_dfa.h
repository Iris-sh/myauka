/*
    Файл:    groupped_dfa.h
    Создан:  14 января 2016г. в 18:19 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef GROUPPED_DFA_H
#define GROUPPED_DFA_H

#include <vector>
#include <map>
#include <set>
#include <utility>
#include "../include/ndfa.h"
#include "../include/dfa.h"
#include "../include/categories.h"
#include "../include/command.h"

// struct DFA_state_with_action{
//     size_t st;
//     size_t action_idx;
// };

using G_DFA_state_jumps = std::map<Category, DFA_state_with_action>;
using G_DFA_jumps       = std::vector<G_DFA_state_jumps>;


struct G_DFA{
    G_DFA_jumps      jumps;
    size_t           begin_state = 0;
    std::set<size_t> final_states;

    G_DFA()                  = default;
    ~G_DFA()                 = default;
    G_DFA(const G_DFA& orig) = default;
};

//void fuse_DFA_jumps(G_DFA& gdfa, const Min_DFA& min_dfa);

void grouped_DFA_by_regexp(G_DFA& gdfa, const Command_buffer& buf);

void print_groupped_DFA(const G_DFA& gdfa);
#endif