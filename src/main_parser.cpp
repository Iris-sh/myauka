/*
    File:    main_parser.cpp
    Created: 14 December 2015 at 15:25 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include <cctype>
#include <algorithm>
#include <vector>
#include "../include/main_parser.h"
#include "../include/aux_files_generate.h"
#include "../include/idx_to_string.h"
#include "../include/belongs.h"
#include "../include/trie.h"
#include "../include/scope.h"
#include "../include/ndfa.h"
#include "../include/dfa.h"
#include "../include/first_chars.h"
#include "../include/groupped_dfa.h"
#include "../include/categories.h"
#include "../include/attributed_char_trie.h"
#include "../include/char_conv.h"
#include "../include/list_to_columns.h"
#include "../include/char_trie.h"
#include "../include/operations_with_sets.h"
#include "../include/unwrapped_command.h"
#include "../include/info_for_constructing.h"
#include "../include/unwrap_commands.h"
#include "../include/conv_case.h"
#include "../include/implement_automata.h"
#include "../include/implement_scaner.h"
#include "../include/scaner_header.h"
#include "../include/print_char32.h" // для отладочной печати

static const char32_t* default_names[] = {
    U"Scaner", U"Lexem_code", U"Id"
};

static const Main_lexem_code expected_keywords[] = {
    Kw_scaner_name, Kw_codes_type, Kw_ident_name
};

static const char* expected_keywords_strings[] = {
    "%scaner_name", "%codes_type", "%ident_name"
};

static const Id_kind name_attribute[] = {
    Scaner_name, Codes_type_name, Lexem_ident_name
};

const std::set<char32_t> spaces = {
    0,  1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32
};

const char* start_proc_newline_is_not_lexem =
    R"~(::start_proc(){
    bool t = true;
    state = -1;
    /* Для автомата, обрабатывающего какую-либо лексему, состояние с номером (-1)
     * является состоянием, в котором происходит инициализация этого автомата. */
    if(belongs(SPACES, char_categories)){
        loc->current_line += U'\n' == ch;
        return t;
    }
    lexem_begin_line = loc->current_line;)~";

const char* start_proc_newline_is_lexem = R"~(::start_proc(){
    bool t = true;
    state = -1;
    /* Для автомата, обрабатывающего какую-либо лексему, состояние с номером (-1)
     * является состоянием, в котором происходит инициализация этого автомата. */
    if(belongs(SPACES, char_categories)){
        if(U'\n' == ch){
            token.code = Newline;
            lexem_begin_line = loc->current_line;
            loc->current_line += U'\n' == ch;
        }
        return t;
    }
    lexem_begin_line = loc->current_line;)~";

static const std::string possible_automata_name_str[] = {
    "A_start",     "A_unknown",   "A_id",     "A_keyword",
    "A_idKeyword", "A_delimiter", "A_number", "A_string",
    "A_comment"
};

static const std::string possible_automata_proc_proto[] = {
    "bool start_proc();",     "bool unknown_proc();",
    "bool id_proc();",        "bool keyword_proc();",
    "bool idkeyword_proc();", "bool delimiter_proc();",
    "bool number_proc();",    "bool string_proc();",
    "bool comment_proc();"
};

static const std::string possible_automata_final_proc_proto[] = {
    "void none_proc();",            "void unknown_final_proc();",
    "void id_final_proc();",        "void keyword_final_proc();",
    "void idkeyword_final_proc();", "void delimiter_final_proc();",
    "void number_final_proc();",    "void string_final_proc();",
    "void comment_final_proc();"
};

static const std::string possible_proc_ptr[] = {
    "start_proc()",     "unknown_proc()",
    "id_proc()",        "keyword_proc()",
    "idkeyword_proc()", "delimiter_proc()",
    "number_proc()",    "string_proc()",
    "comment_proc()"
};

static const std::string possible_fin_proc_ptr[] = {
    "none_proc()",            "unknown_final_proc()",
    "id_final_proc()",        "keyword_final_proc()",
    "idkeyword_final_proc()", "delimiter_final_proc()",
    "number_final_proc()",    "string_final_proc()",
    "comment_final_proc()"
};

