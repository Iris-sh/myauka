/*
    File:    main_scaner.h
    Created: 14 December 2015 at 15:25 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef MAIN_SCANER_H
#define MAIN_SCANER_H

#include "../include/abstract_scaner.h"
#include "../include/error_count.h"
#include <string>
// #include "../include/trie.h"

enum Main_lexem_code : uint16_t {
    None,                  Unknown,             Id,
    Kw_action,             Kw_class_members,    Kw_codes,
    Kw_codes_type,         Kw_comments,         Kw_delimiters,
    Kw_ident_name,         Kw_idents,           Kw_keywords,
    Kw_multilined,         Kw_nested,           Kw_newline_is_lexem,
    Kw_numbers,            Kw_scaner_name,      Kw_single_lined,
    Kw_strings,            Kw_token_fields,     Kw_header_additions,
    Kw_impl_additions,     Kw_lexem_info_name,  Comma,
    Colon,                 Opened_fig_brack,    Closed_fig_brack,
    String,                M_Kw_action,         M_Kw_class_members,
    M_Kw_codes,            M_Kw_codes_type,     M_Kw_comments,
    M_Kw_delimiters,       M_Kw_ident_name,     M_Kw_idents,
    M_Kw_keywords,         M_Kw_multilined,     M_Kw_nested,
    M_Kw_newline_is_lexem, M_Kw_numbers,        M_Kw_scaner_name,
    M_Kw_single_lined,     M_Kw_strings,        M_Kw_token_fields,
    M_Kw_header_additions, M_Kw_impl_additions, M_Kw_lexem_info_name
};

struct Main_lexem_info{
    Main_lexem_code code;
    union{
        size_t ident_index;
        size_t string_index;
    };
};

class Main_scaner : public Scaner<Main_lexem_info>{
public:
    Main_scaner() = default;
    Main_scaner(Location_ptr location, const Errors_and_tries& et) :
        Scaner<Main_lexem_info>(location, et) {};
    Main_scaner(const Main_scaner& orig) = default;
    virtual ~Main_scaner() = default;
    virtual Main_lexem_info current_lexem();
private:
    enum Category : unsigned short {
        Spaces,        Other,       Percent,
        After_percent, Id_begin,    Id_body,
        Delimiters,    Double_quote
    };

    enum Automaton_name{
        A_start,   A_unknown,   A_id,
        A_keyword, A_delimiter, A_string
    };
    Automaton_name automaton; /* текущий автомат */

    typedef bool (Main_scaner::*Automaton_proc)();
    /* Это тип указателя на функцию--член, реализующую
     * конечный автомат, обрабатывающий лексему. Функция
     * эта должна возвращать true, если лексема ещё не
     * разобрана до конца, и false --- в противном случае. */

    typedef void (Main_scaner::*Final_proc)();
    /* А это -- тип указателя на функцию-член, выполняющую
     * необходимые действия в случае неожиданного окончания
     * лексемы. */

    static Automaton_proc procs[];
    static Final_proc     finals[];
    /* функции обработки лексем: */
    bool start_proc();     bool unknown_proc();
    bool id_proc();        bool keyword_proc();
    bool delimiter_proc(); bool string_proc();
    /* функкции для выполнения действий в случае неожиданного
     * окончания лексемы */
    void none_final_proc();      void unknown_final_proc();
    void id_final_proc();        void keyword_final_proc();
    void delimiter_final_proc(); void string_final_proc();
    /* Функция, корректирующая код лексемы, скорее всего, являющейся
     * ключевым словом, и выводящая нужную диагностику. */
    void correct_keyword();
    /* Следующая функция по своему аргументу, символу типа char32_t,
       выдаёт множество категорий символов, которым (категориям)
       он принадлежит. */
    uint64_t get_categories_set(char32_t c);
};
#endif