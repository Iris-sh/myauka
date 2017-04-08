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
/*
 * Insert a character category with the name default_name into the associative array
 * std::map<size_t, std::string> category_name. A character category is a set of
 * characters. If such set did't belong to the set, then the function inserts
 * the category with the name default_name and returns the pair <true, default_name>;
 * else the function returns the pair <false, the current name of the set>.
*/
std::pair<bool, std::string> add_category(Info_for_constructing&    info,
                                          const std::set<char32_t>& s,
                                          const std::string&        default_name);
#endif