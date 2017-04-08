/*
    File:    str_repres_for_set_of_size_t_const.cpp
    Created: 04 February 2017 at 18:54 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/str_repres_for_set_of_size_t_const.h"
#include "../include/list_to_columns.h"
#include "../include/indent.h"
#include <vector>
std::string str_repres_for_set_of_size_t_const(const std::set<size_t>& s,
                                               const std::string& const_name){
    std::string result;
    result = "static const std::set<size_t> " + const_name + " = {\n";

    std::vector<std::string> elems;
    for(auto x : s){
        elems.push_back(std::to_string(x));
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 8;
    f.spaces_between_columns = 1;

    result += string_list_to_columns(elems, f) +"\n};";
    return result;
}