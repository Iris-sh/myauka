/*
    File:    attributed_char_trie.h
    Created: 06 November 2016г. в 12:20 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef ATTRIBUTED_CHAR_TRIE
#define ATTRIBUTED_CHAR_TRIE

#include "../include/trie.h"

struct Attributed_char {
    size_t         attribute; /* character attribute (lexem code) */
    char32_t       ch;        /* character */
};

bool operator == (Attributed_char x, Attributed_char y);

using Attributed_string = std::basic_string<Attributed_char>;

struct Attributed_cstring {
    char32_t* str;       /* string representation of lexem */
    size_t    attribute; /* lexem code */
};

Attributed_string attributed_cstring2string(const Attributed_cstring acstr,
                                            size_t default_attr);

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

    Attributed_char_trie(){};

    Attributed_char_trie(const Attributed_char_trie& orig) = default;

    /* Using the index idx, this function builds the corresponding pair
     *     (C-style string, lexem code).
     */
    Attributed_cstring get_attributed_cstring(size_t idx);

    /* This function builds the intermediate repesentation of the transition table. */
    Jumps_and_inits jumps();
private:
    void get_next_level(const std::vector<size_t>& current_level,
                        std::vector<size_t>& next_level);
    size_t jumps_for_subtrie(size_t root_child, size_t current_state,
                             Jumps& current_jumps);
    virtual void post_action(const std::basic_string<Attributed_char>& s, size_t n);
};
#endif
