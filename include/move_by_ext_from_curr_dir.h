/*
    File:    move_by_ext_from_curr_dir.h
    Created: 5 November 2016 at 12:55 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef MOVE_BY_EXT_FROM_CURR_DIR_H
#define MOVE_BY_EXT_FROM_CURR_DIR_H
#include <string>
#include <boost/filesystem/operations.hpp>

/**
 * \param [in] path_to The directory in which to move files from the current directory
 * \param [in] ext     Extension of moved files
 * \return             true if the operation was successful, and false otherwise
*/
bool move_by_ext_from_curr_dir(const boost::filesystem::path& path_to,
                               const std::string& ext);
#endif