void Main_parser::compile(){
    parse();
    et_.ec->print();
    if (!verify()){
        et_.ec->print();
        return;
    }

    if(belongs(Comment_aut, set_of_used_automata)){
        add_fictive_delimiters();
    }

    Info_for_constructing constr_info;

    constr_info.id_begin                           = unwrap_commands(id_begin);
    constr_info.id_body                            = unwrap_commands(id_body);
    constr_info.numbers_regexp                     = unwrap_commands(numbers_regexp);
    constr_info.strings_regexp                     = unwrap_commands(strings_regexp);
    size_t sp_indeces                              = constr_info.char_cat.insertSet(spaces);
    constr_info.category_name[sp_indeces]          = "SPACES";
    constr_info.name_of_scaner_class               = idx_to_string(et_.ids_trie, scaner_name_idx);
    constr_info.scaner_file_name_without_ext       = tolower_case(constr_info.name_of_scaner_class);
    constr_info.header_name                        = constr_info.scaner_file_name_without_ext + ".h";
    constr_info.impl_file_name                     = constr_info.scaner_file_name_without_ext + ".cpp";
    constr_info.aut_impl[Start_aut]                = "bool " + constr_info.name_of_scaner_class;
    constr_info.set_of_used_automata               = set_of_used_automata;
    constr_info.del_repres                         = del_repres;
    constr_info.et                                 = et_;
    constr_info.scope                              = scope_;
    constr_info.del_postaction                     = del_postaction;
    constr_info.there_is_Elem_definition           = there_is_Elem_definition;
    constr_info.codes_type_name                    = codes_type_name_idx ? idx_to_string(et_.ids_trie, codes_type_name_idx) : "Codes";
    constr_info.begin_chars                        = begin_chars;
    constr_info.acts_for_strings                   = acts_for_strings;
    constr_info.acts_for_numbers                   = acts_for_numbers;
    constr_info.possible_automata_name_str         = const_cast<std::string*>(possible_automata_name_str);
    constr_info.possible_proc_ptr                  = const_cast<std::string*>(possible_proc_ptr);
    constr_info.possible_fin_proc_ptr              = const_cast<std::string*>(possible_fin_proc_ptr);
    constr_info.kw_repres                          = kw_repres;
    constr_info.keyword_postaction                 = keyword_postaction;
    constr_info.write_action_name_idx              = write_action_name_idx;
    constr_info.codes                              = codes;
    constr_info.possible_automata_proc_proto       = const_cast<std::string*>(possible_automata_proc_proto);
    constr_info.possible_automata_final_proc_proto = const_cast<std::string*>(possible_automata_final_proc_proto);
    constr_info.mark_of_single_lined               = mark_of_single_lined;
    constr_info.mark_of_multilined_begin           = mark_of_multilined_begin;
    constr_info.mark_of_multilined_end             = mark_of_multilined_end;
    constr_info.lexem_info_name                    = "Lexem_info";
    constr_info.multilined_is_nested               = multilined_is_nested;
    constr_info.token_fields_idx                   = token_fields_idx;

    if(newline_is_lexem){
        constr_info.aut_impl[Start_aut] += start_proc_newline_is_lexem;
    }else{
        constr_info.aut_impl[Start_aut] += start_proc_newline_is_not_lexem;
    }

    implement_automata(constr_info);
    implement_scaner(constr_info);
    scaner_header(constr_info);

    aux_files_generate();
}

using Intersection_msg_index = std::pair<Begin_chars_for_aut, Begin_chars_for_aut>;

