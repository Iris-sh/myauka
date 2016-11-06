/*
    Файл:    trie.h
    Создан:  13 декабря 2015г. в 09:05 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef TRIE_H
#define TRIE_H

#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <string>
#include <set>

template<typename T>
class Trie {
public:
    /* Конструктор по умолчанию. */
    Trie<T>();
    /* Деструктор. */
    ~Trie() = default;
    /* Копирующий конструктор. */
    Trie(const Trie<T>& orig) = default;
    /* Функция вставки в префиксное дерево. */
    size_t insert(const std::basic_string<T>& s);
    /* Функция, вычисляющая максимальную степень вершин префиксного
     * дерева (корень дерева не учитывается). */
    size_t maximal_degree();
protected:
    /* тип узла префиксного дерева: */
    struct node{
      size_t parent, first_child, next;
      /* Все узлы-потомки текущего узла организованы в виде односвязного списка, первым
       * элементом которого является элемент с индексом first_child. В поле parent
       * содержится индекс родительского узла, а в поле next -- следующего потомка
       * родительского узла. Если у текущего узла потомков нет, то в поле first_child
       * содержится нуль. Аналогично, последний элемент в списке потомков в поле next
       * содержит нуль. Здесь под индексом понимается индекс в поле node_buffer,
       * представляющем собой вектор (в смысле библиотеки STL) из узлов префиксного
       * дерева. */
      size_t path_len; /* в этом поле содержится длина пути
                        * от текущего узла до корня дерева */
      size_t degree; /* В этом поле содержится степень узла,
                      * то есть количество выходящих из узла рёбер. */
      T c; /* в этом поле содержится символ вставленной строки,
            * являющийся меткой текущего узла. */
      node(){
        next = parent = path_len = first_child = 0;
        degree = 0; c = T();
      }
    };
    std::vector<node>   node_buffer;
    std::vector<size_t> nodes_indeces;
    /* Функция, добавляющая к списку потомков узла parent_idx узел, помеченный
     * значением x типа T. Функция возвращает индекс вставленного узла. */
    size_t add_child(size_t parent_idx, T x);
    /* Эта функция выполняет (возможно, необходимые) действия
     * по окончании вставки последнего символа. */
    virtual void post_action(const std::basic_string<T>& s, size_t n){ };
};

template<typename T>
Trie<T>::Trie(){
    node_buffer = std::vector<node>(1);
    nodes_indeces = std::vector<size_t>();
}

template<typename T>
size_t Trie<T>::maximal_degree(){
    size_t deg = 0;
    size_t len = node_buffer.size();
    for(int i = 1; i < len; i++){
        deg = std::max(deg,node_buffer[i].degree);
    }
    return deg;
}

template<typename T>
size_t Trie<T>::add_child(size_t parent_idx, T x){
    size_t current, previous;
    node   temp;
    current = previous = node_buffer[parent_idx].first_child;
    /* В переменной temp содержится узел, который, возможно, придётся вставить. */
    temp.c = x; temp.degree = 0;
    temp.next = 0; temp.parent = parent_idx;
    temp.path_len = node_buffer[parent_idx].path_len + 1;
    if(!current){
        /* Здесь можем оказаться, лишь если у узла с индексом parent_idx потомков
         * вообще нет. Значит добавляемый узел будет первым в списке потомков. При
         * этом степень узла parent_idx увеличится на единицу, и станет равна 1. */
        node_buffer.push_back(temp);
        size_t child_idx = node_buffer.size() - 1;
        node_buffer[parent_idx].first_child = child_idx;
        node_buffer[parent_idx].degree = 1;
        return child_idx;
    }
    while(current){
        // Если же потомки есть, то нужно пройти по списку потомков.
        node current_node = node_buffer[current];
        if(current_node.c == x){
          /* Если потомок, помеченный нужным символом (символом x),
           * есть, то нужно вернуть индекс этого потомка. */
          return current;
        }else{
          previous = current; current = current_node.next;
        }
    }
    /* Если же такого потомка нет, то нужно этого потомка добавить
     * в конец списка потомков.*/
    node_buffer.push_back(temp);
    size_t next_child = node_buffer.size() - 1;
    node_buffer[previous].next = next_child;
    node_buffer[parent_idx].degree++;
    return next_child;
}

template<typename T>
size_t Trie<T>::insert(const std::basic_string<T>& s){
    ssize_t len = s.length();
    size_t current_root = 0;
    for (ssize_t i = 0; i < len; i++) {
        current_root = add_child(current_root,s[i]);
    }
    nodes_indeces.push_back(current_root);
    post_action(s,current_root);
    return current_root;
}
#endif
