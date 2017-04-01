/*
    File:    automata_repres.cpp
    Created: 04 февраля 2017г. в 17:55 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/automata_repres.h"
#include "../include/add_category.h"
#include "../include/get_act_repres.h"
#include "../include/str_repres_for_set_of_size_t_const.h"
#include "../include/indent.h"

static const std::string sp_else_sp = " else ";

static std::string check_there_is_jump(const Str_data_for_automaton& f){
    std::string result;
    result = indent        + "if(!there_is_jump){\n" +
             double_indent + "t = false;\n"          +
             double_indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
             triple_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
             triple_indent + "en->increment_number_of_errors();\n" + double_indent + "}\n";
    auto temp = f.final_actions;
    if(!temp.empty()){
        result += double_indent + temp + "\n";
    }
    result += indent + "}\n";
    return result;
}

static std::string qindent_string(const std::string& s){
    std::string result;
    if(!s.empty()){
        result = quadruple_indent + s + "\n";
    }
    return result;
}

static std::string automata_repres_case_j(Info_for_constructing& info,
                                          const Category&              cat,
                                          const DFA_state_with_action& swa,
                                          const Str_data_for_automaton& f,
                                          size_t& last_category_suffix)
{
    std::string result;
    auto        temp   = get_act_repres(info, swa.action_idx);
    switch(cat.kind){
        case All_chars:
            if(!temp.empty()){
                result = temp + "\n" +
                         triple_indent + "state = " + std::to_string(swa.st) + ";\n" +
                         triple_indent + "there_is_jump = true;\n";
            }else{
                result = "state = " +  std::to_string(swa.st) + ";\n" +
                         triple_indent + "there_is_jump = true;\n";
            }
            break;
        case Not_single_quote:
            result = R"~( else if(ch != U'\''){)~" "\n" + qindent_string(temp) +
                     quadruple_indent + "state = " + std::to_string(swa.st) + ";\n" +
                     quadruple_indent + "there_is_jump = true;\n" +
                     triple_indent + "}\n";
            break;
        case Not_double_quote:
            result = R"~( else if(ch != U'\"'){)~" "\n" + qindent_string(temp) +
                     quadruple_indent + "state = " + std::to_string(swa.st) + ";\n" +
                     quadruple_indent + "there_is_jump = true;\n" +
                     triple_indent + "}\n";
            break;
        case Set_of_cs:
            {
                std::string default_cat_name = f.category_name_prefix +
                                               std::to_string(last_category_suffix);
                auto result_cat = add_category(info, cat.s, default_cat_name);
                if(result_cat.first){
                    // если категории до того не было
                    last_category_suffix++;
                }
                result = " else if(belongs(" + result_cat.second +
                         ", char_categories)){\n" + qindent_string(temp) +
                         quadruple_indent + "state = " + std::to_string(swa.st) + ";\n" +
                         quadruple_indent + "there_is_jump = true;\n" +
                         triple_indent + "}\n";
            }
            break;
    }
    return result;
}

static std::string automata_repres_case(Info_for_constructing& info,
                                        const   G_DFA_state_jumps& m,
                                        const   Str_data_for_automaton& f,
                                        size_t  counter,
                                        size_t& last_category_suffix)
{
#define ELSE_SPACE_WIDTH 5
    std::string result;
    if(m.empty()){
        return result;
    }
    result = double_indent + "case " + std::to_string(counter) + ":\n";
    std::vector<std::string> ifs;
    for(const auto& j : m){
        auto cat  = j.first;
        auto swa  = j.second;
        auto temp = automata_repres_case_j(info, cat, swa, f, last_category_suffix);
        ifs.push_back(temp);
    }
    if(ifs[0].substr(0, ELSE_SPACE_WIDTH + 1) == sp_else_sp){
        ifs[0] = ifs[0].substr(ELSE_SPACE_WIDTH + 1);
        // ELSE_SPACE_WIDTH + 1 --- длина строки " else "
    }
    for(const auto& x : ifs){
        result += triple_indent + x;
    }

    result += "\n" + triple_indent + "break;\n";
    return result;
}

static std::string automata_repres_switch(Info_for_constructing& info,
                                          const G_DFA& aut,
                                          const Str_data_for_automaton& f){
    std::string result;
    result                      = indent + "switch(state){\n";
    size_t counter              = 0;
    size_t last_category_suffix = 0;
    for(const auto& m : aut.jumps){
        result += automata_repres_case(info, m, f, counter,last_category_suffix);
        counter++;
    }
    result += double_indent + "default:\n" +
              triple_indent + ";\n" +
              indent        + "}\n";
    return result;
}

std::string automata_repres(Info_for_constructing& info, const G_DFA& aut,
                            const Str_data_for_automaton& f)
{
    std::string result;

    std::string proc_def = "bool " + info.name_of_scaner_class + "::" + f.proc_name + "{\n" +
                           indent + "bool t             = true;\n" +
                           indent + "bool there_is_jump = false;\n";
    proc_def += automata_repres_switch(info, aut, f) + "\n";
    proc_def += check_there_is_jump(f) + "\n";
    proc_def += indent + "return t;\n}";
    result = str_repres_for_set_of_size_t_const(aut.final_states, f.final_states_set_name) +
             "\n\n" + proc_def;
    return result;
}