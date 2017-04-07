/*
    File:    create_dir_if_not_exists.cpp
    Created: 5 November 2016г. в 12:37 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/create_dir_if_not_exists.h"

namespace fs = boost::filesystem;

bool create_dir_if_not_exists(const boost::filesystem::path& p){
    bool result = true;
    if(!fs::exists(p)){
       result = fs::create_directory(p);
       return result;
    }

    if(!fs::is_directory(p)){
        result = false;
    }
    return result;
}