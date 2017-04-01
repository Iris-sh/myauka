/*
    File:    num_str_parser.h
    Created: 06 ноября 2016г. в 12:20 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef NUM_STR_PARSER_H
#define NUM_STR_PARSER_H

#include <memory>
#include <vector>
#include <string>
#include "../include/scope.h"
#include "../include/errors_and_tries.h"
#include "../include/main_scaner.h"
#include "../include/command.h"
#include "../include/act_expr_parser.h"
#include "../include/init_and_final_acts.h"
#include <utility>

using NS_settings = std::pair<Main_lexem_code, Number_or_string>;

class NS_parser {
public:
    NS_parser()                      = default;
    NS_parser(std::shared_ptr<Scope>           scope,
              Errors_and_tries                 et,
              std::shared_ptr<Main_scaner>&    msc_,
              std::shared_ptr<Act_expr_parser> a_parser) :
        scope_(scope), et_(et), msc(msc_), a_parser_(a_parser) { };

    NS_parser(const NS_parser& orig) = default;
    virtual ~NS_parser()             = default;

    void compile(Command_buffer& buf, Init_and_final_acts& acts);
protected:
    virtual NS_settings settings() = 0;
private:
    Main_lexem_info                  li;
    Main_lexem_code                  lc;

    std::shared_ptr<Scope>           scope_;
    Errors_and_tries                 et_;
    std::shared_ptr<Main_scaner>     msc;
    std::shared_ptr<Act_expr_parser> a_parser_;

    /* В следующей переменной содержится либо код ключевого слова
     * %numbers, либо код ключевого слова %strings. */
    Main_lexem_code                  sec_begin;

    Number_or_string                 kind;

    enum State {
        Num_str_kw, Maybe_init_acts, Init_acts,    Maybe_final_acts,
        Final_acts, Action_sec,      Act_expr_beg, Act_def
    };

    State                            state;

    /*
       Начало секций %numbers и %strings выглядит одинаково, и это начало можно записать
       следующим регулярным выражением:
                                           ab?eb?(cdb)*f
       где введены следующие обозначения:
              a  ключевое слово %numbers или %strings (в зависимости от вида секции)
              b  строковый литерал
              c  ключевое слово %action
              d  идентификатор, являющийся именем действия
              e  двоеточие
              f  открывающая фигурная скобка

        Построив для этого регулярного выражения сначала НКА, затем по этому НКА построив
        ДКА, и минимизировав последний, получим следующую таблицу переходов:

        |-----------|---|---|---|---|---|---|---------------------|
        | Состояние | a | b | c | d | e | f | Примечание          |
        |-----------|---|---|---|---|---|---|---------------------|
        |     A     | B |   |   |   |   |   | начальное состояние |
        |-----------|---|---|---|---|---|---|---------------------|
        |     B     |   | C |   |   | D |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     C     |   |   |   |   | D |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     D     |   | E | F |   |   | G |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     E     |   |   | F |   |   | G |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     F     |   |   |   | H |   |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|
        |     G     |   |   |   |   |   |   | конечное состояние  |
        |-----------|---|---|---|---|---|---|---------------------|
        |     H     |   | E |   |   |   |   |                     |
        |-----------|---|---|---|---|---|---|---------------------|

        Однако такие имена состояний неудобно использовать в коде, поэтому нужны более
        осмысленные имена, которые и собраны в перечислении State. Ниже приведена
        таблица соответствия имён состояний из таблицы переходов и имён из данного
        перечисления.

        |-----------|---------------------------------------------|
        | Состояние |     Имя из перечисления Num_str_state       |
        |-----------|---------------------------------------------|
        |     A     | Num_str_kw                                  |
        |-----------|---------------------------------------------|
        |     B     | Maybe_init_acts                             |
        |-----------|---------------------------------------------|
        |     C     | Init_acts                                   |
        |-----------|---------------------------------------------|
        |     D     | Maybe_final_acts                            |
        |-----------|---------------------------------------------|
        |     E     | Final_acts                                  |
        |-----------|---------------------------------------------|
        |     F     | Action_sec                                  |
        |-----------|---------------------------------------------|
        |     G     | Act_expr_beg                                |
        |-----------|---------------------------------------------|
        |     H     | Act_def                                     |
        |-----------|---------------------------------------------|

     */
    Command_buffer      buf_;
    Init_and_final_acts acts_;
    /* Следующая функция реализует вышеупомянутый автомат, и возвращает true,
     * если текущий раздел --- тот, который предполагалось, и false в
     * противном случае. */
    bool begin_of_num_or_str_sec();


    void add_action_definition(size_t action_id, size_t action_def_idx);

    /* Ниже приводится таблица функций, реализующих выполнение действий в
       каждом из состояний автомата, который реализует функция
       begin_of_num_or_str_sec */
    typedef bool (NS_parser::*State_proc)();

    static State_proc procs[];

    bool num_str_kw_proc();    bool maybe_init_acts_proc();
    bool init_acts_proc();     bool maybe_final_acts_proc();
    bool final_acts_proc();    bool action_sec_proc();
    bool act_expr_beg_proc();  bool act_def_proc();

    size_t current_action_name_idx = 0;
};

class String_parser : public NS_parser {
public:
    String_parser()                           = default;
    String_parser(std::shared_ptr<Scope>           scope,
                  Errors_and_tries                 et,
                  std::shared_ptr<Main_scaner>&    msc_,
                  std::shared_ptr<Act_expr_parser> a_parser) :
        NS_parser(scope, et, msc_, a_parser) { };

    String_parser(const String_parser& orig)  = default;
    virtual ~String_parser()                  = default;
protected:
    virtual NS_settings settings();
};

class Number_parser : public NS_parser {
public:
    Number_parser()                           = default;
    Number_parser(std::shared_ptr<Scope>           scope,
                  Errors_and_tries                 et,
                  std::shared_ptr<Main_scaner>&    msc_,
                  std::shared_ptr<Act_expr_parser> a_parser) :
        NS_parser(scope, et, msc_, a_parser) { };

    Number_parser(const Number_parser& orig)  = default;
    virtual ~Number_parser()                  = default;
protected:
    virtual NS_settings settings();
};
#endif