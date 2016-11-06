/*
    Файл:    error_count.cpp
    Создан:  13 декабря 2015г. в 09:05 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/error_count.h"
#include <cstdio>

void Error_count::increment_number_of_errors(){
    number_of_errors++;
}

int Error_count::get_number_of_errors(){
    return number_of_errors;
}

void Error_count::print(){
    printf("\nВсего ошибок: %d\n", number_of_errors);
}
