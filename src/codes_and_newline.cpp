/*
    File:    codes_and_newline.cpp
    Created: 11 August 2016 at 7:40 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/codes_and_newline.h"
#include <cstdio>

const char32_t* newline_lexem_str = U"Newline";

void Codes_and_newline::newline_is_lexem_sec(){
    li = msc->current_lexem();
    size_t idx       = et_.ids_trie->insert(newline_lexem_str);
    add_new_lexem_code(idx);
}

void Codes_and_newline::add_new_lexem_code(size_t idx){
    auto s = scope_->idsc.find(idx);
    Id_attributes iattr;
    if(s != scope_->idsc.end()){
        printf("Line %zu repeatedly defines identifier ",
               msc->lexem_begin_line_number());
        et_.ids_trie->print(idx); printf("\n");
        et_.ec -> increment_number_of_errors();
    }else{
        iattr.kind = Code_of_lexem;
        iattr.code = ++last_code_val;
        scope_->idsc[idx] = iattr;
        codes.push_back(idx);
    }
}

Codes_and_newline::State_proc Codes_and_newline::procs[] = {
    &Codes_and_newline::codes_id_proc, &Codes_and_newline::codes_comma_sep_proc
};

void Codes_and_newline::codes_sec(std::vector<size_t>& codes_, size_t& last_code){
    codes = codes_; last_code_val = last_code;
    codes_sec_();
    codes_ = codes; last_code = last_code_val;
}

void Codes_and_newline::codes_sec_(){
    lc = (li = msc-> current_lexem()).code;
    if(lc != Kw_codes){
        printf("The line %zu expects %%codes.\n", msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
        msc->back();
        return;
    }

    state = Codes_id;
    for( ; ; ){
        lc = (li = msc-> current_lexem()).code;
        bool t = (this->*procs[state])();
        if(!t){return;};
    }
}

bool Codes_and_newline::codes_id_proc(){
    bool t = true;
    if(Id == lc){
        add_new_lexem_code(li.ident_index);
        state = Codes_comma_sep;
        return t;
    }
    printf("Missing identifier in line %zu.\n", msc->lexem_begin_line_number());
    et_.ec -> increment_number_of_errors();
    if(Comma == lc){
        state = Codes_comma_sep;
    }else{
        msc->back(); t = false;
    }
    return t;
}

bool Codes_and_newline::codes_comma_sep_proc(){
    bool t = true;
    switch(lc){
        case Id:
            printf("In line %zu, a comma is expected.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            add_new_lexem_code(li.ident_index);
            state = Codes_comma_sep;
            break;

        case Comma:
            state = Codes_id;
            break;

        default:
            msc->back(); t = false;
    }
    return t;
}