/*
    Файл:    init_and_final_acts.h
    Создан:  04 февраля 2017г. в 14:39 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef INIT_AND_FINAL_ACTS_H
#define INIT_AND_FINAL_ACTS_H
/* Пользовательские действия, выполняемые при инициализации и при завершении
   автомата обработки строк (или автомата обработки чисел), порождённого из
   описания сканера.
*/

struct Init_and_final_acts{
    size_t init_acts = 0;
    size_t fin_acts  = 0;
};
#endif