/*
    Файл:    comments_parser.cpp
    Создан:  06 ноября 2016г. в 12:20 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/comments_parser.h"
#include "../include/belongs.h"

void Comments_parser::compile_(){
    multilined_is_nested = false;
    mark_of_single_lined = mark_of_multilined_begin = mark_of_multilined_end = 0;

    lc = (li = msc->current_lexem()).code;
    if(!lc){
        /* Если текст с описанием сканера закончился, то и секции обработки комментариев
           нет, и, значит, эту секцию обрабатывать не нужно. */
        msc->back();
        return;
    }

    msc->back();

    state = Comments_kw;
    while((lc = (li = msc->current_lexem()).code)){
        bool t = (this->*procs[state])();
        if(!t){
            msc->back();
            return;
        }
    }

    if(!belongs(state, 1ULL << Single_lined_mark | 1ULL << Multilined_end)){
        printf("В строке %zu неожиданно закончилась секция описания комментариев.\n",
               msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
    }
}

Comments_parser_result Comments_parser::compile(){
    Comments_parser_result result;

    compile_();
    result = std::make_tuple(mark_of_single_lined, mark_of_multilined_begin,
                             mark_of_multilined_end, multilined_is_nested);
//     result.mark_of_single_lined     = mark_of_single_lined;
//     result.mark_of_multilined_begin = mark_of_multilined_begin;
//     result.mark_of_multilined_end   = mark_of_multilined_end;
//     result.multilined_is_nested     = multilined_is_nested;

    return result;
}

