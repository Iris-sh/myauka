/*
    File:    generate_delim_automaton_impl.cpp
    Created: 03 февраля 2017г. в 15:49 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/generate_delim_automaton_impl.h"
#include "../include/belongs.h"
#include "../include/attributed_char_trie.h"
#include "../include/errors_and_tries.h"
#include "../include/idx_to_string.h"
#include "../include/jump_table_string_repres.h"
#include "../include/add_category.h"
#include "../include/get_act_repres.h"
#include "../include/indent.h"
#include <string>
#include <vector>

static const std::string del_begin_cat_name_by_default = "DELIMITER_BEGIN";
static const std::string del_jump_table_name           = "delim_jump_table";
static const std::string del_init_table_name           = "init_table_for_delimiters";
static const std::string delim_proc_body_              = R"~(::delimiter_proc(){
    bool t = false;
    if(-1 == state){
        state = get_init_state(ch, init_table_for_delimiters,
                               sizeof(init_table_for_delimiters)/sizeof(State_for_char));
        token.code = delim_jump_table[state].code;
        t = true;
        return t;
    }
    Elem elem = delim_jump_table[state];
    token.code = delim_jump_table[state].code;
    int y = search_char(ch, elem.symbols);
    if(y != THERE_IS_NO_CHAR){
        state = elem.first_state + y; t = true;
    })~";

static std::string delim_proc_body(const std::string& s){
    std::string result;
    result = delim_proc_body_;
    if(s.empty()){
        result += "\n" + indent + "return t;\n}";
    }else{
        result += "\n" +
                  indent + "if(!t){\n" +
                  double_indent + s + "\n" +
                  indent + "}\n" +
                  indent + "return t;\n}";
    }
    return result;
}

void generate_delim_automaton_impl(Info_for_constructing& info){
    /* Данная функция строит реализацию автомата, обрабатывающего разделители. */
    if(!belongs(Delimiter_aut, info.set_of_used_automata)){
        return;
    }

    std::set<char32_t>          first_chars_for_delims; /* Это множество
       состоит из символов, с которых могут начинаться разделители. */
    Attributed_char_trie        atrie;

    std::vector<std::u32string> delimiter_strings;

    for(size_t del_idx : info.del_repres){
        auto delimiter = info.et.strs_trie->get_string(del_idx);
        delimiter_strings.push_back(delimiter);
        first_chars_for_delims.insert(delimiter[0]);
    }
    size_t counter = 0;
    for(size_t del_idx : info.del_repres){
        Attributed_cstring atrib_cstr;
        atrib_cstr.str       = const_cast<char32_t*>(delimiter_strings[counter].c_str());
        auto& scope_         = info.scope;
        atrib_cstr.attribute = (scope_->strsc[del_idx]).code;
        atrie.insert(attributed_cstring2string(atrib_cstr, 1));
        counter++;
    }

    Jumps_and_inits jmps = atrie.jumps(); /* построили заготовку под таблицу переходов */
    /* теперь нужно дописать нужный текст в реализацию стартового автомата
       и сгенерировать функцию, обрабатывающую разделители */
    auto cat_res = add_category(info, first_chars_for_delims, del_begin_cat_name_by_default);
    std::string delimiter_begin_cat_name = cat_res.second;

    info.aut_impl[Start_aut] += "\n    if(belongs(" + delimiter_begin_cat_name +
        ", char_categories)){\n        (loc->pcurrent_char)--; " +
        "automaton = A_delimiter;\n        state = -1;\n        return t;\n    }\n";

    auto del_postact = get_act_repres(info, info.del_postaction);

    info.aut_impl[Delimiter_aut] = jump_table_string_repres(info, jmps, del_jump_table_name,
                                                            del_init_table_name) +
                              "bool " + info.name_of_scaner_class + delim_proc_body(del_postact);

    info.aut_impl_fin_proc[Delimiter_aut] = "void " + info.name_of_scaner_class +
                                       R"~(::delimiter_final_proc(){
    )~" + indent + del_postact +
    R"~(
    token.code = delim_jump_table[state].code;
    )~" + "\n}";
}