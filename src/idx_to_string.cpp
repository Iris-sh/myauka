/* 
    File:    idx_to_string.cpp
    Created: 13 декабря 2015г. в 09:05 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/idx_to_string.h"
#include "../include/char_conv.h"
std::string idx_to_string(std::shared_ptr<Char_trie> t, size_t idx){
    auto u32str = t->get_string(idx);
    std::string s = u32string_to_utf8(u32str);
    return s;
}