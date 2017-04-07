/*
    File:    move_by_ext_from_curr_dir.h
    Created: 5 November 2016г. в 12:55 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef MOVE_BY_EXT_FROM_CURR_DIR_H
#define MOVE_BY_EXT_FROM_CURR_DIR_H
#include <string>
#include <boost/filesystem/operations.hpp>

/** Следующая функция перемещает файлы из текущего каталога,
 *  имеющие заданное расширение ext, в каталог path_to.
 *  Функция возвращает true, если операция прошла успешно,
 *  и false в противном случае. */
bool move_by_ext_from_curr_dir(const boost::filesystem::path& path_to,
                               const std::string& ext);
#endif