/*
    File:    trie_for_set_of_char.cpp
    Created: 06 November 2016г. в 12:20 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/trie.h"
#include "../include/trie_for_set_of_char.h"
#include <string>

std::set<char32_t> Trie_for_set_of_char::get_set(size_t idx){
    std::set<char32_t> s;
    size_t current = idx;
    for( ; current; current = node_buffer[current].parent){
        s.insert(node_buffer[current].c);
    }
    return s;
}

void Trie_for_set_of_char::post_action(const std::basic_string<char32_t>& s, size_t n){
}

size_t Trie_for_set_of_char::insertSet(const std::set<char32_t>& s){
    std::basic_string<char32_t> str;
    for(char32_t ch : s){
        str += ch;
    }
    size_t idx = insert(str);
    return idx;
}