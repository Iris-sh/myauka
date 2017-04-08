/*
    File:    attributed_char_trie.cpp
    Created: 06 November 2016 at 12:20 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/attributed_char_trie.h"
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <string>
#include <set>

Attributed_string attributed_cstring2string(const Attributed_cstring acstr,
                                            size_t default_attr){
    char32_t* p = acstr.str;
    Attributed_string astr;
    while(char32_t c = *p++){
        Attributed_char ac;
        ac.attribute = default_attr; ac.ch = c;
        astr += ac;
    }
    astr[astr.length()-1].attribute = acstr.attribute;
    return astr;
}

Attributed_cstring Attributed_char_trie::get_attributed_cstring(size_t idx){
    size_t id_len = node_buffer[idx].path_len;
    char32_t* p = new char32_t[id_len + 1];
    p[id_len] = 0;
    size_t current = idx;
    size_t i       = id_len-1;
    /* Поскольку idx -- индекс элемента в node_buffer, содержащего последний символ
     * вставленной строки, а каждый элемент вектора node_buffer содержит поле parent,
     * указывающее на элемент с предыдущим символом строки, то для получения вставленной
     * строки, которой соответствует индекс idx, в виде массива символов, нужно пройтись
     * от элемента с индексом idx к корню. При этом символы вставленной строки будут
     * читаться от её конца к началу. */
    for( ; current; current = node_buffer[current].parent){
        p[i--] = node_buffer[current].c.ch;
    }
    Attributed_cstring astr =
        {.str = p, .attribute = node_buffer[idx].c.attribute};
    return astr;
}

bool operator == (Attributed_char x, Attributed_char y){
    return x.ch == y.ch;
}

void Attributed_char_trie::post_action(const std::basic_string<Attributed_char>& s,
                                       size_t n){
    node_buffer[n].c.attribute = s[s.length()-1].attribute;
}

void Attributed_char_trie::get_next_level(const std::vector<size_t>& current_level,
                                          std::vector<size_t>& next_level){
    next_level = std::vector<size_t>();
    for(size_t x : current_level){
        size_t current_child = node_buffer[x].first_child;
        while(current_child){
            next_level.push_back(current_child);
            current_child = node_buffer[current_child].next;
        }
    }
}

size_t Attributed_char_trie::jumps_for_subtrie(size_t subtrie_root,
                                               size_t current_state,
                                               Jumps& current_jumps){
    /* Создадим вектор из уровней поддерева, корнем которого является
     * subtrie_root. Нулевым уровнем считаем сам узел subtrie_root. */
    using Level = std::vector<size_t>;
    std::vector<Level> levels = std::vector<Level>(1);
    levels[0].push_back(subtrie_root);
    size_t current_level_number = 0;
    while(true){
        Level next_level;
        get_next_level(levels[current_level_number],next_level);
        if(next_level.empty()){break;}
        levels.push_back(next_level);
        current_level_number++;
    }
    /* Теперь можем дописать заготовку для таблицы переходов. */
    size_t state = current_state;
    for(const auto& layer : levels){
    /* Цикл по слоям. */
        for(size_t x : layer){
        /* Цикл по текущему слою. */
            Jump_chars jc;
            jc.jump_chars = U""; jc.first_state = state + 1;
            jc.code = node_buffer[x].c.attribute;
            size_t current_child = node_buffer[x].first_child;
            while(current_child){
            /* Цикл по потомкам текущего узла. */
                jc.jump_chars += node_buffer[current_child].c.ch;
                current_child = node_buffer[current_child].next;
            }
            state++;
            if(jc.jump_chars.empty()){
                jc.first_state = 0;
            }
            current_jumps.push_back(jc);
        }
    }
    return state;
}

Jumps_and_inits Attributed_char_trie::jumps(){
    Jumps_and_inits ji;
    size_t subtrie_root = node_buffer[0].first_child;
    size_t current_state = 0;
    while(subtrie_root){
        ji.init_table.push_back(
            std::pair<size_t,char32_t>(current_state, node_buffer[subtrie_root].c.ch)
        );
        current_state = jumps_for_subtrie(subtrie_root, current_state, ji.jumps);
        subtrie_root = node_buffer[subtrie_root].next;
    }
    if(!ji.init_table.empty()){
        std::sort(ji.init_table.begin(), ji.init_table.end(),
                  [](Init a, Init b){return a.second < b.second;});
    }
    return ji;
}