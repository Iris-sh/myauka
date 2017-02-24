/*
    Файл:    get_act_repres.cpp
    Создан:  04 февраля 2017г. в 18:35 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/get_act_repres.h"
#include "../include/idx_to_string.h"

/* Данная функция на вход получает индекс имени действия, i, в префиксном дереве
 * идентификаторов, а в качестве результата выдаёт строковое представление
 * определения действия. */
std::string get_act_repres(Info_for_constructing& info, size_t i){
    /* Непустая строка возвращается только в том случае, когда действие
     * с таким индексом имени есть. */
    std::string result;
    if(!i){
        return result;
    }
    auto isc = info.scope->idsc;
    auto iti = isc.find(i);
    if(iti != isc.end()){
        size_t act_str_idx = iti->second.act_string;
        result = idx_to_string(info.et.strs_trie, act_str_idx);
    }
    return result;
}