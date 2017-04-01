/*
    File:    operations_with_sets.h
    Created: 13 декабря 2015г. в 09:05 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef OPERATIONS_WITH_SETS_H
#define OPERATIONS_WITH_SETS_H

#include <set>
#include <cstdio>
/**
    В данном файле определяются теоретико--множественные операции
    со стандартными контейнерами std::set.
*/
namespace operations_with_sets{
    //! Функция single_elem возвращает множество, состоящее из одного элемента.
    template<typename T>
    std::set<T> single_elem(const T& x){
        std::set<T> s;
        s.insert(x);
        return s;
    }

    /** Функция печати элементов множества. Принимает в качестве
        аргумента функцию печати элемента множества. */
    template<typename T>
    void print_set(const std::set<T>& a, void (*print_elem)(const T&)){
        if(a.empty()){
            printf("{}");
            return;
        }
        auto first       = a.begin();
        auto before_last = --a.end();
        putchar('{');
        for(auto i = first; i != before_last; ++i){
            print_elem(*i);
            putchar(',');
        }
        print_elem(*before_last);
        putchar('}');
    }

    template<typename T>
    void print_set(const std::set<T>& a, void (*print_elem)(const T)){
        if(a.empty()){
            printf("{}");
            return;
        }
        auto first       = a.begin();
        auto before_last = --a.end();
        putchar('{');
        for(auto i = first; i != before_last; ++i){
            print_elem(*i);
            putchar(',');
        }
        print_elem(*before_last);
        putchar('}');
    }

    /** Проверка принадлежности элемента x множеству a. Если элемент x
        множеству a принадлежит, то возвращается true, иначе ---
        возвращается false. */
    template<typename T>
    bool is_elem(const T& x, const std::set<T>& a){
        return a.find(x) != a.end();
    }

    /** Объединение множеств a и b, то есть множество, содержащее и
        элементы множества a, и элементы множества b. */
    template<typename T>
    std::set<T> operator + (const std::set<T>& a, const std::set<T>& b){
        std::set<T> s = a;
        s.insert(b.begin(), b.end());
        return s;
    }

    /** Теоретико--множественная разность множеств a и b (обозначается в
        теории множеств как a \ b), то есть множество, состоящее лишь из тех
        элементов множества a, которые не принадлежат множеству b. */
    template<typename T>
    std::set<T> operator - (const std::set<T>& a, const std::set<T>& b){
        std::set<T> s = a;
        for(const auto x : b){
            s.erase(x);
        }
        return s;
    }

    /** Пересечение множеств a и b, то есть множество, состоящее в точности из
        тех элементов, которые принадлежат и a, и b. */
    template<typename T>
    std::set<T> operator * (const std::set<T>& a, const std::set<T>& b){
        std::set<T> s;
        for(const auto& x : a){
            if(is_elem(x, b)){
                s.insert(x);
            }
        }
        return s;
    }

    /** Симметрическая разность множеств a и b, то есть объединение этих
        множеств с выкинутыми общими элементами. */
    template<typename T>
    std::set<T> operator ^ (const std::set<T>& a, const std::set<T>& b){
        return (a - b) + (b - a);
    }

    /** Проверяет, является ли множество a подмножеством множества b,
        возможно, совпадающим с b. */
    template<typename T>
    bool is_subseteq(const std::set<T>& a, const std::set<T>& b){
        std::set<T> s = (a * b) ^ a;
        return s.empty();
    }
};
#endif