/* 
    File:    scope.h
    Created: 13 декабря 2015г. в 09:05 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef SCOPE_H
#define SCOPE_H

#include <map>
enum Id_kind{
    Scaner_name,   Codes_type_name, Lexem_ident_name,
    Code_of_lexem, Action_name
};

enum Str_kind {
    Keyword_repres,             Delimiter_repres,
    Added_members,              Added_token_fiels,
    Action_definition,          Single_lined_comment_begin,
    Multi_lined_comment_begin,  Multi_lined_comment_end
};

struct Id_attributes{
    Id_kind kind; /* Вид идентификатора, то есть является
                   * ли этот идентификатор именем сканера,
                   * именем типа кодов лексем, кодом лексемы,
                   * или именем действия. */
    union {
        size_t code; /* числовое значение кода лексемы,
                      * если идентификатор --- код лексемы */
        size_t act_string; /* индекс строкового литерала,
                            * являющегося определением
                            * действия, в префиксном дереве
                            * строковых литералов*/
    };
};

struct Str_attributes{
    Str_kind kind; /* Вид строкового литерала, то есть является
                    * строковым представлением ключевого слова,
                    * строковым представлением разделителя,
                    * списком добавляемых в класс сканера членов,
                    * определением действия, началом однострочного
                    * комментария, началом многострочного
                    * комментария, или концом многострочного
                    * комментария. */
    size_t code; /* код лексемы, если строковый литерал является
                  * представлением ключевого слова или разделителя */
};

using Id_scope = std::map<size_t, Id_attributes>;

using Str_scope = std::map<size_t, Str_attributes>;

class Scope {
public:
    Id_scope  idsc; /* Отображение индексов идентификаторов в
                     * атрибуты идентификаторов. */
    Str_scope strsc; /* Отображение индексов строковых литералов
                      * в атрибуты литералов. */
    Scope()  = default;
    ~Scope() = default;
    Scope(const Scope& orig) = default;
};
#endif