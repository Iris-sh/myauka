/*
    File:    lexem_info_name_parser.cpp
    Created: 24 June 2017 at 14:06 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/lexem_info_name_parser.h"

enum class State{
    Keyword, Name
};

size_t Lexem_info_name_parser::compile(){
    size_t          ret_val = 0;
    State           state   = State::Keyword;
    while((lc = (li = msc_->current_lexem()).code)){
        switch(state){
            case State::Keyword:
                if(Kw_lexem_info_name == lc){
                    state = State::Name;
                }else{
                    msc_->back();
                    return ret_val;
                }
                break;
            case State::Name:
//                 if(String == lc){
//                     return li.string_index;
//                 }else{
//                     printf("Error at line %zu: the string literal was expected.\n",
//                            msc_->lexem_begin_line_number() );
//                     msc_->back();
//                     return ret_val;
//                 }
                break;
        }
    }
    return ret_val;
}