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
// struct Comments_parser_result {
//     size_t mark_of_single_lined;
//     size_t mark_of_multilined_begin;
//     size_t mark_of_multilined_end;
//     bool   multilined_is_nested;
// };

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
    /* Секцию описания комментариев можно описать регулярным выражением
                     a((bc)?(de?cfc)|bc)
        где введены следующие обозначения:
             a    ключевое слово %comments
             b    ключевое слово %single_lined
             c    строковый литерал
             d    ключевое слово %multilined
             e    ключевое слово %nested
             f    : (то есть двоеточие)

        Построив для этого регулярного выражения сначала НКА, затем по этому НКА построив
        ДКА, и минимизировав последний, получим следующую таблицу переходов:

        |-----------|---|---|---|---|---|---|---------------------|
        | Состояние | a | b | c | d | e | f | Примечание          |
        |-----------|---|---|---|---|---|---|---------------------|
        |     A     | B |   |   |   |   |   | начальное состояние |
        |-----------|---|---|---|---|---|---|---------------------|
        |     B     |   | C |   | D |   |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     C     |   |   | E |   |   |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     D     |   |   | F |   | G |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     E     |   |   |   | D |   |   | конечное состояние  |
        |-----------|---|---|---|---|---|---|---------------------|
        |     F     |   |   |   |   |   | H |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     G     |   |   | F |   |   |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     H     |   |   | I |   |   |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     I     |   |   |   |   |   |   | конечное состояние  |
        |-----------|---|---|---|---|---|---|---------------------|

        Однако такие имена состояний неудобно использовать в коде, поэтому нужны более
        осмысленные имена, которые и собраны в перечислении State. Ниже приведена таблица
        соответствия имён состояний из таблицы переходов и имён из данного перечисления.

        |-----------|---------------------------------------------|
        | Состояние |     Имя из перечисления Comment_state       |
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