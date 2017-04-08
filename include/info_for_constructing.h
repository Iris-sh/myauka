/*
    File:    info_for_constructing.h
    Created: 02 February 2017 at 10:36 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef INFO_FOR_CONSTRUCTING_H
#define INFO_FOR_CONSTRUCTING_H
#include <map>
#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>
#include <memory>
#include "../include/unwrapped_command.h"
#include "../include/trie_for_set_of_char.h"
#include "../include/used_automaton.h"
#include "../include/scope.h"
#include "../include/errors_and_tries.h"
#include "../include/categories.h"
#include "../include/init_and_final_acts.h"
#include "../include/automaton_with_procs.h"

/* Структура данных для порождения реализации сканера и заголовочного файла сканера. */
struct Info_for_constructing{
    Unwrapped_commands                    id_begin;
    Unwrapped_commands                    id_body;
    Unwrapped_commands                    numbers_regexp;
    Unwrapped_commands                    strings_regexp;
    Trie_for_set_of_char                  char_cat;
    std::map<size_t, std::string>         category_name;
    std::string                           name_of_scaner_class;
    std::string                           scaner_file_name_without_ext;
    std::string                           header_name;
    std::string                           impl_file_name;
    std::map<Used_automaton, std::string> aut_impl;
    std::map<Used_automaton, std::string> aut_impl_fin_proc;
    std::vector<size_t>                   del_repres;
    std::vector<size_t>                   kw_repres;
    std::vector<size_t>                   codes;
    std::vector<Automaton_with_procs>     automaton_info;
    Errors_and_tries                      et;
    std::shared_ptr<Scope>                scope;
    std::string                           codes_type_name;
    std::string                           lexem_info_name;
    std::string                           fields_for_comments_handling;
    Init_and_final_acts                   acts_for_strings;
    Init_and_final_acts                   acts_for_numbers;
    Category*                             begin_chars                        = nullptr;
    std::string*                          possible_automata_name_str         = nullptr;
    std::string*                          possible_proc_ptr                  = nullptr;
    std::string*                          possible_fin_proc_ptr              = nullptr;
    std::string*                          possible_automata_proc_proto       = nullptr;
    std::string*                          possible_automata_final_proc_proto = nullptr;
    uint64_t                              set_of_used_automata               = 0;
    size_t                                del_postaction                     = 0;
    size_t                                keyword_postaction                 = 0;
    size_t                                write_action_name_idx              = 0;
    size_t                                mark_of_single_lined               = 0;
    size_t                                mark_of_multilined_begin           = 0;
    size_t                                mark_of_multilined_end             = 0;
    size_t                                token_fields_idx                   = 0;
    bool                                  there_is_Elem_definition;
    bool                                  multilined_is_nested;

    Info_for_constructing()                             = default;
    Info_for_constructing(const Info_for_constructing&) = default;
    ~Info_for_constructing()                            = default;
};
#endif