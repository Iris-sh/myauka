/*
    File:    conv_case.cpp
    Created: 02 февраля 2017г. в 11:35 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/conv_case.h"
#include <cctype>
std::string tolower_case(const std::string& s){
    std::string result = s;
    for(char& c : result){
        c = tolower(c);
    }
    return result;
}

std::string toupper_case(const std::string& s){
    std::string result = s;
    for(char& c : result){
        c = toupper(c);
    }
    return result;
}