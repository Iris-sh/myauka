/*
    File:    keyword_delimiter_parser.h
    Created: 12 августа 2016г. в 09:41 (по Москве)
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
                   size_t& last_code); // возвращается индекс действия по завершении

/* Первый аргумент данной функции --- индекс строкового литерала, представляющего
 * разделитель/ключевое слово, в префиксном дереве строковых литералов, а второй
 * аргумент --- индекс идентификатора, являющегося соответствующим кодом лексемы,
 * в префиксном дереве идентификаторов. */
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

    /* Тела секций определения ключевых слов и разделителей можно описать следующим
       регулярным выражением:
             (ab)?abc(dabc)*
       где введены обозначения
            a строковый литерал, являющийся либо представлением ключевого слова или
              разделителя, либо действием по завершении
            b двоеточие
            c идентификатор, являющийся соответствующим кодом лексемы
            d запятая
       Построив по этому регулярному выражению ДКА с минимально возможным количеством
       состояний, получим такую таблицу переходов:

       |-----------|---|---|---|---|---------------------|
       | Состояние | a | b | c | d | Примечание          |
       |-----------|---|---|---|---|---------------------|
       |     A     | B |   |   |   | начальное состояние |
       |-----------|---|---|---|---|---------------------|
       |     B     |   | C |   |   |                     |
       |-----------|---|---|---|---|---------------------|
       |     C     | D |   | E |   |                     |
       |-----------|---|---|---|---|---------------------|
       |     D     |   | F |   |   |                     |
       |-----------|---|---|---|---|---------------------|
       |     E     |   |   |   | G | конечное состояние  |
       |-----------|---|---|---|---|---------------------|
       |     F     |   |   | E |   |                     |
       |-----------|---|---|---|---|---------------------|
       |     G     | D |   |   |   |                     |
       |-----------|---|---|---|---|---------------------|

       Однако такие имена состояний неудобно использовать в коде, поэтому нужны более
       осмысленные имена, которые и собраны в перечислении State.Ниже приведена таблица
       соответствия имён состояний из таблицы переходов и имён из данного перечисления.

       |-----------|-----------------------------------------------------------------|
       | Состояние |     Имя из перечисления Keywords_and_delimiters_sec_state       |
       |-----------|-----------------------------------------------------------------|
       |     A     | Maybe_repres_str                                                |
       |-----------|-----------------------------------------------------------------|
       |     B     | Colon_sep0                                                      |
       |-----------|-----------------------------------------------------------------|
       |     C     | Maybe_code_kw_or_del                                            |
       |-----------|-----------------------------------------------------------------|
       |     D     | Repres_str                                                      |
       |-----------|-----------------------------------------------------------------|
       |     E     | Code_kw_or_del                                                  |
       |-----------|-----------------------------------------------------------------|
       |     F     | Colon_sep                                                       |
       |-----------|-----------------------------------------------------------------|
       |     G     | Kw_del_comma_sep                                                |
       |-----------|-----------------------------------------------------------------|

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