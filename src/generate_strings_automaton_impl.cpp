/*
    File:    generate_strings_automaton_impl.cpp
    Created: 04 февраля 2017г. в 13:38 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/generate_strings_automaton_impl.h"
#include "../include/belongs.h"
#include "../include/begin_chars_for_aut.h"
#include "../include/add_category.h"
#include "../include/idx_to_string.h"
#include "../include/indent.h"
#include "../include/grouped_DFA_by_uwrapped_regexp.h"
#include "../include/groupped_dfa.h"
#include "../include/automata_repres.h"
#include <string>

static const std::string string_begin_chars_category_name = "STRING_BEGIN";

static const std::string writing_str_into_trie =
    "token.string_index = strs -> insert(buffer);";

void generate_strings_automaton_impl(Info_for_constructing& info)
{
    if(!belongs(String_aut, info.set_of_used_automata)){
        return;
    }

    auto symbols = info.begin_chars[String_beg_char].s;
    auto cat_res = add_category(info, symbols, string_begin_chars_category_name);
    auto strs_begin_category_name = cat_res.second;

    auto temp = "\n    if(belongs(" + strs_begin_category_name +
                ", char_categories)){\n        (loc->pcurrent_char)--; " +
                "automaton = A_string;\n        state = 0;\n";

    auto init_acts_str = idx_to_string(info.et.strs_trie, info.acts_for_strings.init_acts);
    auto fin_acts_str  = idx_to_string(info.et.strs_trie, info.acts_for_strings.fin_acts);

    if(init_acts_str.empty()){
        temp += double_indent + "return t;\n" +
                indent        + "}\n";
    }else{
        temp += double_indent + init_acts_str +"\n" +
                double_indent + "return t;\n" +
                indent        + "}\n";
    }

    info.aut_impl[Start_aut] += temp;

    G_DFA string_GDFA;
    grouped_DFA_by_uwrapped_regexp(string_GDFA, info.strings_regexp, false);

    Str_data_for_automaton f;
    f.automata_name         = info.possible_automata_name_str[String_aut];
    f.proc_name             = info.possible_proc_ptr[String_aut];
    f.category_name_prefix  = "STRING";
    f.diagnostic_msg        = "В строке %zu неожиданно закончился строковый литерал.";
    f.final_states_set_name = "final_states_for_strings";

    temp =  "void " + info.name_of_scaner_class + "::" +
            info.possible_fin_proc_ptr[String_aut] + "{\n" +
            indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
            double_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
            double_indent + "en->increment_number_of_errors();\n" +
            indent + "}\n";

    if(fin_acts_str.empty()){
        f.final_actions =  writing_str_into_trie;
        temp            += indent + writing_str_into_trie + "\n}";
    }else{
        f.final_actions =  fin_acts_str + "\n" +
                           triple_indent + writing_str_into_trie;
        temp            += indent + fin_acts_str + "\n" +
                           indent + writing_str_into_trie + "\n}";
    }

    info.aut_impl[String_aut] = automata_repres(info, string_GDFA, f);
    info.aut_impl_fin_proc[String_aut] = temp;
}