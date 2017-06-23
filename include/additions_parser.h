/*
    File:    additions_parser.h
    Created: 08 May 2017 at 18:22 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef ADDITIONS_PARSER_H
#define ADDITIONS_PARSER_H

#include <memory>
#include "../include/errors_and_tries.h"
#include "../include/main_scaner.h"

enum class Header_or_impl{
    Header, Impl
};

class Additions_parser{
public:
    Additions_parser()                        = default;
    Additions_parser(Errors_and_tries              et,
                     std::shared_ptr<Main_scaner>& msc) :
        et_(et), msc_(msc) { };
    Additions_parser(const Additions_parser&) = default;
    ~Additions_parser()                       = default;

    size_t compile(const Header_or_impl hi);
private:
    Main_lexem_info                  li;
    Main_lexem_code                  lc;

    Errors_and_tries                 et_;
    std::shared_ptr<Main_scaner>     msc_;
};
#endif