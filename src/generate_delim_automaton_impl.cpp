/*
    Файл:    generate_delim_automaton_impl.cpp
    Создан:  03 февраля 2017г. в 15:49 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/generate_delim_automaton_impl.h"
#include "../include/belongs.h"
#include "../include/attributed_char_trie.h"
#include "../include/errors_and_tries.h"
#include <string>
#include <vector>

static const std::string del_begin_cat_name_by_default = "DELIMITER_BEGIN";

void generate_delim_automaton_impl(Info_for_constructing& info){
    /* Данная функция строит реализацию автомата, обрабатывающего разделители. */
    if(!belongs(Delimiter_aut, info.set_of_used_automata)){
        return;
    }

    std::set<char32_t>          first_chars_for_delims; /* Это множество
       состоит из символов, с которых могут начинаться разделители. */
    Attributed_char_trie        atrie;

    std::vector<std::u32string> delimiter_strings

    for(size_t del_idx : info.del_repres){
        auto delimiter = info.et.strs_trie->get_string(del_idx);
        delimiter_strings.push_back(delimiter);
        first_chars_for_delims.insert(delimiter[0]);
    }
    size_t counter = 0;
    for(size_t del_idx : info.del_repres){
        Attributed_cstring atrib_cstr;
        atrib_cstr.str       = const_cast<char32_t*>(delimiter_strings[counter].c_str());
        atrib_cstr.attribute = (scope_->strsc[del_idx]).code;
        atrie.insert(attributed_cstring2string(atrib_cstr, 1));
        counter++;
    }

    Jumps_and_inits jmps = atrie.jumps(); /* построили заготовку под таблицу переходов */
    /* теперь нужно дописать нужный текст в реализацию стартового автомата
       и сгенерировать функцию, обрабатывающую разделители */
//     auto cat_res = add_category(first_chars_for_delims, del_begin_cat_name_by_default);
//     std::string delimiter_begin_cat_name = cat_res.second;
//
//     aut_impl[Start_aut] += "\n    if(belongs(" + delimiter_begin_cat_name +
//         ", char_categories)){\n        (loc->pcurrent_char)--; " +
//         "automaton = A_delimiter;\n        state = -1;\n        return t;\n    }\n";
//
//     auto del_postact = get_act_repres(del_postaction);
//
//     aut_impl[Delimiter_aut] = jump_table_string_repres(jmps, del_jump_table_name,
//                                                        del_init_table_name) +
//                               "bool " + name_of_scaner_class + delim_proc_body(del_postact);
//
//     aut_impl_fin_proc[Delimiter_aut] = "void " + name_of_scaner_class +
//                                        R"~(::delimiter_final_proc(){
//     )~" + indent + del_postact +
//     R"~(
//     token.code = delim_jump_table[state].code;
//     )~" + "\n}";
}
