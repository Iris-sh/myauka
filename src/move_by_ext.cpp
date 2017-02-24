/*
    Файл:    move_by_ext.cpp
    Создан:  5 ноября 2016г. в 12:46 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/move_by_ext.h"

namespace fs = boost::filesystem;

Move_result move_by_ext(const boost::filesystem::path& path_from,
                        const boost::filesystem::path& path_to,
                        const std::string& ext)
{
    Move_result result = Move_result::Success;

    if(!fs::exists(path_from)){
        return Move_result::Source_dir_does_not_exists;
    }

    if(!fs::is_directory(path_from)){
        return Move_result::Source_is_not_directory;
    }

    if(!fs::exists(path_to)){
        return Move_result::Destination_dir_does_not_exists;
    }

    if(!fs::is_directory(path_to)){
        return Move_result::Destination_is_not_directory;
    }

    fs::path e;
    if(ext.empty()){
        e = fs::path(".");
    }else if(ext[0] != '.'){
        e = fs::path("." + ext);
    }else{
        e = fs::path(ext);
    }

    for(auto& x : fs::directory_iterator(path_from)){
        auto curr_name = x.path().filename();
        auto curr_ext  = curr_name.extension();
        if(!curr_ext.compare(e)){
            fs::rename(x, path_to / curr_name);
        }
    }
    return result;
}