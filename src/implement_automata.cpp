/*
    Файл:    implement_automata.cpp
    Создан:  03 февраля 2017г. в 13:45 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#include <string>
#include "../include/implement_automata.h"
#include "../include/generate_delim_automaton_impl.h"
#include "../include/generate_strings_automaton_impl.h"
#include "../include/generate_numbers_automaton_impl.h"
#include "../include/generate_idents_and_keywords_automata_impl.h"
#include "../include/belongs.h"
#include "../include/automaton_with_procs.h"

static const std::string none_proc = R"~(::none_proc(){
    /* Данная подпрограмма будет вызвана, если по прочтении входного текста оказались
     * в автомате A_start. Тогда ничего делать не нужно. */
})~";

static void generate_unknown_automata_impl(Info_for_constructing& info){
    info.aut_impl[Unknown_aut] = "bool " + info.name_of_scaner_class + R"~(::unknown_proc(){
    return belongs(Other, char_categories);
})~";

    info.aut_impl_fin_proc[Unknown_aut] = "void " + info.name_of_scaner_class + R"~(::unknown_final_proc(){
    /* Данная подпрограмма будет вызвана, если по прочтении входного текста
     * оказались в автомате A_unknown. Тогда ничего делать не нужно. */
})~";
}

static void generate_automata_impl(Info_for_constructing& info){
    info.aut_impl_fin_proc[Start_aut] = "void " + info.name_of_scaner_class + none_proc;
    generate_delim_automaton_impl(info);
    generate_strings_automaton_impl(info);
    generate_numbers_automaton_impl(info);
    generate_idents_and_keywords_automata_impl(info);
    generate_unknown_automata_impl(info);

}

static void prepare_automata_info(Info_for_constructing& info){
    info.set_of_used_automata |= (1ULL << Start_aut) | (1ULL << Unknown_aut);
    bool t = belongs(Comment_aut, info.set_of_used_automata) != 0;
    info.set_of_used_automata &= ~(1ULL << Comment_aut);
    for(int a = Start_aut; a <= Comment_aut; a++){
        Automaton_with_procs ap;
        if(belongs(a, info.set_of_used_automata)){
            ap.name           = info.possible_automata_name_str[a];
            ap.proc_proto     = info.possible_automata_proc_proto[a];
            ap.fin_proc_proto = info.possible_automata_final_proc_proto[a];
            ap.proc_ptr       = "&" + info.name_of_scaner_class + "::" + info.possible_proc_ptr[a];
            ap.fin_proc_ptr   = "&" + info.name_of_scaner_class + "::" +
                                info.possible_fin_proc_ptr[a];
            info.automaton_info.push_back(ap);
        }
    }
    if(t){
        info.set_of_used_automata |= 1ULL << Comment_aut;
    }
}

void implement_automata(Info_for_constructing& info){
    generate_automata_impl(info);
    prepare_automata_info(info);
}