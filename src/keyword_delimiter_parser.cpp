/*
    Файл:    keyword_delimiter_parser.cpp
    Создан:  12 августа 2016г. в 10:51 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstdio>
#include <tuple>
#include "../include/keyword_delimiter_parser.h"
#include "../include/belongs.h"

KW_parser::State_proc KW_parser::procs[] = {
    &KW_parser::maybe_repres_str_proc,     &KW_parser::colon_sep0_proc,
    &KW_parser::maybe_code_kw_or_del_proc, &KW_parser::repres_str_proc,
    &KW_parser::code_kw_or_del_proc,       &KW_parser::colon_sep_proc,
    &KW_parser::kw_del_comma_sep_proc
};

static const std::string defined_keyword =
    "В строке %zu повторно определено ключевое слово ";

static const std::string defined_delimiter =
    "В строке %zu повторно определён разделитель ";

Settings Keyword_parser::settings(){
    return std::make_tuple(defined_keyword, Keyword_repres, Kw_keywords);
}

Settings Delimiter_parser::settings(){
    return std::make_tuple(defined_delimiter, Delimiter_repres, Kw_delimiters);
}

void KW_parser::add_new_string(const size_t idx, const size_t code_){
/* Первый аргумент данной функции --- индекс строкового литерала, представляющего
 * ключевое слово или разделитель, в префиксном дереве строковых литералов, а
 * второй аргумент --- индекс идентификатора, являющегося соответствующим кодом лексемы,
 * в префиксном дереве идентификаторов. */
    auto s = scope_->strsc.find(idx);
    Str_attributes sattr;
    Id_attributes  iattr;
    if(s != scope_->strsc.end()){
        printf(error_message_format_str.c_str(), msc->lexem_begin_line_number());
        et_.strs_trie->print(idx); printf("\n");
        et_.ec -> increment_number_of_errors();
    }else{
        auto s1 = scope_->idsc.find(code_);
        if(s1 == scope_->idsc.end()){
             iattr.kind = Code_of_lexem;
             iattr.code = ++last_code_val;
             scope_->idsc[idx] = iattr;
             codes.push_back(idx);
        }else{
            iattr = s1->second;
        }
    sattr.kind  = k;
    sattr.code  = iattr.code;
    scope_->strsc[idx] = sattr;
    repres.push_back(idx);
    }
}

size_t KW_parser::compile(std::vector<size_t>& repres_, std::vector<size_t>& codes_,
                          size_t& last_code){
    last_code_val = last_code;
    repres        = repres_;
    codes         = codes_;
    compile_();
    last_code     = last_code_val;
    repres_       = repres;
    codes_        = codes;
    return postaction;
}

void KW_parser::compile_(){
    lc = (li = msc-> current_lexem()).code;
    Main_lexem_code checked_code;

    std::tie(error_message_format_str, k, checked_code) = settings();

    if(lc != checked_code){
        msc->back();
        return;
    }

    state                    = Maybe_repres_str;
    maybe_repres_str_idx     = 0;
    //maybe_code_kw_or_del_idx = 0;
    idx                      = 0;
    postaction               = 0;
    for( ; ; ){
        lc = (li = msc-> current_lexem()).code;
        bool t = (this->*procs[state])();
        if(!t){return;};
    }
}

