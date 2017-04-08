/*
    File:    char_trie.cpp
    Created: 06 November 2016 at 12:20 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/char_conv.h"
#include "../include/char_trie.h"
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <string>
#include <set>

Char_trie_as_map::~Char_trie_as_map(){
    for(auto x : *m){
        delete [] x.second;
    }
}

Char_trie_as_map Char_trie::as_map(){
    Char_trie_as_map t;
    t.m = new std::map<size_t,char32_t*>();
    for(auto x : nodes_indeces){
        t.m -> insert({x,get_cstring(x)});
    }
    return t;
}

char32_t* Char_trie::get_cstring(size_t idx){
    size_t id_len = node_buffer[idx].path_len;
    char32_t* p = new char32_t[id_len + 1];
    p[id_len] = 0;
    size_t current = idx;
    size_t i       = id_len-1;
    /* Поскольку idx -- индекс элемента в node_buffer, содержащего последний символ
     * вставленной строки, а каждый элемент вектора node_buffer содержит поле parent,
     * указывающее на элемент с предыдущим символом строки, то для получения
     * вставленной строки, которой соответствует индекс idx, в виде массива символов,
     * нужно пройтись от элемента с индексом idx к корню. При этом символы вставленной
     * строки будут читаться от её конца к началу. */
    for( ; current; current = node_buffer[current].parent){
        p[i--] = node_buffer[current].c;
    }
    return p;
}

std::u32string Char_trie::get_string(size_t idx){
    char32_t* p = get_cstring(idx);
    std::u32string s = std::u32string(p);
    delete [] p;
    return s;
}

void Char_trie::print(size_t idx){
    std::u32string s32 = get_string(idx);
    std::string    s8  = u32string_to_utf8(s32);
    printf("%s",s8.c_str());
}

size_t Char_trie::get_length(size_t idx){
    return node_buffer[idx].path_len;
}