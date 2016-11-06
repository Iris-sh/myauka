/*
    File:    file_contents.h
    Author:  Гаврилов Владимир Сергеевич
    Created: 4 февраля 2016, 13:10
    e-mails: vladimir.s.gavrilov@gmail.com,
             gavrilov.vladimir.s@mail.ru,
             gavvs1977@yandex.ru
*/

#ifndef FILE_CONTENTS_H
#define FILE_CONTENTS_H
#include <string>
#include <utility>

/** Коды возврата из функции get_contents. */
enum class Get_contents_return_code{
    Normal,           ///< Этот код означает, что всё прошло успешно.
    Impossible_open,  ///< Этот код означает, что не удалось открыть файл.
    Read_error        ///< Этот код означает, что во время чтения файла произошла ошибка.
};

using Contents  = std::pair<Get_contents_return_code, std::string>;

/**
   Возвращает всё содержимое файла с заданным именем.
   \param [in] name --- имя читаемого файла
   \returns Пару из кода возврата (первая компонента) и значения, имеющего
тип std::string (вторая компонента). При возникновении ошибки вторая компонента
возвращаемого значения представляет собой пустую строку.
*/
Contents get_contents(const char* name);
#endif