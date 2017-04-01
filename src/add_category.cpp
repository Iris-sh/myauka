/*
    File:    add_category.cpp
    Created: 04 февраля 2017г. в 13:10 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/add_category.h"
/*
 * Добавляет в ассоциативный массив std::map<size_t, std::string> category_name
 * категорию (множество) символов с именем default_name. Если такого множества ещё
 * нет, то добавляем с таким названием, и возвращаем его и true. Если же есть, то
 * возвращаем имеющееся название и false.
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