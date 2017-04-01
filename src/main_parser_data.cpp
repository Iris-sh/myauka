/*
    File:    main_parser_data.cpp
    Created: 14 декабря 2015г. в 15:25 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/main_parser_data.h"
#include "../include/idx_to_string.h"

const char32_t* none_string    = U"None";
const char32_t* unknown_string = U"Unknown";
const char32_t* write_act_name = U"write";
const char32_t* write_act_body = U"buffer += ch;";

Main_parser_data::Main_parser_data(){
    et_.ec        = std::make_shared<Error_count>();
    et_.ids_trie  = std::make_shared<Char_trie>();
    et_.strs_trie = std::make_shared<Char_trie>();
    scope_        = std::make_shared<Scope>();
    scaner_name_idx = codes_type_name_idx = ident_name_idx =
                      token_fields_idx = class_members_idx = 0;
    newline_is_lexem = false;

    codes                = std::vector<size_t>();
    kw_repres            = std::vector<size_t>();
    del_repres           = std::vector<size_t>();
    id_begin             = std::vector<Command>();
    id_body              = std::vector<Command>();
    numbers_regexp       = std::vector<Command>();
    strings_regexp       = std::vector<Command>();
//     aut_name_with_lens   = std::vector<Used_automaton_name_with_lengths>();

    set_of_used_automata = 0;

    name_of_scaner_class         = "";
    scaner_file_name_without_ext = "";
    codes_type_name              = "";
    lexem_info_name              = "";

    Id_attributes iattr;

    iattr.kind      = Code_of_lexem;

    size_t idx = et_.ids_trie->insert(none_string);

    iattr.code = 0; codes.push_back(idx);
    scope_->idsc[idx] = iattr;

    idx = et_.ids_trie->insert(unknown_string);
    iattr.code = 1; codes.push_back(idx);
    scope_->idsc[idx] = iattr;

    last_code_val = 1;

    multilined_is_nested = false;
    mark_of_single_lined = mark_of_multilined_begin = mark_of_multilined_end = 0;

    for(Category& cat : begin_chars){
        cat = Category();
    }

    char_cat           = Trie_for_set_of_char();
    // categories_indeces = std::set<size_t>();

    aut_impl          = std::map<Used_automaton, std::string>();
    aut_impl_fin_proc = std::map<Used_automaton, std::string>();

    category_name=std::map<size_t, std::string>();
    last_category_suffix=0;

    delimiter_strings = std::vector<std::u32string>();
    keyword_strings   = std::vector<std::u32string>();

//     start_proc_impl = "";
    automaton_info = std::vector<Automaton_with_procs>();

    there_is_Elem_definition = false;

    iattr.kind              = Action_name;
    idx                     = et_.ids_trie -> insert(write_act_name);
    write_action_name_idx   = idx;
//     printf("Index for $write: %zu.\n", idx);
    size_t body_idx         = et_.strs_trie-> insert(write_act_body);
    iattr.act_string        = body_idx;
    scope_->idsc[idx]       = iattr;

    Str_attributes sattr;
    sattr.kind              = Action_definition;
    sattr.code              = 0;
    scope_->strsc[body_idx] = sattr;

    fields_for_comments_handling = "";
}

void Main_parser_data::init_end(Location_ptr loc){
    msc                   = std::make_shared<Main_scaner>(loc, et_);
    expr_sc               = std::make_shared<Expr_scaner>(loc, et_);
    id_definition_parser  = std::make_shared<Simple_regex_parser>(expr_sc, et_);
    num_and_str_parser    = std::make_shared<Act_expr_parser>(expr_sc, et_, scope_);
    codes_and_newline     = std::make_shared<Codes_and_newline>(scope_, et_, msc);
    keywords_sec_parser   = std::make_shared<Keyword_parser>(scope_, et_, msc);
    delimiters_sec_parser = std::make_shared<Delimiter_parser>(scope_, et_, msc);
    num_sec_parser        = std::make_shared<Number_parser>(scope_, et_, msc, num_and_str_parser);
    strs_sec_parser       = std::make_shared<String_parser>(scope_, et_, msc, num_and_str_parser);
    comments_parser       = std::make_shared<Comments_parser>(et_, msc);
}

/* Данная функция на вход получает индекс имени действия, i, в префиксном дереве
 * идентификаторов, а в качестве результата выдаёт строковое представление
 * определения действия. */
std::string Main_parser_data::get_act_repres(size_t i){
    /* Непустая строка возвращается только в том случае, когда действие
     * с таким индексом имени есть. */
    std::string result;
//     printf("action index is = %zu\n", i);
    if(!i){
        return result;
    }
    auto isc = scope_->idsc;
    auto iti = isc.find(i);
    if(iti != isc.end()){
        size_t act_str_idx = iti->second.act_string;
        result = idx_to_string(et_.strs_trie, act_str_idx);
    }
    return result;
}

/*
 * Добавляет в ассоциативный массив std::map<size_t, std::string> category_name
 * категорию (множество) символов с именем default_name. Если такого множества ещё
 * нет, то добавляем с таким названием, и возвращаем его и true. Если же есть, то
 * возвращаем имеющееся название и false.
*/
std::pair<bool, std::string> Main_parser_data::add_category(const std::set<char32_t>& s,
                                                            const std::string& default_name){
    std::string result_str;
    bool        result_bool;

    size_t i = char_cat.insertSet(s);
    auto it  = category_name.find(i);
    if(it != category_name.end()){
        result_str  = it->second;
        result_bool = false;
    }else{
        category_name[i] = default_name;
        result_str       = default_name;
        result_bool      = true;
    }

    auto result = std::make_pair(result_bool, result_str);
    return result;
}