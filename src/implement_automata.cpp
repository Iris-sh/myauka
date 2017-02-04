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

static const std::string none_proc = R"~(::none_proc(){
    /* Данная подпрограмма будет вызвана, если по прочтении входного текста оказались
     * в автомате A_start. Тогда ничего делать не нужно. */
})~";

// static void generate_strings_automaton_impl(Info_for_constructing& info){}

static void generate_numbers_automaton_impl(Info_for_constructing& info){}

static void generate_idents_and_keywords_automata_impl(Info_for_constructing& info){}

static void generate_unknown_automata_impl(Info_for_constructing& info){}

static void generate_automata_impl(Info_for_constructing& info){
    info.aut_impl_fin_proc[Start_aut] = "void " + info.name_of_scaner_class + none_proc;
    generate_delim_automaton_impl(info);
    generate_strings_automaton_impl(info);
    generate_numbers_automaton_impl(info);
    generate_idents_and_keywords_automata_impl(info);
    generate_unknown_automata_impl(info);

}

static void prepare_automata_info(Info_for_constructing& info){}

void implement_automata(Info_for_constructing& info){
    generate_automata_impl(info);
    prepare_automata_info(info);
}