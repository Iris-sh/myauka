/*
    File:    get_act_repres.cpp
    Created: 04 February 2017 at 18:35 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/get_act_repres.h"
#include "../include/idx_to_string.h"
/**
 *  \param [in] info information about constructing scaner
 *  \param [in] i    index of action name in the prefix tree of identifiers
 *
 *  \return          string representation of action
 */
std::string get_act_repres(Info_for_constructing& info, size_t i){
    /* A non-empty string is returned only if there is an action with
     * such an index name. */
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