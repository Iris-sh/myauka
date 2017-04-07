/*
    File:    fsize.h
    Author:  Гаврилов Владимир Сергеевич
    Created: 06 August 2015, 10:06
    e-mails: vladimir.s.gavrilov@gmail.com,
             gavrilov.vladimir.s@mail.ru,
             gavvs1977@yandex.ru
*/

#ifndef FSIZE_H
#define FSIZE_H
#include <cstdio>
/* Данная функция выдаёт размер файла в байтах, если
 * fptr != NULL, и (-1) в противном случае. */
long fsize(FILE* fptr);
#endif