/*
    File:    codes_and_newline.h
    Created: 10 августа 2016г. в 21:43 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef CODES_AND_NEWLINE_H
#define CODES_AND_NEWLINE_H

#include <memory>
#include <vector>
#include "../include/scope.h"
#include "../include/errors_and_tries.h"
#include "../include/main_scaner.h"

class Codes_and_newline{
public:
    Codes_and_newline() = default;
    Codes_and_newline(std::shared_ptr<Scope> scope, Errors_and_tries et,
                 std::shared_ptr<Main_scaner>& msc_) :
        scope_(scope), et_(et), msc(msc_) {
            codes = std::vector<size_t>();
            state = 0;
        };
    Codes_and_newline(const Codes_and_newline& orig) = default;
    ~Codes_and_newline() = default;

    void codes_sec(std::vector<size_t>& codes_, size_t& last_code);
    void newline_is_lexem_sec();

private:
    Main_lexem_info li;
    Main_lexem_code lc;

    std::shared_ptr<Scope>       scope_;
    Errors_and_tries             et_;
    std::shared_ptr<Main_scaner> msc;

    std::vector<size_t>          codes;

    size_t                       last_code_val;

    void codes_sec_();

    unsigned   state;
    typedef bool (Codes_and_newline::*State_proc)();
    static State_proc procs[];
    enum State{
        Codes_id, Codes_comma_sep
    };

    bool codes_id_proc(); bool codes_comma_sep_proc();

    /* Следующая функция проверяет идентификатор из перечисления с кодами
     * лексем на существование. Если такой идентификатор уже есть, то
     * выводится диагностика. Иначе идентификатор добавляется и в
     * вектор codes, и в ассоциативный массив идентификаторов, idsc. */
    void add_new_lexem_code(size_t idx);
};
#endif