/*
    Файл:    belongs.h
    Создан:  13 декабря 2015г. в 09:05 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef BELONGS_H
#define BELONGS_H
#include <cstdint>
/* Данная функция проверяет, принадлежит ли элемент element
 * множеству s. При этом считаем, что множество s состоит из
 * не более чем 64 элементов, так что в качестве внутреннего
 * представления множества используется тип uint64_t.
 * Если бит с номером i внутреннего представления равен 1,
 * то элемент i принадлежит множеству, иначе --- не принадлежит. */
inline uint64_t belongs(uint64_t element, uint64_t s){
    return s & (1ULL << element);
}
#endif