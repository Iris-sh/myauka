/*
    Файл:    main_parser_data.h
    Создан:  14 декабря 2015г. в 15:25 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef MAIN_PARSER_DATA_H
#define MAIN_PARSER_DATA_H

#include <memory>
#include <vector>
#include <string>
#include <set>
#include <map>
#include "../include/location.h"
#include "../include/error_count.h"
#include "../include/trie.h"
#include "../include/scope.h"
#include "../include/main_scaner.h"
#include "../include/errors_and_tries.h"
#include "../include/command.h"
#include "../include/expr_scaner.h"
#include "../include/simple_regex_parser.h"
#include "../include/act_expr_parser.h"
#include "../include/categories.h"
#include "../include/trie_for_set_of_char.h"
#include "../include/codes_and_newline.h"
#include "../include/keyword_delimiter_parser.h"
#include "../include/num_str_parser.h"
#include "../include/comments_parser.h"
#include "../include/used_automaton.h"
#include "../include/begin_chars_for_aut.h"
#include "../include/automaton_with_procs.h"

// enum Begin_chars_for_aut{
//     Id_beg_char,     Keyword_beg_char, Delimiter_beg_char,
//     Number_beg_char, String_beg_char,  Comment_beg_char
// };

class Main_parser_data {
public:
    Main_parser_data();
    ~Main_parser_data()                            = default;
    Main_parser_data(const Main_parser_data& orig) = default;

    Main_parser_data(Location_ptr loc) : Main_parser_data()
        {init_end(loc);};
protected:
    Errors_and_tries                     et_;
    std::shared_ptr<Scope>               scope_;
    std::shared_ptr<Main_scaner>         msc;
    std::shared_ptr<Expr_scaner>         expr_sc;
    std::shared_ptr<Simple_regex_parser> id_definition_parser;
    std::shared_ptr<Act_expr_parser>     num_and_str_parser;
    std::shared_ptr<Codes_and_newline>   codes_and_newline;
    std::shared_ptr<KW_parser>           keywords_sec_parser;
    std::shared_ptr<KW_parser>           delimiters_sec_parser;
    std::shared_ptr<NS_parser>           num_sec_parser;
    std::shared_ptr<NS_parser>           strs_sec_parser;
    std::shared_ptr<Comments_parser>     comments_parser;

    size_t scaner_name_idx; /* Индекс идентификатора, являющегося
                             * именем сканера, в префиксном дереве
                             * идентификаторов. */
    size_t codes_type_name_idx; /* Индекс идентификатора, являющегося
                                 * именем типа кодов лексем, в
                                 * префиксном дереве идентификаторов. */
    size_t ident_name_idx;      /* Индекс идентификатора, являющегося
                                 * кодом лексемы 'идентификатор', в
                                 * префиксном дереве идентификаторов. */

    size_t token_fields_idx; /* Индекс строкового литерала с добавляемыми
                              * в тип, описывающий сведения о лексеме,
                              * полями. */
    size_t class_members_idx; /* Индекс строкового литерала с добавляемыми
                               * в класс сканера полями. */

    bool newline_is_lexem;

    std::vector<size_t> codes; /* Вектор из индексов идентификаторов
                                * из перечисления, определяющего коды
                                * лексем. */

    size_t last_code_val; /* Последнее использованное числовое значение
                           * кода лексемы. */

    std::string name_of_scaner_class;
    std::string scaner_file_name_without_ext;
    std::string codes_type_name;
    std::string lexem_info_name;

    std::vector<size_t> kw_repres; /* Вектор из индексов строковых литералов,
                                    * представляющих ключевые слова.*/
    std::vector<size_t> del_repres; /* Вектор из индексов строковых литералов,
                                     * представляющих разделители. */


    Command_buffer id_begin, id_body;
    Command_buffer numbers_regexp, strings_regexp;
//
//     enum Used_automaton{
//         Start_aut,     Unknown_aut,   Id_aut,     Keyword_aut,
//         IdKeyword_aut, Delimiter_aut, Number_aut, String_aut,
//         Comment_aut
//     };/* Данное перечисление состоит из имён порождаемых автоматов. */
//
//     struct Automaton_with_procs{
//         std::string name;
//         std::string proc_proto;
//         std::string fin_proc_proto;
//         std::string proc_ptr;
//         std::string fin_proc_ptr;
//
//         Automaton_with_procs()                            = default;
//         ~Automaton_with_procs()                           = default;
//         Automaton_with_procs(const Automaton_with_procs&) = default;
//     };

    std::vector<Automaton_with_procs> automaton_info;
    Category begin_chars[6];

    uint64_t       set_of_used_automata;
    size_t         mark_of_single_lined;
    size_t         mark_of_multilined_begin;
    size_t         mark_of_multilined_end;
    bool           multilined_is_nested;

    Trie_for_set_of_char char_cat;

    std::map<Used_automaton, std::string> aut_impl;
    std::map<Used_automaton, std::string> aut_impl_fin_proc;

    std::map<size_t, std::string> category_name;
    size_t last_category_suffix;

    std::vector<std::u32string> delimiter_strings;
    std::vector<std::u32string> keyword_strings;

    std::string get_act_repres(size_t i);
    std::pair<bool, std::string> add_category(const std::set<char32_t>& s,
                                              const std::string& default_name);

    bool there_is_Elem_definition;

    size_t write_action_name_idx;

//     std::string start_proc_impl;

    std::string fields_for_comments_handling;

private:
    void init_end(Location_ptr loc);
};
#endif
