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
/**
 *  \param [in] info information about constructing scaner
 *  \param [in] i    index of action name in the prefix tree of identifiers
 *
 *  \return          string representation of action
 */
std::string get_act_repres(Info_for_constructing& info, size_t i);
#endif