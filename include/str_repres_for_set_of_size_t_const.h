/*
    File:    str_repres_for_set_of_size_t_const.h
    Created: 04 February 2017г. в 18:50 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef STR_REPRES_FOR_SET_OF_SIZE_T_CONST_H
#define STR_REPRES_FOR_SET_OF_SIZE_T_CONST_H
#include <string>
#include <set>
#include <cstddef>
std::string str_repres_for_set_of_size_t_const(const std::set<size_t>& s,
                                               const std::string& const_name);
#endif