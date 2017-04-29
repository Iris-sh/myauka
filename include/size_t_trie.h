/*
    File:    size_t_trie.h
    Created: 06 November 2016 at 12:20 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef SIZE_T_TRIE_H
#define SIZE_T_TRIE_H
#include "../include/trie.h"
/**
 * \class Size_t_trie This class is intended for storing the sets of states of
 *                    nondeterministic finite automata. Each such set as an internal
 *                    representation will have a string of states, each of which, in
 *                    its turn, is represented by its number, which is a value of
 *                    type size_t.
 */
class Size_t_trie : public Trie<size_t> {
public:
    virtual ~Size_t_trie() { };
    Size_t_trie(){};
    Size_t_trie(const Size_t_trie& orig) = default;

    /**
     *  \brief The function get_set on the index idx of the set of states builds
     *         the same set, but already as std::set < size_t >.
     *  \param [in] idx The index of the set of states in the prefix tree of such sets.
     *  \return         The same set, but already as std::set < size_t >.
     */
    std::set<size_t> get_set(size_t idx);
private:
    virtual void post_action(const std::basic_string<size_t>& s, size_t n);
};
#endif