const std::map<Intersection_msg_index, std::string> intersection_msgs = {
    {{Keyword_beg_char, Delimiter_beg_char},
     "Ошибка: множество символов, с которых начинаются ключевые слова, пересекается "
     "с множеством символов, с которых начинаются разделители."},
    {{Keyword_beg_char, String_beg_char},
     "Ошибка: множество символов, с которых начинаются ключевые слова, пересекается "
     "с множеством символов, с которых начинаются строковые литералы."},
    {{Keyword_beg_char, Number_beg_char},
     "Ошибка: множество символов, с которых начинаются ключевые слова, пересекается "
     "с множеством символов, с которых начинаются числовые литералы."},
    {{Keyword_beg_char, Comment_beg_char},
     "Ошибка: множество символов, с которых начинаются ключевые слова, пересекается "
     "с множеством символов, с которых начинаются комментарии."},

    {{Id_beg_char, Delimiter_beg_char},
     "Ошибка: множество символов, с которых начинаются идентификаторы, пересекается "
     "с множеством символов, с которых начинаются разделители."},
    {{Id_beg_char, String_beg_char},
     "Ошибка: множество символов, с которых начинаются идентификаторы, пересекается "
     "с множеством символов, с которых начинаются строковые литералы."},
    {{Id_beg_char, Number_beg_char},
     "Ошибка: множество символов, с которых начинаются идентификаторы, пересекается "
     "с множеством символов, с которых начинаются числовые литералы."},
    {{Id_beg_char, Comment_beg_char},
     "Ошибка: множество символов, с которых начинаются идентификаторы, пересекается "
     "с множеством символов, с которых начинаются комментарии."},

    {{Delimiter_beg_char, String_beg_char},
     "Ошибка: множество символов, с которых начинаются разделители, пересекается "
     "с множеством символов, с которых начинаются строковые литералы."},
    {{Delimiter_beg_char, Number_beg_char},
     "Ошибка: множество символов, с которых начинаются разделители, пересекается "
     "с множеством символов, с которых начинаются числовые литералы."},

    {{String_beg_char, Number_beg_char},
     "Ошибка: множество символов, с которых начинаются строковые литералы, "
     "пересекается с множеством символов, с которых начинаются числовые литералы."},
    {{String_beg_char, Comment_beg_char},
     "Ошибка: множество символов, с которых начинаются строковые литералы, "
     "пересекается с множеством символов, с которых начинаются комментарии."},

    {{Number_beg_char, Comment_beg_char},
     "Ошибка: множество символов, с которых начинаются числовые литералы, "
     "пересекается с множеством символов, с которых начинаются комментарии."}
};

bool Main_parser::verify(){
    bool t = (et_.ec -> get_number_of_errors()) == 0;
    if(!t){
        return t;
    }

    if(!set_of_used_automata){
        printf("Ни один из определённых кодов лексем не используется"
               " ни для одной лексемы.\n");
        et_.ec -> increment_number_of_errors();
        return false;
    }

    if(belongs(Keyword_aut, set_of_used_automata)){
        Category temp_cat;
        temp_cat.kind = Set_of_cs;
        Set_of_char s;
        for(size_t kw_idx : kw_repres){
            auto kw_str = et_.strs_trie->get_string(kw_idx);
            s.insert(kw_str[0]);
        }
        temp_cat.s = s;
        begin_chars[Keyword_beg_char] = temp_cat;
    }
    if(belongs(Delimiter_aut, set_of_used_automata)){
        Category temp_cat;
        temp_cat.kind = Set_of_cs;
        Set_of_char s;
        for(size_t del_idx : del_repres){
            auto del_str = et_.strs_trie->get_string(del_idx);
            s.insert(del_str[0]);
        }
        temp_cat.s = s;
        begin_chars[Delimiter_beg_char] = temp_cat;
    }

    if(belongs(String_aut, set_of_used_automata)){
        begin_chars[String_beg_char] = first_chars(strings_regexp);
    }
    if(belongs(Number_aut, set_of_used_automata)){
        begin_chars[Number_beg_char] = first_chars(numbers_regexp);
    }

    if(belongs(Id_aut, set_of_used_automata)){
        if(id_begin.empty() || id_body.empty()){
            puts("Ошибка: имя лексемы 'идентификатор' определено, а "
                 "структура идентификаторов не описана.");
            et_.ec -> increment_number_of_errors();
            return false;
        }
        begin_chars[Id_beg_char] = first_chars(id_begin);
    }
    if(belongs(Comment_aut, set_of_used_automata)){
        Category c1, c2;
        if(mark_of_single_lined){
            auto s = et_.strs_trie->get_string(mark_of_single_lined);
            c1.kind = Set_of_cs;
            c1.s    = Set_of_char({s[0]});
        }
        if(mark_of_multilined_begin){
            auto s = et_.strs_trie->get_string(mark_of_multilined_begin);
            c2.kind = Set_of_cs;
            c2.s    = Set_of_char({s[0]});
        }
        begin_chars[Comment_beg_char] = c1 + c2;
    }

    for(const auto& z : intersection_msgs){
        auto  msg_index = z.first;
        auto& msg       = z.second;
        if(begin_chars[msg_index.first] * begin_chars[msg_index.second]){
            puts(msg.c_str());
            et_.ec -> increment_number_of_errors();
            t = false;
        }
    }

    return t;
}

