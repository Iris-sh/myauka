/*
    File:    move_by_ext.h
    Created: 5 ноября 2016г. в 12:44 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef MOVE_BY_EXT_H
#define MOVE_BY_EXT_H
#include <string>
#include <boost/filesystem/operations.hpp>
enum class Move_result {
    Source_dir_does_not_exists,      Source_is_not_directory,
    Destination_dir_does_not_exists, Destination_is_not_directory,
    Success
};

/** Следующая функция перемещает файлы с расширением ext из каталога
    path_from в каталог path_to. */
Move_result move_by_ext(const boost::filesystem::path& path_from,
                        const boost::filesystem::path& path_to,
                        const std::string& ext);
#endif