/*
    Файл:    indent.h
    Создан:  04 февраля 2017г. в 10:32 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
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
extern const std::string indent;           //= std::string(INDENT_WIDTH,           ' ');
extern const std::string double_indent;    //= std::string(DOUBLE_INDENT_WIDTH,    ' ');
extern const std::string triple_indent;    //= std::string(TRIPLE_INDENT_WIDTH,    ' ');
extern const std::string quadruple_indent; //= std::string(QUADRUPLE_INDENT_WIDTH, ' ');
#endif