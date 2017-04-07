/*
    File:    size_t_trie.h
    Created: 06 November 2016г. в 12:20 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef SIZE_T_TRIE_H
#define SIZE_T_TRIE_H
#include "../include/trie.h"
/*! Данный класс предназначен для хранения множеств состояний недетерминированных конечных
 *  автоматов. Каждое такое множество в качестве внутреннего представления будет иметь
 *  строку из состояний, каждое из которых, в свой черёд, представляется своим номером,
 *  являющимся значением типа size_t. */
class Size_t_trie : public Trie<size_t> {
public:
    virtual ~Size_t_trie() { };
    Size_t_trie(){};
    Size_t_trie(const Size_t_trie& orig) = default;
    /*! Функция get_set по индексу idx множества состояний строит это же множество, но уже как
     * std::set<size_t> . */
    std::set<size_t> get_set(size_t idx);
private:
    virtual void post_action(const std::basic_string<size_t>& s, size_t n);
};
#endif