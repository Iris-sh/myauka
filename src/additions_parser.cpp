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
    size_t ret_val = 0;
    State state = State::Addition;
    while((lc = (li = msc->current_lexem()).code){
        switch(state){}
    }
    return ret_val;
}