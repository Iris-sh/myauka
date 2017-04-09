/*
    File:    comments_parser.h
    Created: 06 November 2016 at 12:20 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef COMMENTS_PARSER_H
#define COMMENTS_PARSER_H

#include "../include/errors_and_tries.h"
#include "../include/main_scaner.h"
#include <memory>
#include <tuple>

using Comments_parser_result = std::tuple<size_t, size_t, size_t, bool>;
/*
 * First component:  mark of single lined comment.
 * Second component: mark of multilined comment begin.
 * Third component:  mark of multilined comment end.
 * Fourth component: can multilined comment be nested?
 */

class Comments_parser {
public:
    Comments_parser()                             = default;
    Comments_parser(const Comments_parser& orig ) = default;

    Comments_parser(Errors_and_tries et, std::shared_ptr<Main_scaner>& msc_) :
        et_(et), msc(msc_) {};

    ~Comments_parser()                            = default;

    Comments_parser_result compile();
private:
    void compile_();

    size_t                           mark_of_single_lined;
    size_t                           mark_of_multilined_begin;
    size_t                           mark_of_multilined_end;
    bool                             multilined_is_nested;

    Main_lexem_info                  li;
    Main_lexem_code                  lc;

    Errors_and_tries                 et_;
    std::shared_ptr<Main_scaner>     msc;

    typedef bool (Comments_parser::*State_proc)();
    /* Section of comments description can be written as a regular expression
                     a((bc)?(de?cfc)|bc)
        where
             a    is the keyword %comments
             b    is the keyword %single_lined
             c    string literal
             d    is the keyword %multilined
             e    is the keyword %nested
             f    : (that is colon)

        If for this regular expression we first construct NDFA, then DFA,
        and minimize the latter, we obtain the following transition table:

        |-----------|---|---|---|---|---|---|---------------------|
        |   state   | a | b | c | d | e | f | remark              |
        |-----------|---|---|---|---|---|---|---------------------|
        |     A     | B |   |   |   |   |   | begin state         |
        |-----------|---|---|---|---|---|---|---------------------|
        |     B     |   | C |   | D |   |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     C     |   |   | E |   |   |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     D     |   |   | F |   | G |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     E     |   |   |   | D |   |   | final state         |
        |-----------|---|---|---|---|---|---|---------------------|
        |     F     |   |   |   |   |   | H |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     G     |   |   | F |   |   |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     H     |   |   | I |   |   |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     I     |   |   |   |   |   |   | final state         |
        |-----------|---|---|---|---|---|---|---------------------|

        However, such state names are inconvenient to use in the code, so
        more meaningful names are needed, which are collected in the State
        enumeration. Below is a table of the correspondence of state names
        from the table of transitions and names from this enumeration.

        |-----------|---------------------------------------------|
        |   state   |         Name from the enum State            |
        |-----------|---------------------------------------------|
        |     A     | Comments_kw                                 |
        |-----------|---------------------------------------------|
        |     B     | Single_or_multiLined                        |
        |-----------|---------------------------------------------|
        |     C     | Single_lined                                |
        |-----------|---------------------------------------------|
        |     D     | Multilined                                  |
        |-----------|---------------------------------------------|
        |     E     | Single_lined_mark                           |
        |-----------|---------------------------------------------|
        |     F     | Multi_lined_begin                           |
        |-----------|---------------------------------------------|
        |     G     | Nested                                      |
        |-----------|---------------------------------------------|
        |     H     | Beg_end_delim                               |
        |-----------|---------------------------------------------|
        |     I     | Multilined_end                              |
        |-----------|---------------------------------------------|

    */

    enum State {
        Comments_kw, Single_or_multiLined, Single_lined,
        Multilined,  Single_lined_mark,    Multi_lined_begin,
        Nested,      Beg_end_delim,        Multilined_end
    };

    State state;

    static State_proc procs[];

    bool comments_kw_proc();
    bool single_or_multilined_proc();
    bool single_lined_proc();
    bool multilined_proc();
    bool single_lined_mark_proc();
    bool multi_lined_begin_proc();
    bool nested_proc();
    bool beg_end_delim_proc();
    bool multilined_end_proc();
};
#endif