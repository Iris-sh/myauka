/*
    File:    create_dir_if_not_exists.h
    Created: 5 November 2016 at 12:33 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef CREATE_DIR_IF_NOT_EXISTS_H
#define CREATE_DIR_IF_NOT_EXISTS_H
#include <boost/filesystem/operations.hpp>
/**
    The following function creates a directory having a path p, if this directory
    doesn't exists. If there exists p, but p is not directory, then the returned
    value is false. Otherwise, the returned value is true.
*/
bool create_dir_if_not_exists(const boost::filesystem::path& p);
#endif