/*
    File:    move_by_ext_from_curr_dir.cpp
    Created: 5 November 2016г. в 13:41 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/move_by_ext_from_curr_dir.h"
#include "../include/create_dir_if_not_exists.h"
#include "../include/move_by_ext.h"

namespace fs = boost::filesystem;

bool move_by_ext_from_curr_dir(const boost::filesystem::path& path_to,
                               const std::string& ext)
{
    bool result = false;
    result = create_dir_if_not_exists(path_to);
    if(!result){
        return result;
    }

    fs::path path_from(".");

    auto aux_result = move_by_ext(path_from, path_to, ext);
    result = aux_result == Move_result::Success;
    return result;
}