void Main_parser::parse(){
    while((lc = (li = msc-> current_lexem()).code)){
        if(Unknown == lc){
            continue;
        };
        msc->back();
        del_postaction     = 0;
        keyword_postaction = 0;
        switch(lc){
            case Kw_scaner_name:
                name_section_handling(scaner_name_idx, Scaner_name_sec);
                break;
            case Kw_codes_type:
                name_section_handling(codes_type_name_idx, Lexem_code_name_sec);
                break;
            case Kw_ident_name:
                name_section_handling(ident_name_idx, Id_name_sec);
                break;
            case Kw_token_fields:
                token_fields_sec();
                break;
            case Kw_class_members:
                class_members_sec();
                break;
            case Kw_newline_is_lexem:
                codes_and_newline->newline_is_lexem_sec();
                newline_is_lexem = true;
                break;
            case Kw_codes: case Comma: case Id:
                codes_and_newline->codes_sec(codes, last_code_val);
                break;
            case Kw_keywords: case Opened_fig_brack: case Closed_fig_brack:
            case String: case Colon:
                keyword_postaction =
                    keywords_sec_parser->compile(kw_repres, codes, last_code_val);
                set_of_used_automata |= 1ULL << Keyword_aut;
                break;
            case Kw_delimiters:
                del_postaction =
                    delimiters_sec_parser->compile(del_repres, codes, last_code_val);
                set_of_used_automata |= 1ULL << Delimiter_aut;
                break;
            case Kw_idents:
                idents_sec();
                break;
            case Kw_numbers: case Kw_action:
                num_sec_parser->compile(numbers_regexp, acts_for_numbers);
                set_of_used_automata |= 1ULL << Number_aut;
                break;
            case Kw_strings:
                strs_sec_parser->compile(strings_regexp, acts_for_strings);
                set_of_used_automata |= 1ULL << String_aut;
                break;
            case Kw_comments: case Kw_nested: case Kw_multilined: case Kw_single_lined:
                std::tie(mark_of_single_lined,
                         mark_of_multilined_begin,
                         mark_of_multilined_end,
                         multilined_is_nested) =
                    comments_parser->compile();
                set_of_used_automata |= 1ULL << Comment_aut;
                break;
            default:
                ;
        }
    }
}

bool Main_parser::id_is_defined(size_t idx){
    bool is_def = false;
    auto s = scope_->idsc.find(idx);
    if(s != scope_->idsc.end()){
        printf("В строке %zu повторно определён идентификатор ",
               msc->lexem_begin_line_number());
        et_.ids_trie->print(idx); printf("\n");
        et_.ec->increment_number_of_errors();
        is_def = true;
    }
    return is_def;
}

void Main_parser::set_default_name(size_t& ind, Name_sec section){
    char32_t*           default_name     =
        const_cast<char32_t*>(default_names[section]);
    Id_kind         attribute        = name_attribute[section];
    Id_attributes   iattr;

    size_t idx = et_.ids_trie->insert(default_name);
    iattr.kind = attribute;
    scope_->idsc[idx] = iattr; ind = idx;
}

void Main_parser::name_section_handling(size_t& ind, Name_sec section){
    Main_lexem_code expected_keyword = expected_keywords[section];
    Id_kind         attribute        = name_attribute[section];
    Id_attributes   iattr;

    lc = (li = msc-> current_lexem()).code;
    if(Id == li.code){
        printf("В строке %zu ожидается %s", msc->lexem_begin_line_number(),
               expected_keywords_strings[section]);
        et_.ec->increment_number_of_errors();
        iattr.kind = attribute;
        size_t idx = li.ident_index;
        if(id_is_defined(idx)){return;};
        scope_->idsc[idx] = iattr; ind = idx;
        return;
    }
    if(lc != expected_keyword){
        msc->back();
        if(Id_name_sec != section){
            set_default_name(ind, section);
        }
        return;
    }

    lc = (li = msc-> current_lexem()).code;
    if(Id == lc){
        iattr.kind = attribute;
        size_t idx = li.ident_index;
        if(id_is_defined(idx)){return;};
        scope_->idsc[idx] = iattr; ind = idx;
        if(Id_name_sec == section){
            set_of_used_automata |= 1ULL << Id_aut;
        };
        return;
    }

    msc->back();
    if(Id_name_sec != section){
        set_default_name(ind, section);
    };
    return;
}