bool KW_parser::maybe_repres_str_proc(){
    bool t = true;
    if(String == lc){
            maybe_repres_str_idx = li.string_index;
            state = Colon_sep0;
            return t;
    }
    if(belongs(lc, 1ULL << Colon | 1ULL << Id | 1ULL << Comma)){
        printf("В строке %zu ожидается строковый литерал, являющийся либо действием "
                "по завершении, либо представлением ключевого слова или разделителя.\n",
                msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
        switch(lc){
            case Colon:
                state = Maybe_code_kw_or_del;
                break;
            case Id:
                idx = maybe_repres_str_idx;
                add_new_string(idx, li.ident_index);
                state = Code_kw_or_del;
                break;
            case Comma:
                state = Kw_del_comma_sep;
                break;
            default:
                ;
        }
    }else{
        msc->back(); t = false;
    }
    return t;
}

bool KW_parser::colon_sep0_proc(){
    bool t = true;
    if(Colon == lc){
        state = Maybe_code_kw_or_del;
    }else if(belongs(lc, 1ULL << String | 1ULL << Id | 1ULL << Comma)){
        printf("В строке %zu пропущено двоеточие.\n", msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
        switch(lc){
            case Id:
                idx = maybe_repres_str_idx;
                add_new_string(idx, li.ident_index);
                state = Code_kw_or_del;
                break;
            case String:
                postaction = maybe_repres_str_idx;
                idx = li.string_index;
                state = Repres_str;
                break;
            case Comma:
                idx = maybe_repres_str_idx;
                add_new_string(idx, li.ident_index);
                state = Kw_del_comma_sep;
                break;
            default:
                ;
        }
    }else{
        msc->back(); t = false;
    }
    return t;
}

bool KW_parser::maybe_code_kw_or_del_proc(){
    bool t = true;
    switch(lc){
        case Id:
            idx = maybe_repres_str_idx;
            add_new_string(idx, li.ident_index);
            state = Code_kw_or_del;
            break;
        case String:
            postaction = maybe_repres_str_idx;
            idx = li.string_index;
            state = Repres_str;
            break;
        case Colon:
            printf("В строке %zu ожидается либо идентификатор, являющийся кодом лексемы, "
                   "либо строковый литерал, являющийся представлением идентификатора или "
                   "ключевого слова.\n", msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            idx = maybe_repres_str_idx;
            state = Colon_sep;
            break;
        case Comma:
            printf("В строке %zu ожидается либо идентификатор, являющийся кодом лексемы, "
                   "либо строковый литерал, являющийся представлением идентификатора или "
                   "ключевого слова.\n", msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Kw_del_comma_sep;
            break;
        default:
            msc->back(); t = false;;
    }
    return t;
}

bool KW_parser::repres_str_proc(){
    bool t = true;
    if(Colon == lc){
        state = Colon_sep;
    }else if(belongs(lc, 1ULL << String | 1ULL << Id | 1ULL << Comma)){
        printf("В строке %zu пропущено двоеточие.\n", msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
        switch(lc){
            case Id:
                add_new_string(idx, li.ident_index);
                state = Code_kw_or_del;
                break;
            case String:
                idx = li.string_index;
                break;
            case Comma:
                state = Kw_del_comma_sep;
                break;
            default:
                ;
        }
    }else{
        msc->back(); t = false;
    }
    return t;
}

bool KW_parser::colon_sep_proc(){
    bool t = true;
    if(Id == lc){
        add_new_string(idx, li.ident_index);
        state = Code_kw_or_del;
    }else if(belongs(lc, 1ULL << String | 1ULL << Colon | 1ULL << Comma)){
        printf("В строке %zu ожидается идентификатор, являющийся кодом лексемы.\n",
               msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
        switch(lc){
            case String:
                state = Repres_str;
                idx = li.string_index;
                break;
            case Colon:
                break;
            case Comma:
                state = Kw_del_comma_sep;
                break;
            default:
                ;
        }
    }else{
        msc->back(); t = false;
    }
    return t;
}

bool KW_parser::code_kw_or_del_proc(){
    bool t = true;
    if(Comma == lc){
        state = Kw_del_comma_sep;
    }else if(belongs(li.code, 1ULL << String | 1ULL << Id | 1ULL << Colon)){
        printf("В строке %zu пропущена запятая.\n", msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
        switch(lc){
            case String:
                state = Repres_str;
                idx = li.string_index;
                break;
            case Id:
                add_new_string(idx, li.ident_index);
                break;
            case Colon:
                state = Colon_sep;
                break;
            default:
                ;
        }
    }else{
        msc->back(); t = false;
    }
    return t;
}

bool KW_parser::kw_del_comma_sep_proc(){
    bool t = true;
    if(String == lc){
        idx = li.string_index;
        state = Repres_str;
        return t;
    }
    if(belongs(lc, 1ULL << Colon | 1ULL << Id | 1ULL << Comma)){
        printf("В строке %zu ожидается строковый литерал, являющийся либо действием "
                "по завершении, либо представлением ключевого слова или разделителя.\n",
                msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
        switch(lc){
            case Colon:
                state = Colon_sep;
                break;
            case Id:
                add_new_string(idx, li.ident_index);
                state = Code_kw_or_del;
                break;
            case Comma:
                break;
            default:
                ;
        }
    }else{
        msc->back(); t = false;
    }
    return t;
}