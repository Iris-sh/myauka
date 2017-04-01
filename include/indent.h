/*
    File:    indent.h
    Created: 04 февраля 2017г. в 10:32 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef INDENT_H
#define INDENT_H
#include <string>
#define INDENT_WIDTH           4
#define DOUBLE_INDENT_WIDTH    ((INDENT_WIDTH) * 2)
#define TRIPLE_INDENT_WIDTH    ((INDENT_WIDTH) * 3)
#define QUADRUPLE_INDENT_WIDTH ((INDENT_WIDTH) * 4)
extern const std::string indent;
extern const std::string double_indent;
extern const std::string triple_indent;
extern const std::string quadruple_indent;
#endif