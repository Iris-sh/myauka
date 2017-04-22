/*
    File:    keyword_delimiter_parser.h
    Created: 12 August 2016 at 09:41 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef KEYWORD_DELIMITER_PARSER_H
#define KEYWORD_DELIMITER_PARSER_H

#include <memory>
#include <vector>
#include <string>
#include "../include/scope.h"
#include "../include/errors_and_tries.h"
#include "../include/main_scaner.h"

using Settings = std::tuple<std::string, Str_kind, Main_lexem_code>;

class KW_parser {
public:
    KW_parser()                      = default;
    KW_parser(std::shared_ptr<Scope> scope, Errors_and_tries et,
              std::shared_ptr<Main_scaner>& msc_) :
        scope_(scope), et_(et), msc(msc_) {
            state                    = 0;
            repres                   = std::vector<size_t>();
            codes                    = std::vector<size_t>();
            error_message_format_str = std::string();
        };
    KW_parser(const KW_parser& orig) = default;
    virtual ~KW_parser()             = default;

    size_t compile(std::vector<size_t>& repres_, std::vector<size_t>& codes_,
                   size_t& last_code); //< return value is index of final action

/**
 *  \param [in] idx     index of string literal that represents delimiter/keyword
 *                      (index in prefix tree of string literals)
 *
 *  \param [in] code_   index of identifier, which is corresponding lexem code
 *                      (index in prefix tree of identifiers)
 * */
    void add_new_string(const size_t idx, const size_t code_);
protected:
    virtual Settings settings() = 0;
private:
    std::string                  error_message_format_str;
    Str_kind                     k;
    Main_lexem_code              checked_code;

    Main_lexem_info              li;
    Main_lexem_code              lc;

    std::shared_ptr<Scope>       scope_;
    Errors_and_tries             et_;
    std::shared_ptr<Main_scaner> msc;

    unsigned                     state = 0;

    std::vector<size_t>          repres;
    std::vector<size_t>          codes;
    size_t                       last_code_val;

    size_t                       maybe_repres_str_idx;
    size_t                       idx;
    size_t                       postaction;

    typedef bool (KW_parser::*State_proc)();


    static State_proc procs[];
    enum State{
        Maybe_repres_str, Colon_sep0, Maybe_code_kw_or_del, Repres_str,
        Code_kw_or_del,   Colon_sep,  Kw_del_comma_sep
    };

    bool maybe_repres_str_proc();     bool colon_sep0_proc();
    bool maybe_code_kw_or_del_proc(); bool repres_str_proc();
    bool code_kw_or_del_proc();       bool colon_sep_proc();
    bool kw_del_comma_sep_proc();

    void compile_();

    /* Bodies of sections of definition of keywords and delimiters can be described as
     * the following regular expression:
             (ab)?abc(dabc)*
       here
            a is string literal, which is either representation of keyword, or
              representation of delimiter, or final action
            b is colon
            c is identifier, which is corresponding lexem code
            d is comma
       Constructing from this expression a DFA with the minimum possible number
       of states, we obtain the following transition table:

       |-----------|---|---|---|---|---------------------|
       |   state   | a | b | c | d | Remark              |
       |-----------|---|---|---|---|---------------------|
       |     A     | B |   |   |   | begin state         |
       |-----------|---|---|---|---|---------------------|
       |     B     |   | C |   |   |                     |
       |-----------|---|---|---|---|---------------------|
       |     C     | D |   | E |   |                     |
       |-----------|---|---|---|---|---------------------|
       |     D     |   | F |   |   |                     |
       |-----------|---|---|---|---|---------------------|
       |     E     |   |   |   | G | final state         |
       |-----------|---|---|---|---|---------------------|
       |     F     |   |   | E |   |                     |
       |-----------|---|---|---|---|---------------------|
       |     G     | D |   |   |   |                     |
       |-----------|---|---|---|---|---------------------|

       However, such state names are inconvenient to use in the code, so you need more
       meaningful names, which are collected in the State enum. Below is a table of the
       correspondence of state names from the table of transitions and names from
       this enumeration.

       |-----------|---------------------------------|
       |   state   |      Name from enum State       |
       |-----------|---------------------------------|
       |     A     | Maybe_repres_str                |
       |-----------|---------------------------------|
       |     B     | Colon_sep0                      |
       |-----------|---------------------------------|
       |     C     | Maybe_code_kw_or_del            |
       |-----------|---------------------------------|
       |     D     | Repres_str                      |
       |-----------|---------------------------------|
       |     E     | Code_kw_or_del                  |
       |-----------|---------------------------------|
       |     F     | Colon_sep                       |
       |-----------|---------------------------------|
       |     G     | Kw_del_comma_sep                |
       |-----------|---------------------------------|

    */
};

class Keyword_parser : public KW_parser {
public:
    Keyword_parser()                           = default;
    Keyword_parser(std::shared_ptr<Scope> scope, Errors_and_tries et,
                   std::shared_ptr<Main_scaner>& msc_) : KW_parser(scope, et, msc_) { };

    Keyword_parser(const Keyword_parser& orig) = default;
    virtual ~Keyword_parser()                  = default;
protected:
    virtual Settings settings();
};

class Delimiter_parser : public KW_parser {
public:
    Delimiter_parser()                             = default;
    Delimiter_parser(std::shared_ptr<Scope> scope, Errors_and_tries et,
                     std::shared_ptr<Main_scaner>& msc_) : KW_parser(scope, et, msc_) { };

    Delimiter_parser(const Delimiter_parser& orig) = default;
    virtual ~Delimiter_parser()                    = default;
protected:
    virtual Settings settings();
};
#endif