/*
    File:    lexem_info_name_parser.h
    Created: 24 June 2017 at 11:54 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef LEXEM_INFO_NAME_PARSER_H
#define LEXEM_INFO_NAME_PARSER_H

#include <memory>
#include "../include/errors_and_tries.h"
#include "../include/main_scaner.h"

class Lexem_info_name_parser{
public:
    Lexem_info_name_parser()                              = default;
    Lexem_info_name_parser(Errors_and_tries              et,
                           std::shared_ptr<Main_scaner>& msc) :
        et_(et), msc_(msc) { };
    Lexem_info_name_parser(const Lexem_info_name_parser&) = default;

    ~Lexem_info_name_parser()                             = default;
    size_t compile();
private:
    Main_lexem_info                  li;
    Main_lexem_code                  lc;

    Errors_and_tries                 et_;
    std::shared_ptr<Main_scaner>     msc_;
};
#endif