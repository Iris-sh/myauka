/*
    File:    move_by_ext.h
    Created: 5 November 2016 at 12:44 Moscow time
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

/**
 * \param [in] path_from The directory from which to move files
 * \param [in] path_to   The directory in which to move files
 * \param [in] ext       Extension of moved files
 *
 * \return               Return code of type Move_result
 */
Move_result move_by_ext(const boost::filesystem::path& path_from,
                        const boost::filesystem::path& path_to,
                        const std::string& ext);
#endif