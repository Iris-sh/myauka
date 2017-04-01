/*
    File:    trie_for_set_of_char.h
    Created: 06 ноября 2016г. в 12:20 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef TRIE_FOR_SET_OF_CHAR_H
#define TRIE_FOR_SET_OF_CHAR_H
#include "../include/trie.h"
#include <set>
class Trie_for_set_of_char : public Trie<char32_t>{
public:
    virtual ~Trie_for_set_of_char() { };
    Trie_for_set_of_char(){};
    Trie_for_set_of_char(const Trie_for_set_of_char& orig) = default;
    /*! Функция get_set по индексу idx множества символов типа char32_t
     * строит это же  множество, но уже как std::set<char32_t> . */
    std::set<char32_t> get_set(size_t idx);
    size_t insertSet(const std::set<char32_t>& s);
private:
    virtual void post_action(const std::basic_string<char32_t>& s, size_t n);
};
#endif