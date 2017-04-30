/*
    File:    add_category.cpp
    Created: 04 February 2017 at 13:10 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/add_category.h"
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
                                          const std::string&        default_name)
{
    std::string result_str;
    bool        result_bool;

    size_t i = info.char_cat.insertSet(s);
    auto it  = info.category_name.find(i);
    if(it != info.category_name.end()){
        result_str            = it->second;
        result_bool           = false;
    }else{
        info.category_name[i] = default_name;
        result_str            = default_name;
        result_bool           = true;
    }

    auto result = std::make_pair(result_bool, result_str);
    return result;
}