Comments_parser::State_proc Comments_parser::procs[] = {
    &Comments_parser::comments_kw_proc,       &Comments_parser::single_or_multilined_proc,
    &Comments_parser::single_lined_proc,      &Comments_parser::multilined_proc,
    &Comments_parser::single_lined_mark_proc, &Comments_parser::multi_lined_begin_proc,
    &Comments_parser::nested_proc,            &Comments_parser::beg_end_delim_proc,
    &Comments_parser::multilined_end_proc
};

 bool Comments_parser::comments_kw_proc(){
     bool t = true;
     switch(lc){
         case Kw_comments:
             state = Single_or_multiLined;
             break;

         case Kw_single_lined:
             printf("В строке %zu пропущено слово %%comments.\n",
                 msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             state = Single_lined;
             break;

         case String:
             printf("В строке %zu пропущены слова %%comments и %%single_lined.\n",
                 msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             state = Single_lined_mark;
             mark_of_single_lined = li.string_index;
             break;

         case Kw_multilined:
             printf("В строке %zu пропущено слово %%comments.\n",
                 msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             state = Multilined;
             break;

         case Kw_nested:
             printf("В строке %zu пропущены слова %%comments и %%multilined.\n",
                 msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             state = Nested;
             multilined_is_nested = true;
             break;

         case Colon:
             printf("В строке %zu пропущены слова %%comments и %%multilined, а также "
                    "описание начала многострочного комментария.\n",
                    msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             state = Beg_end_delim;
             break;

         default:
             printf("В строке %zu встречен неожиданный вид лексемы.\n",
                    msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             t = false;
     }
     return t;
 }

bool Comments_parser::single_or_multilined_proc(){
    bool t = true;
    switch(lc){
        case Kw_comments:
            printf("В строке %zu неожиданно встретилось слово %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            state = Single_lined;
            break;

        case String:
            printf("В строке %zu пропущены слова %%comments и %%single_lined.\n",
                msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Single_lined_mark;
            mark_of_single_lined = li.string_index;
            break;

        case Kw_multilined:
            state = Multilined;
            break;

        case Kw_nested:
            printf("В строке %zu пропущено слово %%multilined.\n",
                msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Nested;
            multilined_is_nested = true;
            break;

        case Colon:
            printf("В строке %zu пропущено слово %%multilined, а также "
                   "описание начала многострочного комментария.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Beg_end_delim;
            break;

        default:
            printf("В строке %zu встречен неожиданный вид лексемы.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            t = false;
    }
    return t;
}

bool Comments_parser::single_lined_proc(){
    bool t = true;
    switch(lc){
        case Kw_comments:
            printf("В строке %zu неожиданно встретилось слово %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            printf("В строке %zu неожиданно встретилось слово %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            state = Single_lined_mark;
            mark_of_single_lined = li.string_index;
            break;

        case Kw_multilined:
            printf("В строке %zu неожиданно встретилось слово %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Multilined;
            break;

        case Kw_nested:
            printf("В строке %zu пропущено слово %%multilined.\n",
                msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Nested;
            multilined_is_nested = true;
            break;

        case Colon:
            printf("В строке %zu пропущено слово %%multilined, а также "
                   "описание начала многострочного комментария.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Beg_end_delim;
            break;

        default:
            printf("В строке %zu встречен неожиданный вид лексемы.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            t = false;
    }
    return t;
}

bool Comments_parser::multilined_proc(){
    bool t = true;
    switch(lc){
        case Kw_comments:
            printf("В строке %zu неожиданно встретилось слово %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            printf("В строке %zu неожиданно встретилось слово %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            mark_of_multilined_begin = li.string_index;
            state = Multi_lined_begin;
            break;

        case Kw_multilined:
            printf("В строке %zu неожиданно встретилось слово %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_nested:
            state = Nested;
            multilined_is_nested = true;
            break;

        case Colon:
            printf("В строке %zu пропущено слово описание начала "
                   "многострочного комментария.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Beg_end_delim;
            break;

        default:
            printf("В строке %zu встречен неожиданный вид лексемы.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            t = false;
    }
    return t;
}

bool Comments_parser::single_lined_mark_proc(){
    bool t = true;
    switch(lc){
        case Kw_comments:
            printf("В строке %zu неожиданно встретилось слово %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            printf("В строке %zu неожиданно встретилось слово %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            printf("В строке %zu пропущено слово %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Multi_lined_begin;
            mark_of_multilined_begin = li.string_index;
            break;

        case Kw_multilined:
            state = Multilined;
            break;

        case Kw_nested:
            printf("В строке %zu пропущено слово %%multilined.\n",
                msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Nested;
            multilined_is_nested = true;
            break;

        case Colon:
            printf("В строке %zu пропущено слово описание начала "
                   "многострочного комментария.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Beg_end_delim;
            break;

        default:
            printf("В строке %zu встречен неожиданный вид лексемы.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            t = false;
    }
    return t;
}

bool Comments_parser::multi_lined_begin_proc(){
    bool t = true;
    switch(lc){
        case Kw_comments:
            printf("В строке %zu неожиданно встретилось слово %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
           break;

        case Kw_single_lined:
            printf("В строке %zu неожиданно встретилось слово %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            printf("В строке %zu пропущено двоеточие.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Multilined_end;
            mark_of_multilined_end = li.string_index;
            break;

        case Kw_multilined:
            printf("В строке %zu неожиданно встретилось слово %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_nested:
            printf("В строке %zu неожиданно встретилось слово %%nested.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Colon:
            state = Beg_end_delim;
            break;

        default:
            printf("В строке %zu встречен неожиданный вид лексемы.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            t = false;
    }
    return t;
}

bool Comments_parser::nested_proc(){
    bool t = true;
    switch(lc){
        case Kw_comments:
            printf("В строке %zu неожиданно встретилось слово %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            printf("В строке %zu неожиданно встретилось слово %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            state = Multi_lined_begin;
            mark_of_multilined_begin = li.string_index;
            break;

        case Kw_multilined:
            printf("В строке %zu неожиданно встретилось слово %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_nested:
            printf("В строке %zu неожиданно встретилось слово %%nested.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Colon:
            printf("В строке %zu неожиданно пропущено описание начала "
                   "многострочного комментария.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Beg_end_delim;
            break;

        default:
            printf("В строке %zu встречен неожиданный вид лексемы.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            t = false;
    }
    return t;
}

bool Comments_parser::beg_end_delim_proc(){
    bool t = true;
    switch(lc){
        case Kw_comments:
            printf("В строке %zu неожиданно встретилось слово %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            printf("В строке %zu неожиданно встретилось слово %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            mark_of_multilined_end = li.string_index;
            state = Multilined_end;
            break;

        case Kw_multilined:
            printf("В строке %zu неожиданно встретилось слово %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_nested:
            printf("В строке %zu неожиданно встретилось слово %%nested.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Colon:
            printf("В строке %zu неожиданно встретилось слово двоеточие.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        default:
            printf("В строке %zu встречен неожиданный вид лексемы.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            t = false;
    }
    return t;
}

bool Comments_parser::multilined_end_proc(){
    bool t;
    t = false;
    return t;
}