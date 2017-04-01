/*
    File:    attributed_char_trie.h
    Created: 06 ноября 2016г. в 12:20 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef ATTRIBUTED_CHAR_TRIE
#define ATTRIBUTED_CHAR_TRIE

#include "../include/trie.h"

struct Attributed_char {
    size_t         attribute; /* атрибут символа (код лексемы) */
    char32_t       ch;        /* сам символ */
};

bool operator == (Attributed_char x, Attributed_char y);

using Attributed_string = std::basic_string<Attributed_char>;

struct Attributed_cstring {
    char32_t* str;       /* Строковое представление лексемы. */
    size_t    attribute; /* Код лексемы. */
};

Attributed_string attributed_cstring2string(const Attributed_cstring acstr,
                                            size_t default_attr);

struct Attributed_char_trie_as_map {
    std::map<size_t,Attributed_cstring> *m;
    ~Attributed_char_trie_as_map();
};

struct Jump_chars {
    std::u32string jump_chars;
    size_t         code;
    size_t         first_state;
};

using Jumps = std::vector<Jump_chars>;
using Init  = std::pair<size_t,char32_t>;
using Inits = std::vector<Init>;

struct Jumps_and_inits {
    Jumps jumps;
    Inits init_table;
};

class Attributed_char_trie : public Trie<Attributed_char>{
public:
    virtual ~Attributed_char_trie() { };
    /* Конструктор по умолчанию. */
    Attributed_char_trie(){};
    /* Копирующий конструктор. */
    Attributed_char_trie(const Attributed_char_trie& orig) = default;
    /* Данная функция по индексу idx строит соответствующую
     * этому индексу пару (строка в стиле C, код лексемы). */
    Attributed_cstring get_attributed_cstring(size_t idx);
    /* Функция, возвращающая префиксное дерево в виде отображения индексов лексем строк
     * с кодами соответствующих в строки в стиле C, для которых указаны коды лексем. */
    Attributed_char_trie_as_map as_map();
    /* Функция, строящая заготовку для таблицы переходов. */
    Jumps_and_inits jumps();
private:
    void get_next_level(const std::vector<size_t>& current_level,
                        std::vector<size_t>& next_level);
    size_t jumps_for_subtrie(size_t root_child, size_t current_state,
                             Jumps& current_jumps);
    virtual void post_action(const std::basic_string<Attributed_char>& s, size_t n);
};
#endif
