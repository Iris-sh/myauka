/*
    File:    num_str_parser.cpp
    Created: 06 ноября 2016г. в 12:20 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/num_str_parser.h"
#include "../include/belongs.h"
#include <tuple>

NS_settings String_parser::settings(){
    return std::make_pair(Kw_strings, String_expr);
}

NS_settings Number_parser::settings(){
    return std::make_pair(Kw_numbers, Number_expr);
}

void NS_parser::add_action_definition(size_t action_id, size_t action_def_idx){
    auto& isc   = scope_->idsc;
    auto& ssc   = scope_->strsc;
    auto  id_it = isc.find(action_id);
    auto  s_it  = ssc.find(action_def_idx);

    if( (id_it != isc.end()) && ((id_it->second).kind != Action_name) ){
        printf("Ошибка в строке %zu: идентификатор ", msc->lexem_begin_line_number());
        et_.ids_trie->print(action_id);
        printf(" уже определён, но не как имя действия.\n");
        et_.ec->increment_number_of_errors();
        return;
    }

    if( (s_it != ssc.end()) && ((s_it->second).kind != Action_definition) ){
        printf("Ошибка в строке %zu: строковый литерал ", msc->lexem_begin_line_number());
        et_.strs_trie->print(action_def_idx);
        printf(" уже есть, но не как определение действия.\n");
        et_.ec->increment_number_of_errors();
        return;
    }

    Id_attributes a;
    a.kind                        = Action_name;
    a.act_string                  = action_def_idx;
    scope_->idsc[action_id]       = a;

    Str_attributes sa;
    sa.kind                       = Action_definition;
    sa.code                       = 0;
    scope_->strsc[action_def_idx] = sa;
}

void NS_parser::compile(Command_buffer& buf, Init_and_final_acts& acts){
    buf_.clear();
    std::tie(sec_begin, kind) = settings();
    bool t = begin_of_num_or_str_sec();
    if(!t){
        return;
    }
    a_parser_->compile(buf_, kind);
    buf  = buf_;
    acts = acts_;
}

bool NS_parser::begin_of_num_or_str_sec(){
    lc = (li = msc->current_lexem()).code;
    bool t = false;
    if(!lc){
        /* Если текст с описанием сканера закончился, то далее
           секцию обрабатывать не нужно. */
        msc->back();
        return t;
    }

    msc->back();

    acts_.init_acts = acts_.fin_acts = 0;
    state = Num_str_kw;
    t     = true;
    while((lc = (li = msc->current_lexem()).code)){
        t = (this->*procs[state])();
        if(!t){
            return t;
        }
        if(Act_expr_beg == state){
            return t;
        }
    }

    if(state != Act_expr_beg){
        printf("Неожиданный конец шапки раздела в строке %zu.\n",
               msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
    }
    msc->back();
    return t;
}

NS_parser::State_proc NS_parser::procs[] = {
    &NS_parser::num_str_kw_proc,   &NS_parser::maybe_init_acts_proc,
    &NS_parser::init_acts_proc,    &NS_parser::maybe_final_acts_proc,
    &NS_parser::final_acts_proc,   &NS_parser::action_sec_proc,
    &NS_parser::act_expr_beg_proc, &NS_parser::act_def_proc
};

bool NS_parser::num_str_kw_proc(){
    bool t = true;
    if(sec_begin == lc){
        state = Maybe_init_acts;
        return t;
    }
    t = false;
    if(belongs(lc, (1ULL << Kw_numbers) | (1ULL << Kw_strings))){
        msc->back();
        return t;
    }
    msc->back();
    printf("В строке %zu пропущено %s.\n", msc->lexem_begin_line_number(),
           (Kw_strings == sec_begin) ? "%strings" : "%numbers");
    et_.ec -> increment_number_of_errors();
    switch(lc){
        case String:
            state = Init_acts;
            acts_.init_acts = li.string_index;
            break;

        case Kw_action:
            state = Action_sec;
            break;

        case Id:
            printf("В строке %zu пропущено слово %%action.\n",
                msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            state = Maybe_final_acts;
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            break;

        default:
            msc->back();
    }
    return t;
}

bool NS_parser::maybe_init_acts_proc(){
    bool t = true;
    if(sec_begin == lc){
        printf("В строке %zu неожиданно обнаружено слово %s.\n",
               msc->lexem_begin_line_number(),
               (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            state = Init_acts;
            acts_.init_acts = li.string_index;
            break;

        case Kw_action:
            state = Action_sec;
            printf("В строке %zu неожиданно встретилось слово %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            break;

        case Id:
            printf("В строке %zu пропущено слово %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            state = Maybe_final_acts;
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            printf("В строке %zu неожиданно встретилась {.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}

bool NS_parser::init_acts_proc(){
    bool t = true;
    if(sec_begin == lc){
        printf("В строке %zu неожиданно обнаружено слово %s.\n",
               msc->lexem_begin_line_number(),
               (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            acts_.fin_acts = li.string_index;
            state = Final_acts;
            printf("В строке %zu пропущено двоеточие.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_action:
            state = Action_sec;
            printf("В строке %zu неожиданно встретилось слово %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            break;

        case Id:
            printf("В строке %zu пропущено слово %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            state = Maybe_final_acts;
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            printf("В строке %zu неожиданно встретилась {.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}

bool NS_parser::maybe_final_acts_proc(){
    bool t = true;
    if(sec_begin == lc){
        printf("В строке %zu неожиданно обнаружено слово %s.\n",
               msc->lexem_begin_line_number(),
               (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            acts_.fin_acts = li.string_index;
            state = Final_acts;
            break;

        case Kw_action:
            state = Action_sec;
            break;

        case Id:
            printf("В строке %zu пропущено слово %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            printf("В строке %zu неожиданно встретилось двоеточие.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}

bool NS_parser::final_acts_proc(){
    bool t = true;
    if(sec_begin == lc){
        printf("В строке %zu неожиданно обнаружено слово %s.\n",
               msc->lexem_begin_line_number(),
              (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            printf("В строке %zu неожиданно встретился строковый литерал с "
                   "определением действия.\n", msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_action:
            state = Action_sec;
            break;

        case Id:
            printf("В строке %zu пропущено слово %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            printf("В строке %zu неожиданно встретилось двоеточие.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}

bool NS_parser::action_sec_proc(){
    bool t = true;
    if(sec_begin == lc){
        printf("В строке %zu неожиданно обнаружено слово %s.\n",
               msc->lexem_begin_line_number(),
               (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            printf("В строке %zu неожиданно встретился строковый литерал с "
                   "определением действия.\n", msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Final_acts;
            break;

        case Kw_action:
            printf("В строке %zu неожиданно встретилось %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Id:
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            printf("В строке %zu неожиданно встретилось двоеточие.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            printf("В строке %zu неожиданно встретилась {.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}

bool NS_parser::act_expr_beg_proc(){
    bool t = false;
    msc->back();
    return t;
}

bool NS_parser::act_def_proc(){
    bool t = true;
    if(sec_begin == lc){
        printf("В строке %zu неожиданно обнаружено слово %s.\n",
               msc->lexem_begin_line_number(),
               (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            add_action_definition(current_action_name_idx, li.string_index);
            state = Final_acts;
            break;

        case Kw_action:
            printf("В строке %zu неожиданно встретилось %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Id:
            printf("В строке %zu неожиданно встретился идентификатор.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Colon:
            printf("В строке %zu неожиданно встретилось двоеточие.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            printf("В строке %zu неожиданно встретилась {.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}