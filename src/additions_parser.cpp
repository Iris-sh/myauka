/*
    File:    additions_parser.cpp
    Created: 20 May 2017 at 14:08 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/additions_parser.h"

enum class State {
    Addition, Addition_string
};

size_t Additions_parser::compile(const Header_or_impl hi)
{
    size_t          ret_val = 0;
    State           state   = State::Addition;
    Main_lexem_code m       = (hi == Header_or_impl::Header) ?
                              Kw_header_additions : Kw_impl_additions;
    while((lc = (li = msc->current_lexem()).code){
        switch(state){
            case State::Addition:
                if(lc == m){
                    state = State::Addition_string;
                }else{
                    msc_->back();
                    return ret_val;
                }
                break;
            case State::Addition_string:
                if(String == lc){
                    return li.string_index;
                }else{
                    printf("Error at line %zu: the string literal was expected.\n",
                           msc->lexem_begin_line_number() );
                    msc_->back();
                    return ret_val;
                }
                break;
        }
    }
    return ret_val;
}