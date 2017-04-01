/* 
    File:    abstract_scaner.h
    Created: 13 декабря 2015г. в 09:05 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef ABSTRACT_SCANER_H
#define ABSTRACT_SCANER_H

#include <string>
#include <memory>
#include "../include/error_count.h"
#include "../include/trie.h"
#include "../include/location.h"
#include "../include/errors_and_tries.h"

template<typename Lexem_type>
class Scaner{
public:
    Scaner<Lexem_type>() = default;
    Scaner(Location_ptr location, const Errors_and_tries& et);
    Scaner(const Scaner<Lexem_type>& orig) = default;
    virtual ~Scaner() = default;
    /* Функция back() возвращает текущую лексему во входной поток.*/
    void back();
    /* Функция current_lexem() возвращает сведения о текущей
     * лексеме (код лексемы и значение лексемы). */
    virtual Lexem_type current_lexem() = 0;
    /* Функция lexem_begin_line_number() возвращает номер строки
     * обрабатываемого текста, с которой начинается лексема,
     * сведения о которой возвращены функцией current_lexem(). */
    size_t lexem_begin_line_number();
protected:
    int                          state; /* текущее состояние текущего автомата */

    Location_ptr                 loc;
    char32_t*                    lexem_begin; /* указатель на начало лексемы */
    char32_t                     ch;          /* текущий символ */

    /* множество категорий символов, которым принадлежит
     * текущий символ */
    uint64_t                     char_categories;

    /* промежуточное значение сведений о лексеме */
    Lexem_type                   token;

    /* номер строки, с которой начинается текущая лексема */
    size_t                       lexem_begin_line;

    /* указатель на класс, подсчитывающий количество ошибок: */
    std::shared_ptr<Error_count> en;
    /* указатель на префиксное дерево для идентификаторов: */
    std::shared_ptr<Char_trie>   ids;
    /* указатель на префиксное дерево для строк: */
    std::shared_ptr<Char_trie>   strs;

    /*буфер для записи обрабатываемого идентификатора или строки: */
    std::u32string               buffer;
};

template<typename Lexem_type>
Scaner<Lexem_type>::Scaner(Location_ptr location, const Errors_and_tries& et){
    ids = et.ids_trie; strs = et.strs_trie; en = et.ec;
    loc = location;
    lexem_begin = location->pcurrent_char;
    lexem_begin_line = 1;
}

template<typename Lexem_type>
void Scaner<Lexem_type>::back(){
    loc->pcurrent_char = lexem_begin;
    loc->current_line =  lexem_begin_line;
}

template<typename Lexem_type>
size_t Scaner<Lexem_type>::lexem_begin_line_number(){
    return lexem_begin_line;
}
#endif