void Main_parser::token_fields_sec(){
    lc = (li = msc-> current_lexem()).code;
    if(Kw_token_fields != lc){
        if(String == lc){
            printf("В строке %zu ожидается %%token_fields.\n",
                   msc->lexem_begin_line_number());
            et_.ec->increment_number_of_errors();
            token_fields_idx = li.string_index;
        }else{
            msc->back();
        };
        return;
    }

    lc = (li = msc-> current_lexem()).code;
    if(String == lc){
        token_fields_idx = li.string_index;
    }else{
        printf("В строке %zu ожидается строковый литерал с добавляемыми полями лексемы.\n",
               msc->lexem_begin_line_number());
        et_.ec->increment_number_of_errors();
        msc->back();
    }
}

void Main_parser::class_members_sec(){
    lc = (li = msc-> current_lexem()).code;
    if(Kw_class_members != lc){
        if(String == li.code){
            printf("В строке %zu ожидается %%class_members.\n",
                   msc->lexem_begin_line_number());
            et_.ec->increment_number_of_errors();
            class_members_idx = li.string_index;
        }else{
            msc->back();
        };
        return;
    }

    lc = (li = msc-> current_lexem()).code;
    if(String == lc){
        class_members_idx = li.string_index;
    }else{
        printf("В строке %zu ожидается строковый литерал с добавляемыми полями класса.\n",
               msc->lexem_begin_line_number());
        et_.ec->increment_number_of_errors();
        msc->back();
    }
}

void Main_parser::idents_sec(){
    lc = (li = msc-> current_lexem()).code;
    if(lc != Kw_idents){
        msc->back();
        return;
    }
    if(!ident_name_idx){
        puts("Не задано имя лексемы 'идентификатор'.");
        et_.ec -> increment_number_of_errors();
    }
    id_definition_parser->compile(id_begin);
    id_definition_parser->compile(id_body);
}

#define INDENT_WIDTH           4
#define DOUBLE_INDENT_WIDTH    ((INDENT_WIDTH) * 2)
#define TRIPLE_INDENT_WIDTH    ((INDENT_WIDTH) * 3)
#define QUADRUPLE_INDENT_WIDTH ((INDENT_WIDTH) * 4)
static const std::string indent           = std::string(INDENT_WIDTH,           ' ');
static const std::string double_indent    = std::string(DOUBLE_INDENT_WIDTH,    ' ');
static const std::string triple_indent    = std::string(TRIPLE_INDENT_WIDTH,    ' ');
static const std::string quadruple_indent = std::string(QUADRUPLE_INDENT_WIDTH, ' ');

void Main_parser::add_new_lexem_code(size_t idx){
    auto s = scope_->idsc.find(idx);
    Id_attributes iattr;
    if(s != scope_->idsc.end()){
        printf("В строке %zu повторно определён идентификатор ",
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

void Main_parser::add_new_string(const size_t idx, const size_t code_){
/* Первый аргумент данной функции --- индекс строкового литерала, представляющего
 * ключевое слово или разделитель, в префиксном дереве строковых литералов, а
 * второй аргумент --- индекс идентификатора, являющегося соответствующим кодом лексемы,
 * в префиксном дереве идентификаторов. */
    auto s = scope_->strsc.find(idx);
    Str_attributes sattr;
    Id_attributes  iattr;
    if(s != scope_->strsc.end()){
        printf("В строке %zu повторно определён разделитель ",
               msc->lexem_begin_line_number());
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
    sattr.kind  = Delimiter_repres;
    sattr.code  = iattr.code;
    scope_->strsc[idx] = sattr;
    del_repres.push_back(idx);
    }
}

void Main_parser::add_fictive_delimiter(const std::u32string& dcode,
                                        size_t drepres_idx){
    size_t d_idx = et_.ids_trie->insert(dcode);
    add_new_lexem_code(d_idx);
    add_new_string(drepres_idx, d_idx);
}

void Main_parser::add_fictive_delimiters(){
    if(mark_of_single_lined){
        add_fictive_delimiter(U"SINGLE_LINED_COMMENT_MARK", mark_of_single_lined);
    }
    if(mark_of_multilined_begin){
        add_fictive_delimiter(U"MULTI_LINED_COMMENT_MARK", mark_of_multilined_begin);
    }
    if(mark_of_multilined_end){
        add_fictive_delimiter(U"MULTI_LINED_COMMENT_END", mark_of_multilined_end);
    }
}

int Main_parser::get_number_of_errors(){
    return et_.ec -> get_number_of_errors();
}