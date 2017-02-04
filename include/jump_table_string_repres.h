/*
    Файл:    jump_table_string_repres.h
    Создан:  04 февраля 2017г. в 11:30 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef JUMP_TABLE_STRING_REPRES_H
#define JUMP_TABLE_STRING_REPRES_H
#include "../include/attributed_char_trie.h"
#include "../include/info_for_constructing.h"
#include <string>
std::string jump_table_string_repres(Info_for_constructing& info,
                                     const Jumps_and_inits& ji,
                                     const std::string&     table_name,
                                     const std::string&     init_table_name);
#endif