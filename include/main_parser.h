/*
    File:    main_parser.h
    Created: 14 декабря 2015г. в 15:25 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef MAIN_PARSER_H
#define MAIN_PARSER_H

#include "../include/location.h"
#include <memory>
#include <string>
#include "../include/main_parser_data.h"
#include "../include/attributed_char_trie.h"
#include <set>
#include "../include/categories.h"
#include "../include/dfa.h"
#include "../include/groupped_dfa.h"
#include "../include/init_and_final_acts.h"

class Main_parser : public Main_parser_data {
public:
    Main_parser() = default;
    Main_parser(Location_ptr loc) : Main_parser_data(loc) {};
    Main_parser(const Main_parser& orig) = default;
    ~Main_parser() = default;

    void compile();

    int get_number_of_errors();
private:
    Main_lexem_info li;
    Main_lexem_code lc;

    Init_and_final_acts acts_for_strings;
    Init_and_final_acts acts_for_numbers;

    enum Name_sec {
        Scaner_name_sec, Lexem_code_name_sec, Id_name_sec
    };

    void parse();

    /* Следующие функции обрабатывают отдельные разделы входного текста. При этом,
     * поскольку секции %scaner_name, %codes_type, и %ident_name имеют идентичную
     * структуру, то для обработки этих трёх секций можно обойтись одной функцией,
     * функцией name_section_handling, принимающей имя секции и место, в которое нужно
     * записать индекс соответствующего идентификатора в префиксном дереве
     * идентификаторов. */
    void name_section_handling(size_t& ind, Name_sec section);
    void set_default_name(size_t& ind, Name_sec section);

    bool id_is_defined(size_t idx);
    /* Данная функция проверяет, не определён ли уже идентификатор, индекс которого
     * в префиксном дереве идентификаторов равен idx. Если определён, то выводится
     * диагностика и возвращается true. Иначе ничего возвращается false и никакой
     * диагностики не выводится. */

    void token_fields_sec();
    void class_members_sec();
    void idents_sec();
    void comments_sec();

    unsigned   state;
    typedef bool (Main_parser::*State_proc)();

    /* Функция, проверяющая корректность описания сканера. Если
     * описание корректно --- возвращается true, иначе --- false. Кроме
     * того, данная функция выводит необходимую диагностику. */
    bool verify();

    size_t keyword_postaction, del_postaction;

    size_t current_action_name_idx;

    std::string generate_automata_enum();
    std::string generate_automata_proc_protos();
    std::string generate_automata_final_procs_protos();

    void generate_scaner_header();
    void generate_scaner_implementation();

    std::string generate_lexem_codes_enum();
    std::string generate_lexem_info();
    std::string generate_scaner_class();

    void prepare_automata_info();

    std::string procs_tables();
    std::string automata_table();
    std::string final_table();

    void generate_automata_impl();

    void generate_delim_automaton_impl();
    void generate_strings_automaton_impl();
    void generate_numbers_automaton_impl();
    void generate_idents_and_keywords_automata_impl();
    void generate_unknown_automata_impl();

    std::string generate_category_table();
    std::string collect_automata_impls();

    std::string delim_table_gen(const Jumps_and_inits& ji,
                                const std::string&     table_name);
    std::string jump_table_string_repres(const Jumps_and_inits& ji,
                                         const std::string&     table_name,
                                         const std::string&     init_table_name);

    void generate_separate_keywords_automat();
    void generate_separate_identifier_automat();

    struct Str_data_for_automaton {
        std::string automata_name;
        std::string proc_name;
        std::string category_name_prefix;
        std::string diagnostic_msg;
        std::string final_actions;
        std::string final_states_set_name;
    };

    std::string automata_repres(const G_DFA& buf,
                                const Str_data_for_automaton& f);


    std::string automata_repres_switch(const G_DFA& buf, const Str_data_for_automaton& f);

    std::string check_there_is_jump(const Str_data_for_automaton& f);

    std::string automata_repres_case(const G_DFA_state_jumps& m,
                                     const Str_data_for_automaton& f,
                                     size_t counter);
    std::string automata_repres_case_j(const Category&              cat,
                                       const DFA_state_with_action& swa,
                                       const  Str_data_for_automaton& f); //,
                                       // size_t& cat_counter);

    void generate_idkeyword_automat();

    void add_fictive_delimiter(const std::u32string& dcode,  size_t drepres_idx);

    void add_new_lexem_code(size_t idx);
    void add_new_string(const size_t idx, const size_t code_);
    void add_fictive_delimiters();
    /*  Если есть описание комментариев, то
        добавляет фиктивный разделитель. А именно, если описан однострочный
        комментарий, то добавляется фиктивный разделитель
            SINGLE_LINED_COMMENT_MARK
        а если присутствует описание многострочного комментария --- то
        фиктивный разделитель
            MULTILINED_COMMENT_MARK */

    std::string generate_current_lexem_proc();

    std::string current_lexem_without_comments();

    std::string there_is_only_singlelined();

    std::string there_is_only_multilined();

    std::string there_are_all_comments();

    std::string omit_multilined_comment_proc();

    std::string omit_all_comment_proc();

    std::string omit_nested_multilined();
    std::string omit_not_nested_multilined();
};
#endif