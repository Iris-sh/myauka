/*
    Файл:    conv_case.h
    Создан:  02 февраля 2017г. в 11:29 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

/* Функции для преобразования регистра латинских букв. */
#ifndef CONV_CASE_H
#define CONV_CASE_H
#include <string>
std::string tolower_case(const std::string& s);
std::string toupper_case(const std::string& s);
#endif