/*
    File:    error_count.h
    Created: 13 December 2015г. в 09:05 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef ERROR_COUNT_H
#define ERROR_COUNT_H
/* Класс для подсчёта количества ошибок. */
class Error_count {
public:
    Error_count() : number_of_errors(0) {};
    void increment_number_of_errors();
    void print();
    int get_number_of_errors();
private:
    int number_of_errors;
};
#endif