/*
    File:    categories.cpp
    Created: 10 January 2015г. в 10:47 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include <map>
#include <utility>
#include "../include/categories.h"
#include "../include/operations_with_sets.h"
#include "../include/sets_for_classes.h"
#include "../include/char_conv.h"    // для отладочной печати
#include "../include/print_char32.h" // для отладочной печати

using operations_with_sets::operator+;
using operations_with_sets::operator*;
using operations_with_sets::is_elem;
using operations_with_sets::is_subseteq;

using operations_with_sets::print_set; // для отладочной печати

const char* category_kind_as_str[] = {
    "All_chars", "Not_single_quote", "Not_double_quote", "Set_of_cs"
};

void print_category(const Category& c){
    auto k = c.kind;
    printf("%s", category_kind_as_str[k]);
    if(k == Set_of_cs){
        putchar(' ');
        print_set(c.s, print_char32);
    }
}

Category gc2category(const Generalized_char& gc){
    Category categ;
    Kind_of_char k = gc.kind;
    switch(k){
        case Char:
            categ.kind = Set_of_cs;
            categ.s = Set_of_char({gc.c});
            break;

        case Char_class:
            switch(gc.cls){
                case G_all:
                    categ.kind = All_chars;
                    categ.s    = Set_of_char();
                    break;

                case G_ndq:
                    categ.kind = Not_double_quote;
                    categ.s    = Set_of_char();
                    break;

                case G_nsq:
                    categ.kind = Not_single_quote;
                    categ.s    = Set_of_char();
                    break;

                default:
                    categ.kind = Set_of_cs;
                    categ.s    = sets_for_char_classes[gc.cls];
            }
            break;

        default:
            ;
    }
    return categ;
}

/* Операцию сравнения категорий символов на равенство определим так.
   1) Если c1.kind != c1.kind, то категории не равны.
   2) Если (c1.kind == c2.kind) && (c1.kind != Set_of_cs), то категории равны.
   3) В остальных случаях категории равны тогда и только тогда, когда
      (c1.s == c2.s).
*/
bool operator == (const Category& c1, const Category& c2){
    return (c1.kind == c2.kind) && ((c1.kind != Set_of_cs) || (c1.s == c2.s));
}

/* Отношение порядка (<) на множестве категорий символов определим следующим
   образом.
   1) Если (c1.kind != c2.kind), то c1 < c2 эквивалентно (c1.kind < c2.kind).
   2) Если (c1.kind == c2.kind) && (c1.kind != Set_of_cs), то неверно, что c1 < c2.
   3) В остальных случаях c1 < c2 тогда и только тогда, когда (c1.s < c2.s)
      (в смысле STL-контейнера std::set).
*/
bool operator < (const Category& c1, const Category& c2){
    if(c1.kind != c2.kind){
        return c1.kind < c2.kind;
    }else{
        return (c1.kind == Set_of_cs) && (c1.s < c2.s);
    }
}

const std::map<std::pair<Category_kind, Category_kind>, Category_kind> addition_table = {
    {{All_chars, All_chars}, All_chars},
    {{All_chars, Not_single_quote}, All_chars},
    {{All_chars, Not_double_quote}, All_chars},
    {{All_chars, Set_of_cs}, All_chars},
    {{Not_single_quote, All_chars}, All_chars},
    {{Not_double_quote, All_chars}, All_chars},
    {{Set_of_cs, All_chars}, All_chars},
    {{Not_single_quote, Not_single_quote}, Not_single_quote},
    {{Not_double_quote, Not_double_quote}, Not_double_quote},
    {{Not_single_quote, Not_double_quote}, All_chars},
    {{Not_double_quote, Not_single_quote}, All_chars}
};

Category operator + (const Category& c1, const Category& c2){
    Category result;
    Category_kind k1 = c1.kind;
    Category_kind k2 = c2.kind;
    auto ait = addition_table.find(std::make_pair(k1, k2));
    if(ait != addition_table.end()){
        result.kind = ait->second;
        result.s    = Set_of_char();
    }else{
        enum Branch{
            Not_set_and_set, Set_and_not_set, Set_and_set
        };
        Branch b = static_cast<Branch>((Set_of_cs == k1) * 2 + (Set_of_cs == k2) - 1);
        char special_char;
        Set_of_char::iterator si;
        switch(b){
            case Not_set_and_set:
                special_char = (Not_single_quote == k1) ? '\'' : '\"';
                si = c2.s.find(special_char);
                if(si != c2.s.end()){
                    result.kind = All_chars;
                    result.s    = Set_of_char();
                }else{
                    result.kind = c1.kind;
                    result.s    = Set_of_char();
                }
                break;

            case Set_and_not_set:
                special_char = (Not_single_quote == k2) ? '\'' : '\"';
                si = c1.s.find(special_char);
                if(si != c1.s.end()){
                    result.kind = All_chars;
                    result.s    = Set_of_char();
                }else{
                    result.kind = c2.kind;
                    result.s    = Set_of_char();
                }
                break;

            case Set_and_set:
                result.kind = Set_of_cs;
                result.s    = c1.s + c2.s;
                break;
        }
    }
    return result;
}

bool operator * (const Category& c1, const Category& c2){
    bool t = true;
    Category_kind k1 = c1.kind;
    Category_kind k2 = c2.kind;

    if((k1 < Set_of_cs) && (k2 < Set_of_cs)){
        return t;
    }
    if((Set_of_cs == k1) && (All_chars == k2)){
        return !(c1.s.empty());
    }
    if((All_chars == k1) && (Set_of_cs == k2)){
        return !(c2.s.empty());
    }
    enum Branch{
        Not_set_and_set, Set_and_not_set, Set_and_set
    };
    Branch b = static_cast<Branch>((Set_of_cs == k1) * 2 + (Set_of_cs == k2) - 1);
    char32_t special_char;
    switch(b){
        case Not_set_and_set:
            special_char = (Not_single_quote == k1) ? U'\'' : U'\"';
            t = c2.s != Set_of_char({special_char});
            break;

        case Set_and_not_set:
            special_char = (Not_single_quote == k2) ? U'\'' : U'\"';
            t = c1.s != Set_of_char({special_char});
            break;

        case Set_and_set:
            t = !((c1.s * c2.s).empty());
            break;
    }
    return t;
}

const std::map<std::pair<Category_kind, Category_kind>, bool> subcategory_table = {
    {{All_chars, All_chars}, true},                {{All_chars, Not_single_quote}, false},
    {{All_chars, Not_double_quote}, false},        {{All_chars, Set_of_cs}, false},
    {{Not_single_quote, All_chars}, true},         {{Not_single_quote, Not_single_quote}, true},
    {{Not_single_quote, Not_double_quote}, false}, {{Not_single_quote, Set_of_cs}, false},
    {{Not_double_quote, All_chars}, true},         {{Not_double_quote, Not_single_quote}, false},
    {{Not_double_quote, Not_double_quote}, true},  {{Not_double_quote, Set_of_cs}, false},
    {{Set_of_cs, All_chars}, true}
};

bool is_subcategory(const Category& c1, const Category& c2){
    bool t = true;
    Category_kind k1 = c1.kind;
    Category_kind k2 = c2.kind;

    auto it = subcategory_table.find(std::make_pair(k1, k2));
    if(it != subcategory_table.end()){
        t = it->second;
    }else{
        if (Set_of_cs == k2){
            t = is_subseteq(c1.s, c2.s);
        }else{
            char special_char = (Not_single_quote == k2) ? '\'' : '\"';
            t = c1.s.find(special_char) == c1.s.end();
        }
    }
    return t;
}