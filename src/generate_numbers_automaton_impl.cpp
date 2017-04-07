/*
    File:    generate_numbers_automaton_impl.cpp
    Created: 04 February 2017г. в 21:31 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/generate_numbers_automaton_impl.h"
#include "../include/belongs.h"
#include "../include/begin_chars_for_aut.h"
#include "../include/add_category.h"
#include "../include/idx_to_string.h"
#include "../include/indent.h"
#include "../include/grouped_DFA_by_uwrapped_regexp.h"
#include "../include/groupped_dfa.h"
#include "../include/automata_repres.h"
#include <string>

static const std::string number_begin_chars_category_name = "NUMBER_BEGIN";

void generate_numbers_automaton_impl(Info_for_constructing& info){
    if(!belongs(Number_aut, info.set_of_used_automata)){
        return;
    }
    G_DFA num_GDFA;
    grouped_DFA_by_uwrapped_regexp(num_GDFA, info.numbers_regexp, false);

    auto init_acts_num = idx_to_string(info.et.strs_trie, info.acts_for_numbers.init_acts);
    auto fin_acts_num  = idx_to_string(info.et.strs_trie, info.acts_for_numbers.fin_acts);

    auto symbols = info.begin_chars[Number_beg_char].s;
    auto cat_res = add_category(info,symbols, number_begin_chars_category_name);
    auto num_begin_category_name = cat_res.second;

    auto temp = "\n    if(belongs(" + num_begin_category_name +
                ", char_categories)){\n        (loc->pcurrent_char)--; " +
                "automaton = A_number;\n        state = 0;\n";

    if(init_acts_num.empty()){
        temp += double_indent + "return t;\n" +
                indent        + "}\n";
    }else{
        temp += double_indent + init_acts_num +"\n" +
                double_indent + "return t;\n" +
                indent        + "}\n";
    }

    info.aut_impl[Start_aut] += temp;

    Str_data_for_automaton f;
    f.automata_name         = info.possible_automata_name_str[Number_aut];
    f.proc_name             = info.possible_proc_ptr[Number_aut];
    f.category_name_prefix  = "NUMBER";
    f.diagnostic_msg        = "В строке %zu неожиданно закончилось число.";
    f.final_states_set_name = "final_states_for_numbers";

    temp = "void " + info.name_of_scaner_class + "::" +
           info.possible_fin_proc_ptr[Number_aut] + "{\n" +
           indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
           double_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
           double_indent + "en->increment_number_of_errors();\n" +
           indent + "}\n";

    f.final_actions = fin_acts_num;

    if(fin_acts_num.empty()){
        temp += "}\n";
    }else{
        temp += indent + fin_acts_num + "\n}";
    }

    info.aut_impl[Number_aut]          = automata_repres(info, num_GDFA, f);
    info.aut_impl_fin_proc[Number_aut] = temp;
}