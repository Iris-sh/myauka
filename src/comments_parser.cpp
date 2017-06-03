/*
    File:    comments_parser.cpp
    Created: 06 November 2016 at 12:20 Moscow time
    Author:  Гаврилов Владимир Сергеевич
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
        /* If the text with the description of the scanner is over, then there is
         * no comment processing section, and, therefore, this section should
         * not be processed. */
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
        printf("The section describing comments unexpectedly ended in line %zu.\n",
               msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
    }
}

Comments_parser_result Comments_parser::compile(){
    Comments_parser_result result;
    compile_();
    result = std::make_tuple(mark_of_single_lined, mark_of_multilined_begin,
                             mark_of_multilined_end, multilined_is_nested);
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
             printf("At line %zu missing word %%comments.\n",
                 msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             state = Single_lined;
             break;

         case String:
             printf("At line %zu missing words %%comments and %%single_lined.\n",
                 msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             state = Single_lined_mark;
             mark_of_single_lined = li.string_index;
             break;

         case Kw_multilined:
             printf("At line %zu missing word %%comments.\n",
                 msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             state = Multilined;
             break;

         case Kw_nested:
             printf("At line %zu missing words %%comments and %%multilined.\n",
                 msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             state = Nested;
             multilined_is_nested = true;
             break;

         case Colon:
             printf("At line %zu skipped words %%comments and %%multilined, and "
                    "a description of the beginning of the multiline comment.\n",
                    msc->lexem_begin_line_number());
             et_.ec -> increment_number_of_errors();
             state = Beg_end_delim;
             break;

         default:
             printf("At line %zu received an unexpected kind of lexeme.\n",
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
            printf("At line %zu unexpectedly met the word %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            state = Single_lined;
            break;

        case String:
            printf("At line %zu missing words %%comments and %%single_lined.\n",
                msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Single_lined_mark;
            mark_of_single_lined = li.string_index;
            break;

        case Kw_multilined:
            state = Multilined;
            break;

        case Kw_nested:
            printf("At line %zu missing word %%multilined.\n",
                msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Nested;
            multilined_is_nested = true;
            break;

        case Colon:
            printf("At line %zu missing word %%multilined, and a description of "
                   "the beginning of the multiline comment.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Beg_end_delim;
            break;

        default:
            printf("At line %zu received an unexpected kind of lexeme.\n",
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
            printf("At line %zu unexpectedly met the word %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            printf("At line %zu unexpectedly met the word %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            state = Single_lined_mark;
            mark_of_single_lined = li.string_index;
            break;

        case Kw_multilined:
            printf("At line %zu unexpectedly met the word %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Multilined;
            break;

        case Kw_nested:
            printf("At line %zu missing word %%multilined.\n",
                msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Nested;
            multilined_is_nested = true;
            break;

        case Colon:
            printf("At line %zu missing word %%multilined, and a description of "
                   "the beginning of the multiline comment.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Beg_end_delim;
            break;

        default:
            printf("At line %zu received an unexpected kind of lexeme.\n",
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
            printf("At line %zu unexpectedly met the word %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            printf("At line %zu unexpectedly met the word %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            mark_of_multilined_begin = li.string_index;
            state = Multi_lined_begin;
            break;

        case Kw_multilined:
            printf("At line %zu unexpectedly met the word %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_nested:
            state = Nested;
            multilined_is_nested = true;
            break;

        case Colon:
            printf("At line %zu missising a description of "
                   "the multilined comment begin.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Beg_end_delim;
            break;

        default:
            printf("At line %zu received an unexpected kind of lexeme.\n",
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
            printf("At line %zu unexpectedly met the word %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            printf("At line %zu unexpectedly met the word %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            printf("At line %zu missing word %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Multi_lined_begin;
            mark_of_multilined_begin = li.string_index;
            break;

        case Kw_multilined:
            state = Multilined;
            break;

        case Kw_nested:
            printf("At line %zu missing word %%multilined.\n",
                msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Nested;
            multilined_is_nested = true;
            break;

        case Colon:
            printf("At line %zu missising a description of "
                   "the multilined comment begin.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Beg_end_delim;
            break;

        default:
            printf("At line %zu received an unexpected kind of lexeme.\n",
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
            printf("At line %zu unexpectedly met the word %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
           break;

        case Kw_single_lined:
            printf("At line %zu unexpectedly met the word %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            printf("At line %zu missing colon.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Multilined_end;
            mark_of_multilined_end = li.string_index;
            break;

        case Kw_multilined:
            printf("At line %zu unexpectedly met the word %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_nested:
            printf("At line %zu unexpectedly met the word %%nested.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Colon:
            state = Beg_end_delim;
            break;

        default:
            printf("At line %zu received an unexpected kind of lexeme.\n",
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
            printf("At line %zu unexpectedly met the word %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            printf("At line %zu unexpectedly met the word %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            state = Multi_lined_begin;
            mark_of_multilined_begin = li.string_index;
            break;

        case Kw_multilined:
            printf("At line %zu unexpectedly met the word %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_nested:
            printf("At line %zu unexpectedly met the word %%nested.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Colon:
            printf("At line %zu missising a description of the "
                   "multilined comment begin.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Beg_end_delim;
            break;

        default:
            printf("At line %zu received an unexpected kind of lexeme.\n",
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
            printf("At line %zu unexpectedly met the word %%comments.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_single_lined:
            printf("At line %zu unexpectedly met the word %%single_lined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case String:
            mark_of_multilined_end = li.string_index;
            state = Multilined_end;
            break;

        case Kw_multilined:
            printf("At line %zu unexpectedly met the word %%multilined.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_nested:
            printf("At line %zu unexpectedly met the word %%nested.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Colon:
            printf("At line %zu unexpectedly met colon.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        default:
            printf("At line %zu received an unexpected kind of lexeme.\n",
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