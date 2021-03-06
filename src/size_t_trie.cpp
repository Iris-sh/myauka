/*
    File:    size_t_trie.cpp
    Created: 13 December 2015 at 09:05 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <vector>
#include <algorithm>
#include <functional>
#include <map>
#include <cstdio>
#include "../include/size_t_trie.h"

std::set<size_t> Size_t_trie::get_set(size_t idx){
    std::set<size_t> s;
    size_t current = idx;
    for( ; current; current = node_buffer[current].parent){
        s.insert(node_buffer[current].c);
    }
    return s;
}

void Size_t_trie::post_action(const std::basic_string<size_t>& s, size_t n){
}