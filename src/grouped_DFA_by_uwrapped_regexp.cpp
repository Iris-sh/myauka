/*
    File:    grouped_DFA_by_uwrapped_regexp.cpp
    Created: 2 February 2017 at 10:30 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/grouped_DFA_by_uwrapped_regexp.h"
#include "../include/ndfa.h"
#include "../include/dfa.h"
#include "../include/fuse_unwrapped_commands.h"
#include <algorithm>
#include "../include/operations_with_sets.h"

using namespace operations_with_sets;

static void fuse_DFA_jumps(G_DFA& gdfa, const Min_DFA& min_dfa){
    auto& mjs = min_dfa.jumps;

    G_DFA_jumps groupped_jumps;

    for(const auto& mj : mjs){
        /* в mj --- переходы для текущего состояния автомата min_dfa (напомним, что mjs
           --- вектор из переходов для состояний, т.е. элемент в этом векторе, имеющий
           индекс 0 --- переходы для состояния 0; элемент с индексом 1 --- переходы для
          состояния 1, и т.д.) */
        if(mj.empty()){
            /* если в исходном автомате для текущего состояния переходов нет, то
             * их нет и в автомате со сгруппированными переходами, так что никаких
             * действий по группировке делать не нужно */
            groupped_jumps.push_back(G_DFA_state_jumps());
            continue;
        }
        using Gc_DFA_state_action = std::pair<Generalized_char, DFA_state_with_action>;
        std::vector<Gc_DFA_state_action> j;
        for(const auto x : mj){
            j.push_back(x);
        }
        std::sort(j.begin(), j.end(),
                  [](const Gc_DFA_state_action& a, const Gc_DFA_state_action& b)->bool{
                      return (a.second.st != b.second.st) ? (a.second.st < b.second.st) :
                             (a.second.action_idx < b.second.action_idx);
                  });
        /* теперь вектор j отсортирован так, что пары с
           одинаковой второй компонентой идут подряд */
        /* сгруппируем теперь такие пары */
        G_DFA_state_jumps     groupped_sj;
        Category              current_category = gc2category(j[0].first);
        DFA_state_with_action sa               = j[0].second;
        for(auto p : j){
            auto psa = p.second;
            if((psa.st == sa.st) && (psa.action_idx == sa.action_idx)){
                current_category = current_category + gc2category(p.first);
            }else{
                groupped_sj[current_category] = sa;
                current_category              = gc2category(p.first);
                sa                            = p.second;
            }
        }
        groupped_sj[current_category] = sa;
        groupped_jumps.push_back(groupped_sj);
    }
    gdfa.jumps        = groupped_jumps;
    gdfa.begin_state  = min_dfa.begin_state;
    gdfa.final_states = min_dfa.final_states;
}

void grouped_DFA_by_uwrapped_regexp(G_DFA& gdfa, const Unwrapped_commands& buf,
                                    bool optimize)
{
    Unwrapped_commands buf_ = buf;
    NDFA               ndfa;
    DFA                dfa;
    Min_DFA            mdfa;
    if(optimize){
        buf_ = fuse_unwrapped_commands(buf_);
    }
    build_NDFA(ndfa, buf_);
    convert_NDFA_to_DFA(dfa, ndfa);
    minimize_DFA(mdfa, dfa);
    fuse_DFA_jumps(gdfa, mdfa);
}