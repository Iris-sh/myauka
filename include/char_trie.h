/*
    File:    char_trie.h
    Created: 06 November 2016г. в 12:20 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef CHAR_TRIE_H
#define CHAR_TRIE_H

#include "../include/trie.h"

struct Char_trie_as_map {
    std::map<size_t,char32_t*> *m;
    ~Char_trie_as_map();
};

class Char_trie : public Trie<char32_t>{
public:
    virtual ~Char_trie() { };
    /* Конструктор по умолчанию. */
    Char_trie(){};
    /* Копирующий конструктор. */
    Char_trie(const Char_trie& orig) = default;
    /* Функция, по индексу idx строящая строку в стиле C,
     * соответствующую индексу idx. */
    char32_t* get_cstring(size_t idx);
    /* Функция, по индексу idx строящая строку типа u32string,
       соответствующую индексу idx. */
    std::u32string get_string(size_t idx);
    /* Функция, возвращающая префиксное дерево в виде
     * отображения индексов строк в строки в стиле C. */
    Char_trie_as_map as_map();
    /* Функция вывода строки, которой соответствует индекс idx, на экран. */
    void print(size_t idx);
    /* Следующая функция по индексу строки возвращает длину этой строки. */
    size_t get_length(size_t idx);
};

#endif