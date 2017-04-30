/*
    File:    add_category.h
    Created: 04 February 2017 at 13:01 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef ADD_CATEGORY_H
#define ADD_CATEGORY_H
#include <set>
#include <utility>
#include <string>
#include "../include/info_for_constructing.h"
/**
 * \brief   Adds a category (set) of characters named default_name to the associative
 *          array std :: map < size_t, std :: string > category_name.
 * \details Adds a category (set) of characters named default_name to the associative
 *          array std :: map < size_t, std :: string > category_name. If there is not
 *          such a set, then add it with that name, and return it and true. If there
 *          is, then return the existing name and false.
 * \param [in,out] info         Information for a scaner class constructing.
 * \param [in]     s            Added category.
 * \param [in]     default_name Default name.
 * \return                      Pair (flag, value). Here, flag is true if there is no
 *                              category with that name yet, and false otherwise. In
 *                              addition, if there is no category with this name, value
 *                              is default_name, otherwise it equals the existing
 *                              category name.
 */
std::pair<bool, std::string> add_category(Info_for_constructing&    info,
                                          const std::set<char32_t>& s,
                                          const std::string&        default_name);
#endif