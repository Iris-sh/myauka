/*
    File:    get_act_repres.h
    Created: 04 February 2017 at 18:29 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef GET_ACT_REPRES_H
#define GET_ACT_REPRES_H
#include "../include/info_for_constructing.h"
#include <cstddef>
#include <cstdint>
#include <string>
/* Данная функция на вход получает индекс имени действия, i, в префиксном дереве
 * идентификаторов, а в качестве результата выдаёт строковое представление
 * определения действия. */
std::string get_act_repres(Info_for_constructing& info, size_t i);
#endif