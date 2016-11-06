/*
    Файл:    main_parser.cpp
    Создан:  14 декабря 2015г. в 15:25 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
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
#include "../include/detalize_commands.h"
#include "../include/attributed_char_trie.h"
#include "../include/char_conv.h"
#include "../include/list_to_columns.h"
#include "../include/char_trie.h"
#include "../include/operations_with_sets.h"
// #include "../include/print_commands.h"
// #include <bitset>
// #include <set>

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

static const std::string none_proc = R"~(::none_proc(){
    /* Данная подпрограмма будет вызвана, если по прочтении входного текста оказались
     * в автомате A_start. Тогда ничего делать не нужно. */
})~";

void Main_parser::compile(){
    parse();
    if (!verify()){
        et_.ec->print();
        return;
    }

    detalize_commands(id_begin);
    detalize_commands(id_body);
    detalize_commands(numbers_regexp);
    detalize_commands(strings_regexp);

    size_t sp_indeces = char_cat.insertSet(spaces);
    category_name[sp_indeces] = "SPACES";

    name_of_scaner_class = idx_to_string(et_.ids_trie, scaner_name_idx);
    scaner_file_name_without_ext = name_of_scaner_class;
    for(char& c : scaner_file_name_without_ext){
        c = tolower(c);
    }

    aut_impl[Start_aut] = "bool " + name_of_scaner_class;

    if(newline_is_lexem){
        aut_impl[Start_aut] += start_proc_newline_is_lexem;
    }else{
        aut_impl[Start_aut] += start_proc_newline_is_not_lexem;
    }

    if(belongs(Comment_aut, set_of_used_automata)){
        add_fictive_delimiters();
    }

    generate_automata_impl();
    prepare_automata_info();

    generate_scaner_implementation();
    generate_scaner_header();

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
        for(size_t kw_idx : kw_repres){
            auto kw_str = et_.strs_trie->get_string(kw_idx);
            Category    cat;
            cat.kind = Set_of_cs;
            cat.s    = Set_of_char({kw_str[0]});
            temp_cat = temp_cat + cat;
        }
        begin_chars[Keyword_beg_char] = temp_cat;
    }
    if(belongs(Delimiter_aut, set_of_used_automata)){
        Category temp_cat;
        for(size_t del_idx : del_repres){
            auto del_str = et_.strs_trie->get_string(del_idx);
            Category    cat;
            cat.kind = Set_of_cs;
            cat.s    = Set_of_char({del_str[0]});
            temp_cat = temp_cat + cat;
        }
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
const std::string indent           = std::string(INDENT_WIDTH,           ' ');
const std::string double_indent    = std::string(DOUBLE_INDENT_WIDTH,    ' ');
const std::string triple_indent    = std::string(TRIPLE_INDENT_WIDTH,    ' ');
const std::string quadruple_indent = std::string(QUADRUPLE_INDENT_WIDTH, ' ');

const char* header_includes =
    R"~(#include "../include/abstract_scaner.h"
#include "../include/error_count.h"
#include "../include/location.h"
#include <string>")~";

void Main_parser::generate_scaner_header(){
    std::string sentinel = name_of_scaner_class;
    for(char& c : sentinel){
        c = toupper(c);
    }
    sentinel += "_H";

    std::string header_name = scaner_file_name_without_ext + ".h";
    std::string header = "#ifndef " + sentinel + "\n#define " +
                         sentinel + "\n\n" + header_includes + "\n\n";

    lexem_info_name = "Lexem_info";

    header += generate_lexem_codes_enum();
    header += generate_lexem_info();
    header += generate_scaner_class();

    if(!fields_for_comments_handling.empty()){
        header += "\n" + fields_for_comments_handling + "\n";
    }

    header += "#endif";

    FILE* fptr = fopen(header_name.c_str(), "w");
    if(fptr){
        fputs(header.c_str(), fptr);
        fputs("\n",fptr);
        fclose(fptr);
    }else{
        printf("Не удалось создать заголовочный файл сканера.\n");
        et_.ec -> increment_number_of_errors();
    }
}

std::string Main_parser::generate_lexem_codes_enum(){
    std::string s;
    codes_type_name = idx_to_string(et_.ids_trie, codes_type_name_idx);
    s = "enum " + codes_type_name + " : unsigned short {\n";
    std::vector<std::string> lexem_codes_names;
    for(const size_t c : codes){
        lexem_codes_names.push_back(idx_to_string(et_.ids_trie,c));
    }
    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 0;
    s += string_list_to_columns(lexem_codes_names, f);
    s += "\n};\n\n";
    return s;
}

std::string Main_parser::generate_lexem_info(){
    std::string s;
    s = "struct " + lexem_info_name + "{\n" +
        indent + idx_to_string(et_.ids_trie, codes_type_name_idx) +
        " code;\n" + indent + "union{\n";

    bool t = belongs(Id_aut, set_of_used_automata) ||
             belongs(IdKeyword_aut, set_of_used_automata);

    if(t){
        s += double_indent + "size_t    ident_index;\n";
    };
    if(belongs(String_aut, set_of_used_automata)){
        s += double_indent + "size_t    string_index;\n" +
             double_indent + "char32_t  c;\n";
    };

    s += idx_to_string(et_.strs_trie, token_fields_idx) +
         "\n" + indent + "};\n};\n\n";
    return s;
}

static std::string automaton_procs_typedefs(const std::string& s){
    std::string result;
    result = "   typedef bool (" + s +
        R"~(::*Automaton_proc)();
    /* Это тип указателя на функцию--член, реализующую
     * конечный автомат, обрабатывающий лексему. Функция
     * эта должна возвращать true, если лексема ещё не
     * разобрана до конца, и false --- в противном случае. */

    typedef void ()~" + s +
    R"~(::*Final_proc)();
    /* А это -- тип указателя на функцию-член, выполняющую
     * необходимые действия в случае неожиданного окончания
     * лексемы. */

    static Automaton_proc procs[];
    static Final_proc     finals[];

    /* функции обработки лексем: */
)~";
    return result;
}

std::string Main_parser::generate_scaner_class(){
    std::string scaner_class;
    scaner_class = "class " + name_of_scaner_class +
        " : public Abstract_scaner<" + lexem_info_name + "> {\npublic:\n" +
        indent + name_of_scaner_class + "() = default;\n" +
        indent + name_of_scaner_class +
        "(Location* location, const Errors_and_tries& et) :\n" +
        double_indent + "Abstract_scaner<" + lexem_info_name +
        ">(location, et) {};\n" +
        indent + name_of_scaner_class +"(const " +
        name_of_scaner_class + "& orig) = default;\n" +
        indent + "virtual ~" + name_of_scaner_class +
        "() = default;\n" +
        indent + "virtual " + lexem_info_name + " current_lexem();\n" +
        "private:\n";
    scaner_class += generate_automata_enum() +
                    automaton_procs_typedefs(name_of_scaner_class) +
                    generate_automata_proc_protos() +
                    generate_automata_final_procs_protos();
    scaner_class += "\n};\n";
    return scaner_class;
}

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

void Main_parser::prepare_automata_info(){
    set_of_used_automata |= (1ULL << Start_aut) | (1ULL << Unknown_aut);
    bool t = belongs(Comment_aut, set_of_used_automata) != 0;
    set_of_used_automata &= ~(1ULL << Comment_aut);
    for(int a = Start_aut; a <= Comment_aut; a++){
        Automaton_with_procs ap;
        if(belongs(a, set_of_used_automata)){
            ap.name           = possible_automata_name_str[a];
            ap.proc_proto     = possible_automata_proc_proto[a];
            ap.fin_proc_proto = possible_automata_final_proc_proto[a];
            ap.proc_ptr       = "&" + name_of_scaner_class + "::" + possible_proc_ptr[a];
            ap.fin_proc_ptr   = "&" + name_of_scaner_class + "::" +
                                possible_fin_proc_ptr[a];
            automaton_info.push_back(ap);
        }
    }
    if(t){
        set_of_used_automata |= 1ULL << Comment_aut;
    }
}

std::string Main_parser::generate_automata_enum(){
    std::string s;
    s = indent + "enum Automaton_name{\n";
    std::vector<std::string> automata_names;
    for(const auto& ap : automaton_info){
        automata_names.push_back(ap.name);
    }

    Format f;
    f.indent                 = DOUBLE_INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 1;

    s += string_list_to_columns(automata_names, f);
    s += "\n" + indent + "};\n" + indent +
         "Automaton_name automaton; /* текущий автомат */\n\n";
    return s;
}

std::string Main_parser::generate_automata_proc_protos(){
    std::string s;
    std::vector<std::string> automata_protos;
    for(const auto ap : automaton_info){
        automata_protos.push_back(ap.proc_proto);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    s += string_list_to_columns(automata_protos, f, 0);
    s += '\n';
    s += R"~(    /* функции для выполнения действий в случае неожиданного
     * окончания лексемы */
)~";
    return s;
}

std::string Main_parser::generate_automata_final_procs_protos(){
    std::string s;
    std::vector<std::string> final_procs_protos;
    for(const auto ap : automaton_info){
        final_procs_protos.push_back(ap.fin_proc_proto);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    s += string_list_to_columns(final_procs_protos, f, 0);
    return s;
}

static std::string impl_includes(const std::string& impl_hn){
    std::string result;
    result = "#include \"../include/" + impl_hn + "\"\n" +
             R"~(#include "../include/get_init_state.h"
#include "../include/search_char.h"
#include "../include/belongs.h"
#include <set>
#include <string>
#include <vector>
#include "../include/operations_with_sets.h"
)~";
    return result;
}

std::string Main_parser::automata_table(){
    std::string result = name_of_scaner_class +"::Automaton_proc " +
                         name_of_scaner_class + "::procs[] = {\n";

    std::vector<std::string> procs_list;
    for(const auto ap : automaton_info){
        procs_list.push_back(ap.proc_ptr);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result += string_list_to_columns(procs_list, f) + "\n};";
    return result;
}

std::string Main_parser::final_table(){
    std::string result = name_of_scaner_class +"::Final_proc " +
                         name_of_scaner_class + "::finals[] = {\n";

    std::vector<std::string> fprocs_list;
    for(const auto ap : automaton_info){
        fprocs_list.push_back(ap.fin_proc_ptr);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result += string_list_to_columns(fprocs_list, f) + "\n};";
    return result;
}

std::string  Main_parser::procs_tables(){
    std::string result = automata_table() + "\n\n" + final_table() + "\n\n";
    return result;
}

std::string add_newline_if_str_is_not_empty(const std::string& s){
    std::string result;
    if(!s.empty()){
        result = s + "\n";
    }
    return result;
}

std::string delim_init_table(const Jumps_and_inits& ji,
                             const std::string&  init_table_name){
    std::string result;
    result = "static const State_for_char " + init_table_name + "[] ={\n";

    std::vector<std::string> init_table_elems;
    for(const auto e : ji.init_table){
        std::string temp = "{" + std::to_string(e.first) + ", U\'" +
                           char32_to_utf8(e.second) + "\'}";
        init_table_elems.push_back(temp);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 5;
    f.spaces_between_columns = 1;
    result += string_list_to_columns(init_table_elems, f) + "\n};\n\n";
    return result;
}

std::string generate_Elem(const std::string& s){
    std::string result = R"~(struct Elem {
    /** Указатель на строку , состоящую из символов , по которым
        возможен переход. */
    char32_t*       symbols;
    /** код лексемы */
    )~" + s + R"~( code;
    /** Если текущий символ совпадает с symbols[0], то
        выполняется переход в состояние first_state;
        если текущий символ совпадает с symbols[1], то
        выполняется переход в состояние first_state+1;
        если текущий символ совпадает с symbols[2], то
        выполняется переход в состояние first_state+2,
        и так далее. */
    uint16_t        first_state;
};

)~";
    return result;
}

static const std::string del_jump_table_name = "delim_jump_table";
static const std::string del_init_table_name = "init_table_for_delimiters";

std::string Main_parser::delim_table_gen(const Jumps_and_inits& ji,
                                         const std::string&     table_name){
    std::string result = "static const Elem " + table_name + "[] = {\n";
    std::vector<std::string> del_jumps;
    for(const auto& j : ji.jumps){
        std::string temp = "{const_cast<char32_t*>(U\"" +
                           u32string_to_utf8(j.jump_chars) + "\"), " +
                           idx_to_string(et_.ids_trie, codes[j.code]) + ", " +
                           std::to_string(j.first_state) + "}";
        del_jumps.push_back(temp);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 1;
    f.spaces_between_columns = 1;
    result += string_list_to_columns(del_jumps, f) + "\n};\n\n";
    return result;
}

std::string Main_parser::jump_table_string_repres(const Jumps_and_inits& ji,
                                                  const std::string&     table_name,
                                                  const std::string&     init_table_name){
    auto result = delim_init_table(ji, init_table_name);
    auto temp   = delim_table_gen(ji, table_name);
    if(!there_is_Elem_definition){
        result += generate_Elem(codes_type_name) + temp;
        there_is_Elem_definition = true;
    }else{
        result += temp;
    }
    return result;
}

static const std::string delim_proc_body_ = R"~(::delimiter_proc(){
    bool t = false;
    if(-1 == state){
        state = get_init_state(ch, init_table_for_delimiters,
                               sizeof(init_table_for_delimiters)/sizeof(State_for_char));
        token.code = delim_jump_table[state].code;
        t = true;
        return t;
    }
    Elem elem = delim_jump_table[state];
    token.code = delim_jump_table[state].code;
    int y = search_char(ch, elem.symbols);
    if(y != THERE_IS_NO_CHAR){
        state = elem.first_state + y; t = true;
    })~";

static std::string delim_proc_body(const std::string& s){
    std::string result;
    result = delim_proc_body_;
    if(s.empty()){
        result += "\n" + indent + "return t;\n}";
    }else{
        result += "\n" +
                  indent + "if(!t){\n" +
                  double_indent + s + "\n" +
                  indent + "}\n" +
                  indent + "return t;\n}";
    }
    return result;
}

static const std::string del_begin_cat_name_by_default = "DELIMITER_BEGIN";

void Main_parser::generate_delim_automaton_impl(){
    /* Данная функция строит реализацию автомата, обрабатывающего разделители. */
    if(!belongs(Delimiter_aut, set_of_used_automata)){
        return;
    }

    std::set<char32_t>          first_chars_for_delims; /* Это множество
       состоит из символов, с которых могут начинаться разделители. */
    Attributed_char_trie        atrie;

    for(size_t del_idx : del_repres){
        auto delimiter = et_.strs_trie->get_string(del_idx);
        delimiter_strings.push_back(delimiter);
        first_chars_for_delims.insert(delimiter[0]);
    }
    size_t counter = 0;
    for(size_t del_idx : del_repres){
        Attributed_cstring atrib_cstr;
        atrib_cstr.str       = const_cast<char32_t*>(delimiter_strings[counter].c_str());
        atrib_cstr.attribute = (scope_->strsc[del_idx]).code;
        atrie.insert(attributed_cstring2string(atrib_cstr, 1));
        counter++;
    }

    Jumps_and_inits jmps = atrie.jumps(); /* построили заготовку под таблицу переходов */
    /* теперь нужно дописать нужный текст в реализацию стартового автомата
       и сгенерировать функцию, обрабатывающую разделители */
    auto cat_res = add_category(first_chars_for_delims, del_begin_cat_name_by_default);
    std::string delimiter_begin_cat_name = cat_res.second;

    aut_impl[Start_aut] += "\n    if(belongs(" + delimiter_begin_cat_name +
        ", char_categories)){\n        (loc->pcurrent_char)--; " +
        "automaton = A_delimiter;\n        state = -1;\n        return t;\n    }\n";

    auto del_postact = get_act_repres(del_postaction);

    aut_impl[Delimiter_aut] = jump_table_string_repres(jmps, del_jump_table_name,
                                                       del_init_table_name) +
                              "bool " + name_of_scaner_class + delim_proc_body(del_postact);

    aut_impl_fin_proc[Delimiter_aut] = "void " + name_of_scaner_class +
                                       R"~(::delimiter_final_proc(){
    )~" + indent + del_postact +
    R"~(
    token.code = delim_jump_table[state].code;
    )~" + "\n}";
}

std::string str_repres_for_set_of_size_t_const(const std::set<size_t>& s,
                                               const std::string& const_name){
    std::string result;
    result = "static const std::set<size_t> " + const_name + " = {\n";

    std::vector<std::string> elems;
    for(auto x : s){
        elems.push_back(std::to_string(x));
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 8;
    f.spaces_between_columns = 1;

    result += string_list_to_columns(elems, f) +"\n};";
    return result;
}

std::string sp_else_sp = " else ";

static const std::string string_begin_chars_category_name = "STRING_BEGIN";

static const std::string writing_str_into_trie =
    "token.string_index = strs -> insert(buffer);";

void Main_parser::generate_strings_automaton_impl(){
    if(!belongs(String_aut, set_of_used_automata)){
        return;
    }

    auto symbols = begin_chars[String_beg_char].s;
    auto cat_res = add_category(symbols, string_begin_chars_category_name);
    auto strs_begin_category_name = cat_res.second;

    auto temp = "\n    if(belongs(" + strs_begin_category_name +
                ", char_categories)){\n        (loc->pcurrent_char)--; " +
                "automaton = A_string;\n        state = 0;\n";

    auto init_acts_str = idx_to_string(et_.strs_trie, acts_for_strings.init_acts);
    auto fin_acts_str  = idx_to_string(et_.strs_trie, acts_for_strings.fin_acts);

    if(init_acts_str.empty()){
        temp += double_indent + "return t;\n" +
                indent        + "}\n";
    }else{
        temp += double_indent + init_acts_str +"\n" +
                double_indent + "return t;\n" +
                indent        + "}\n";
    }

    aut_impl[Start_aut] += temp;

    G_DFA string_GDFA;
    grouped_DFA_by_regexp(string_GDFA, strings_regexp);

    Str_data_for_automaton f;
    f.automata_name         = possible_automata_name_str[String_aut];
    f.proc_name             = possible_proc_ptr[String_aut];
    f.category_name_prefix  = "STRING";
    f.diagnostic_msg        = "В строке %zu неожиданно закончился строковый литерал.";
    f.final_states_set_name = "final_states_for_strings";

    temp =  "void " + name_of_scaner_class + "::" +
            possible_fin_proc_ptr[String_aut] + "{\n" +
            indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
            double_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
            double_indent + "en->increment_number_of_errors();\n" +
            indent + "}\n";

    if(fin_acts_str.empty()){
        f.final_actions =  writing_str_into_trie;
        temp            += indent + writing_str_into_trie + "\n}";
    }else{
        f.final_actions =  fin_acts_str + "\n" +
                           triple_indent + writing_str_into_trie;
        temp            += indent + fin_acts_str + "\n" +
                           indent + writing_str_into_trie + "\n}";
    }

    aut_impl[String_aut] = automata_repres(string_GDFA, f);
    aut_impl_fin_proc[String_aut] = temp;
}

static const std::string number_begin_chars_category_name = "NUMBER_BEGIN";

void Main_parser::generate_numbers_automaton_impl(){
    if(!belongs(Number_aut, set_of_used_automata)){
        return;
    }
    G_DFA num_GDFA;
    grouped_DFA_by_regexp(num_GDFA, numbers_regexp);

    auto init_acts_num = idx_to_string(et_.strs_trie, acts_for_numbers.init_acts);
    auto fin_acts_num  = idx_to_string(et_.strs_trie, acts_for_numbers.fin_acts);

    auto symbols = begin_chars[Number_beg_char].s;
    auto cat_res = add_category(symbols, number_begin_chars_category_name);
    auto num_begin_category_name = cat_res.second;

    auto temp = "\n    if(belongs(" + num_begin_category_name +
                ", char_categories)){\n        (loc->pcurrent_char)--; " +
                "automaton = A_number;\n        state = 0;\n";

    if(init_acts_num.empty()){
        temp += double_indent + "return t;\n" +
                indent        + "}\n";
    }else{
        temp += double_indent + init_acts_num +"\n" +
                double_indent + "return t;\n" +
                indent        + "}\n";
    }

    aut_impl[Start_aut] += temp;

    Str_data_for_automaton f;
    f.automata_name         = possible_automata_name_str[Number_aut];
    f.proc_name             = possible_proc_ptr[Number_aut];
    f.category_name_prefix  = "NUMBER";
    f.diagnostic_msg        = "В строке %zu неожиданно закончилось число.";
    f.final_states_set_name = "final_states_for_numbers";

    temp = "void " + name_of_scaner_class + "::" +
           possible_fin_proc_ptr[Number_aut] + "{\n" +
           indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
           double_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
           double_indent + "en->increment_number_of_errors();\n" +
           indent + "}\n";

    f.final_actions = fin_acts_num;

    if(fin_acts_num.empty()){
        temp += "}\n";
    }else{
        temp += indent + fin_acts_num + "\n}";
    }

    aut_impl[Number_aut]          = automata_repres(num_GDFA, f);
    aut_impl_fin_proc[Number_aut] = temp;
}

static const std::string separate_keyword_proc_body_ = R"~(::keyword_proc(){
    bool t = false;
    if(-1 == state){
        state = get_init_state(ch, init_table_for_keywords,
                               sizeof(init_table_for_keywords)/sizeof(State_for_char));
        token.code = keywords_jump_table[state].code;
        t = true;
        return t;
    }
    Elem elem  = keywords_jump_table[state];
    token.code = keywords_jump_table[state].code;
    int y = search_char(ch, elem.symbols);
    if(y != THERE_IS_NO_CHAR){
        state = elem.first_state + y; t = true;
    })~";

std::string dindent(const std::string& s){
    std::string result;
    if(!s.empty()){
        result = double_indent + s;
    }
    return result;
}

std::string sindent(const std::string& s){
    std::string result;
    if(!s.empty()){
        result = indent + s;
    }
    return result;
}

static std::string separate_keyword_proc_body(const std::string& s){
    std::string result;
    result = separate_keyword_proc_body_;
    if(s.empty()){
        result += "\n" + indent + "return t;\n}";
    }else{
        result += "\n" +
                  indent + "if(!t){\n" +
                  double_indent + s + "\n" +
                  indent + "}\n" +
                  indent + "return t;\n}";
    }
    return result;
}

static const std::string keywords_begin_cat_name_by_default = "KEYWORD_BEGIN";

static const std::string keyword_jump_table_name = "keyword_jump_table";
static const std::string keyword_init_table_name = "init_table_for_keywords";

void Main_parser::generate_separate_keywords_automat(){
    /* Данная функция строит реализацию отдельного автомата, обрабатывающего
     * ключевые слова. */
    auto                 first_chars_for_keywords = begin_chars[Keyword_beg_char].s;
    Attributed_char_trie atrie;

    for(size_t kw_idx : kw_repres){
        auto keyword = et_.strs_trie->get_string(kw_idx);
        keyword_strings.push_back(keyword);
    }

    size_t counter = 0;
    for(size_t kw_idx : kw_repres){
        Attributed_cstring atrib_cstr;
        atrib_cstr.str       = const_cast<char32_t*>(keyword_strings[counter].c_str());
        atrib_cstr.attribute = (scope_->strsc[kw_idx]).code;
        atrie.insert(attributed_cstring2string(atrib_cstr, 1));
        counter++;
    }

    Jumps_and_inits jmps = atrie.jumps(); /* построили заготовку под таблицу переходов */
    /* теперь нужно дописать нужный текст в реализацию стартового автомата
       и сгенерировать функцию, обрабатывающую ключевые слова */
    auto cat_res = add_category(first_chars_for_keywords, keywords_begin_cat_name_by_default);
    std::string keyword_begin_cat_name = cat_res.second;

    aut_impl[Start_aut] += "\n    if(belongs(" + keyword_begin_cat_name +
        ", char_categories)){\n        (loc->pcurrent_char)--; " +
        "automaton = A_keyword;\n        state = -1;\n        return t;\n    }\n";

    auto kw_postact = get_act_repres(keyword_postaction);

    aut_impl[Keyword_aut] = jump_table_string_repres(jmps, keyword_jump_table_name,
                                                     keyword_init_table_name) +
                              "bool " + name_of_scaner_class + separate_keyword_proc_body(kw_postact);

    aut_impl_fin_proc[Keyword_aut] = "void " + name_of_scaner_class +
                                       R"~(::keyword_final_proc(){
    )~" + indent + kw_postact +
    R"~(
    token.code = keyword_jump_table[state].code;
    )~" + "\n}";
}

static const std::string identifier_begin_cat_name_by_default = "IDENTIFIER_BEGIN";

/* Построение по командам, отвечающим регуляркам a и b, команд, отвечающих регулярке ab*. */
Command_buffer regexp1_with_regexp2ast(const Command_buffer& a, const Command_buffer& b){
    Command_buffer x                             = a;
    Command_buffer y                             = b;
    size_t         num_commands_in_a             = x.size();
    size_t         last_idx_in_a                 = num_commands_in_a -1;

    for(auto& c : y){
        switch(c.name){
            case Cmd_or: case Cmd_concat:
                c.args.first += num_commands_in_a; c.args.second += num_commands_in_a;
                break;
            case Cmd_Kleene: case Cmd_positive: case Cmd_optional:
                c.args.first += num_commands_in_a;
                break;
            default:
                ;
        }
    }
    x.insert(x.end(), y.begin(), y.end());

    size_t         last_idx_in_a_appended_with_b = x.size() - 1;
    Command        com;
    com.action_name = 0;
    com.name        = Cmd_Kleene;
    com.args.first  = last_idx_in_a_appended_with_b;
    com.args.second = 0;

    x.push_back(com);

    com.action_name = 0;
    com.name        = Cmd_concat;
    com.args.first  = last_idx_in_a;
    com.args.second = x.size() - 1;

    x.push_back(com);

    return x;
}

std::string qindent_string(const std::string& s){
    std::string result;
    if(!s.empty()){
        result = quadruple_indent + s + "\n";
    }
    return result;
}

std::string Main_parser::automata_repres_case_j(const Category&              cat,
                                                const DFA_state_with_action& swa,
                                                const Str_data_for_automaton& f)
{
    std::string result;
    auto        temp   = get_act_repres(swa.action_idx);
    switch(cat.kind){
        case All_chars:
            if(!temp.empty()){
                result = temp + "\n" +
                         triple_indent + "state = " + std::to_string(swa.st) + ";\n" +
                         triple_indent + "there_is_jump = true;\n";
            }else{
                result = "state = " +  std::to_string(swa.st) + ";\n" +
                         triple_indent + "there_is_jump = true;\n";
            }
            break;
        case Not_single_quote:
            result = R"~( else if(ch != U'\''){)~" "\n" + qindent_string(temp) +
                     quadruple_indent + "state = " + std::to_string(swa.st) + ";\n" +
                     quadruple_indent + "there_is_jump = true;\n" +
                     triple_indent + "}\n";
            break;
        case Not_double_quote:
            result = R"~( else if(ch != U'\"'){)~" "\n" + qindent_string(temp) +
                     quadruple_indent + "state = " + std::to_string(swa.st) + ";\n" +
                     quadruple_indent + "there_is_jump = true;\n" +
                     triple_indent + "}\n";
            break;
        case Set_of_cs:
            {
                std::string default_cat_name = f.category_name_prefix +
                                               std::to_string(last_category_suffix);
                auto result_cat = add_category(cat.s, default_cat_name);
                if(result_cat.first){
                    // если категории до того не было
                    last_category_suffix++;
                }
                result = " else if(belongs(" + result_cat.second +
                         ", char_categories)){\n" + qindent_string(temp) +
                         quadruple_indent + "state = " + std::to_string(swa.st) + ";\n" +
                         quadruple_indent + "there_is_jump = true;\n" +
                         triple_indent + "}\n";
            }
            break;
    }
    return result;
}

std::string Main_parser::automata_repres_case(const G_DFA_state_jumps& m,
                                              const Str_data_for_automaton& f,
                                              size_t counter){
#define ELSE_SPACE_WIDTH 5
    std::string result;
    if(m.empty()){
        return result;
    }
    result = double_indent + "case " + std::to_string(counter) + ":\n";
    std::vector<std::string> ifs;
    // size_t category_counter = 0;
    for(const auto& j : m){
        auto cat  = j.first;
        auto swa  = j.second;
        auto temp = automata_repres_case_j(cat, swa, f);//, category_counter);
        ifs.push_back(temp);
    }
    if(ifs[0].substr(0, ELSE_SPACE_WIDTH + 1) == sp_else_sp){
        ifs[0] = ifs[0].substr(ELSE_SPACE_WIDTH + 1);
        // ELSE_SPACE_WIDTH + 1 --- длина строки " else "
    }
    for(const auto& x : ifs){
        result += triple_indent + x;
    }

    result += "\n" + triple_indent + "break;\n";
    return result;
}

std::string Main_parser::automata_repres_switch(const G_DFA& aut,
                                                const Str_data_for_automaton& f){
    std::string result;
    result               = indent + "switch(state){\n";
    size_t counter       = 0;
    last_category_suffix = 0;
    for(const auto& m : aut.jumps){
        result += automata_repres_case(m, f, counter);
        counter++;
    }
    result += double_indent + "default:\n" +
              triple_indent + ";\n" +
              indent        + "}\n";
    return result;
}

std::string Main_parser::check_there_is_jump(const Str_data_for_automaton& f){
    std::string result;
    result = indent        + "if(!there_is_jump){\n" +
             double_indent + "t = false;\n"          +
             double_indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
             triple_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
             triple_indent + "en->increment_number_of_errors();\n" + double_indent + "}\n";
    auto temp = f.final_actions;
    if(!temp.empty()){
        result += double_indent + temp + "\n";
    }
    result += indent + "}\n";
    return result;
}

std::string Main_parser::automata_repres(const G_DFA& aut,
                                         const Str_data_for_automaton& f){
    std::string result;

    std::string proc_def = "bool " + name_of_scaner_class + "::" + f.proc_name + "{\n" +
                           indent + "bool t             = true;\n" +
                           indent + "bool there_is_jump = false;\n";
    proc_def += automata_repres_switch(aut, f) + "\n";
    proc_def += check_there_is_jump(f) + "\n";
    proc_def += indent + "return t;\n}";
    result = str_repres_for_set_of_size_t_const(aut.final_states,
                                                f.final_states_set_name) +
             "\n\n" + proc_def;
    return result;
}

void Main_parser::generate_separate_identifier_automat(){
    auto ident_commands = regexp1_with_regexp2ast(id_begin, id_body);

    for(auto& com : ident_commands){
        com.action_name = write_action_name_idx;
    }

    G_DFA gdfa;
    grouped_DFA_by_regexp(gdfa, ident_commands);

    Str_data_for_automaton f;
    f.automata_name         = possible_automata_name_str[Id_aut];
    f.proc_name             = possible_proc_ptr[Id_aut];
    f.category_name_prefix  = "IDENTIFIER";
    f.diagnostic_msg        = "В строке %zu неожиданно закончился идентификатор.";
    f.final_states_set_name = "final_states_for_identiers";
    f.final_actions         = "token.ident_index = ids -> insert(buffer);";

    aut_impl[Id_aut] = automata_repres(gdfa, f);
    auto temp = "void " + name_of_scaner_class + "::" + possible_fin_proc_ptr[Id_aut] + "{\n" +
                indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
                double_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
                double_indent + "en->increment_number_of_errors();\n" +
                indent + "}\n" +
                indent + f.final_actions + "\n}";
    aut_impl_fin_proc[Id_aut] = temp;

    auto symbols = begin_chars[Id_beg_char].s;
    auto cat_res = add_category(symbols, identifier_begin_cat_name_by_default);
    auto id_begin_category_name = cat_res.second;

    aut_impl[Start_aut] += "\n    if(belongs(" + id_begin_category_name +
        ", char_categories)){\n        (loc->pcurrent_char)--; " +
        "automaton = A_id;\n        state = 0;\n        return t;\n    }\n";
}

Command_buffer u32string_to_commands(const std::u32string& str){
    Command_buffer result;

    if(str.empty()){
        return result;
    }

    Command command;
    command.action_name = 0;
    command.name        = Cmd_char_def;
    command.c           = str[0];
    result.push_back(command);

    auto temp = str.substr(1);
    if(temp.empty()){
        return result;
    }

    for(char32_t ch : temp){
        command.action_name      = 0;
        command.name             = Cmd_char_def;
        command.c                = ch;

        size_t concat_first_arg  = result.size() - 1;
        result.push_back(command);
        size_t concat_second_arg = result.size() - 1;

        command.name             = Cmd_concat;
        command.args.first       = concat_first_arg;
        command.args.second      = concat_second_arg;
        result.push_back(command);
    }
    return result;
}

/* Возвращает команды, соответствующие регулярке a | b. */
Command_buffer regexp1_or_regexp2(const Command_buffer& a, const Command_buffer& b){
    if(a.empty()){
        return b;
    }
    if(b.empty()){
        return a;
    }

    Command_buffer result            = a;
    size_t         num_commands_in_a = a.size();
    size_t         or_first_arg      = num_commands_in_a - 1;

    Command_buffer temp              = b;
    for(auto& c : temp){
        switch(c.name){
            case Cmd_or: case Cmd_concat:
                c.args.first += num_commands_in_a; c.args.second += num_commands_in_a;
                break;
            case Cmd_Kleene: case Cmd_positive: case Cmd_optional:
                c.args.first += num_commands_in_a;
                break;
            default:
                ;
        }
    }

    result.insert(result.end(), temp.begin(), temp.end());
    size_t         or_second_arg     = result.size() - 1;

    Command command;
    command.action_name = 0;
    command.name        = Cmd_or;
    command.args.first  = or_first_arg;
    command.args.second = or_second_arg;
    result.push_back(command);

    return result;
}

Command_buffer u32strings_to_commands(const std::vector<std::u32string>& s){
    Command_buffer result;
    for(const auto& str : s){
        auto current_commands = u32string_to_commands(str);
        result                = regexp1_or_regexp2(result, current_commands);
    }
    return result;
}

using Keyword_and_code = std::pair<std::u32string, std::string>;
/* Первый элемент этой пары --- строковое представление ключевого слова,
 * а второй элемент --- строковое представление идентификатора, являющегося
 * соответствующим кодом лексемы. */

using Keywords_and_codes = std::vector<Keyword_and_code>;

std::string keyword_list_elem(const std::string& codes_n){
    std::string result = "struct Keyword_list_elem{\n    std::u32string keyword;\n    " +
                         codes_n + " kw_code;\n};";
    return result;
}

std::string keyword_list(const Keywords_and_codes& kwcs, const std::string& codes_n){
    std::string result = keyword_list_elem(codes_n) +
                         "\n\nstatic const Keyword_list_elem kwlist[] = {\n";

    std::vector<std::string> kwl;
    for(const auto& k : kwcs){
        auto temp = "{U\"" + u32string_to_utf8(k.first) + "\", " + k.second + "}";
        kwl.push_back(temp);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result += string_list_to_columns(kwl, f) + "\n};\n\n#define NUM_OF_KEYWORDS " +
              std::to_string(kwcs.size()) + "\n";

    return result;
}

static const std::string search_keyword_proc_text =
R"~(
#define THERE_IS_NO_KEYWORD (-1)

static int search_keyword(const std::u32string& finded_keyword){
    int result      = THERE_IS_NO_KEYWORD;
    int low_bound   = 0;
    int upper_bound = NUM_OF_KEYWORDS - 1;
    int middle;
    while(low_bound <= upper_bound){
        middle             = (low_bound + upper_bound) / 2;
        auto& curr_kw      = kwlist[middle].keyword;
        int compare_result = finded_keyword.compare(curr_kw);
        if(0 == compare_result){
            return middle;
        }
        if(compare_result < 0){
            upper_bound = middle - 1;
        }else{
            low_bound   = middle + 1;
        }
    }
    return result;
})~";

static std::string kwtable_data(const Keywords_and_codes& kwcs, const std::string& codes_n){
    auto result = keyword_list(kwcs, codes_n) + search_keyword_proc_text;
    return result;
}

static const std::string idkeyword_final_actions = R"~(
        int search_result = search_keyword(buffer);
        if(search_result != THERE_IS_NO_KEYWORD) {
            token.code = kwlist[search_result].kw_code;
        })~";

static const std::string idkeyword_final_actions1 = R"~(
    int search_result = search_keyword(buffer);
    if(search_result != THERE_IS_NO_KEYWORD) {
        token.code = kwlist[search_result].kw_code;
    }
)~";

static const std::string idkeyword_begin_cat_name_by_default = "IDKEYWORD_BEGIN";

void Main_parser::generate_idkeyword_automat(){
    auto ident_commands = regexp1_with_regexp2ast(id_begin, id_body);

    Keywords_and_codes kwcs;

    for(size_t kw_idx : kw_repres){
        auto keyword             = et_.strs_trie->get_string(kw_idx);
        keyword_strings.push_back(keyword);
        auto num_value_of_code   = (scope_->strsc[kw_idx]).code;
        auto str_repres_for_code = idx_to_string(et_.ids_trie, codes[num_value_of_code]);
        auto temp                = std::make_pair(keyword, str_repres_for_code);
        kwcs.push_back(temp);
    }

    std::sort(kwcs.begin(), kwcs.end(),
              [](const Keyword_and_code& k1, const Keyword_and_code& k2){
                  return k1.first < k2.first;
    });

    auto keywords_commands = u32strings_to_commands(keyword_strings);
    auto idkeyword_commands = regexp1_or_regexp2(ident_commands, keywords_commands);
    for(auto& com : idkeyword_commands){
        com.action_name = write_action_name_idx;
    }

    G_DFA gdfa;
    grouped_DFA_by_regexp(gdfa, idkeyword_commands);

    auto kw_postact = get_act_repres(keyword_postaction);
    Str_data_for_automaton f;
    f.automata_name         = possible_automata_name_str[IdKeyword_aut];
    f.proc_name             = possible_proc_ptr[IdKeyword_aut];
    f.category_name_prefix  = "IDKEYWORD";
    f.diagnostic_msg        =
        "В строке %zu неожиданно закончился идентификатор или ключевое слово.";
    f.final_states_set_name = "final_states_for_idkeywords";
    f.final_actions         = dindent(kw_postact) + idkeyword_final_actions;

    auto kwtable = kwtable_data(kwcs, idx_to_string(et_.ids_trie, codes_type_name_idx));

    using operations_with_sets::operator+;

    auto symbols = begin_chars[Id_beg_char].s + begin_chars[Keyword_beg_char].s;
    auto cat_res = add_category(symbols, idkeyword_begin_cat_name_by_default);
    auto idkeyword_begin_category_name = cat_res.second;

    aut_impl[Start_aut] += "\n    if(belongs(" + idkeyword_begin_category_name +
        ", char_categories)){\n        (loc->pcurrent_char)--; " +
        "automaton = A_idKeyword;\n        state = 0;\n        return t;\n    }\n";

    aut_impl[IdKeyword_aut] = kwtable + "\n\n" + automata_repres(gdfa, f);

    auto temp = "void " + name_of_scaner_class + "::" +
                possible_fin_proc_ptr[IdKeyword_aut] + "{\n" +
                indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
                double_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
                double_indent + "en->increment_number_of_errors();\n" +
                indent + "}\n" + sindent(kw_postact) + idkeyword_final_actions1 + "\n}";
    aut_impl_fin_proc[IdKeyword_aut] = temp;
}

void Main_parser::generate_idents_and_keywords_automata_impl(){
    bool t1 = belongs(Id_aut, set_of_used_automata)      != 0;
    bool t2 = belongs(Keyword_aut, set_of_used_automata) != 0;

    using operations_with_sets::operator*;
    using operations_with_sets::print_set;

    enum Idkw {
        There_is_no_id_and_there_are_no_keywords,
        There_is_no_id_and_there_are_keywords,
        There_is_id_and_there_are_no_keywords,
        There_is_id_and_there_are_keywords
    };
    Idkw t = static_cast<Idkw>(t1 * 2 + t2);
    switch(t){
        case There_is_no_id_and_there_are_no_keywords:
            break;
        case There_is_no_id_and_there_are_keywords:
            /* Обработка ключевых слов будет выполняться аналогично
             * обработке разделителей. */
            generate_separate_keywords_automat();
            break;
        case There_is_id_and_there_are_no_keywords:
            generate_separate_identifier_automat();
            break;
        case There_is_id_and_there_are_keywords:
            if(!(begin_chars[Keyword_beg_char] * begin_chars[Id_beg_char])){
                /* Если множество первых символов ключевых слов не пересекается с
                 * множеством первых символов идентификаторов, то создать отдельный
                 * автомат обработки идентификаторов, и отдельный автомат обработки
                 * ключевых слов, без возможности перехода между ними. При этом обработка
                 * ключевых слов будет выполняться аналогично обработке разделителей. */
                generate_separate_keywords_automat();
                generate_separate_identifier_automat();
            }else{
                /* Если же указанные множества пересекаются, то нужно склеить регулярку
                 * для идентификаторов с регуляркой для ключевых слов, создать по этой
                 * регулярке минимизированный детерминированный конечный автомат со
                 * сгруппированными переходами, и по получившемуся автомату нужно построить
                 * его реализацию. */
                set_of_used_automata &= ~((1ULL << Id_aut) | (1ULL << Keyword_aut));
                set_of_used_automata |= 1ULL << IdKeyword_aut;
                generate_idkeyword_automat();
            }
            break;
    }
}

void Main_parser::generate_unknown_automata_impl(){
    aut_impl[Unknown_aut] = "bool " + name_of_scaner_class + R"~(::unknown_proc(){
    return belongs(Other, char_categories);
})~";

    aut_impl_fin_proc[Unknown_aut] = "void " + name_of_scaner_class + R"~(::unknown_final_proc(){
    /* Данная подпрограмма будет вызвана, если по прочтении входного текста
     * оказались в автомате A_unknown. Тогда ничего делать не нужно. */
})~";
}

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

void Main_parser::generate_automata_impl(){
    aut_impl_fin_proc[Start_aut] = "void " + name_of_scaner_class + none_proc;
    generate_delim_automaton_impl();
    generate_strings_automaton_impl();
    generate_numbers_automaton_impl();
    generate_idents_and_keywords_automata_impl();
    generate_unknown_automata_impl();
}

using Category_name_and_set = std::pair<std::string, Set_of_char>;
using Categories_info       = std::vector<Category_name_and_set>;

/* Следующая функция по заданному символу типа char32_t строит множество категорий,
 * которым он принадлежит. Сведения о категориях передаются в векторе, элементы
 * которого имеют тип Category_name_and_set. Множество категорий представляется в
 * виде числа типа uint64_t: равенство нулю какого--либо разряда означает, что
 * категория с соответствующим номером этому множеству не принадлежит, а равенство
 * единице --- что принадлежит. */
uint64_t construct_categories_set_for_char(char32_t c,
                                           const Categories_info& categories_info)
{
    using operations_with_sets::is_elem;
    uint64_t result  = 0;
    size_t   counter = 0;
    for(const auto& z : categories_info){
        if(is_elem(c, z.second)){
            result |= 1ULL << counter;
        }
        counter++;
    }
    return result;
}

std::string construct_category_enum(const Categories_info& categories_info){
    std::string result = "enum Category {\n";
    std::vector<std::string> category_names;
    for(const auto& z : categories_info){
        category_names.push_back(z.first);
    }
    category_names.push_back("Other");
    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;
    result += string_list_to_columns(category_names, f)+ "\n};\n";
    return result;
}

static const std::map<char32_t, std::string> escapes = {
    {U'\n', R"~(U'\n')~"}, {U'\t', R"~(U'\t')~"}, {U'\'', R"~(U'\'')~"},
    {U'\"', R"~(U'\"')~"}, {U'\?', R"~(U'\?')~"}, {U'\\', R"~(U'\\')~"},
    {U' ',  R"~(U' ')~" }, {U'\r', R"~(U'\r')~"}, {U'\v', R"~(U'\v')~"},
    {U'\a', R"~(U'\a')~"}, {U'\b', R"~(U'\b')~"}, {U'\f', R"~(U'\f')~"}
};

std::string show_char32(char32_t c){
    std::string result;
    auto it = escapes.find(c);
    if(it != escapes.end()){
        result = it->second;
    }else{
        if(c < U' '){
            result = "U\\" + std::to_string(static_cast<unsigned>(c));
        }else{
            result = "U\'" + char32_to_utf8(c) + "\'";
        }
    }
    return result;
}

std::string category_table_string_repres(const std::map<char32_t, uint64_t>& t,
                                         size_t num_of_categories)
{
    std::string result;
    std::string type_for_set_repres;
    switch(num_of_categories){
        case 1 ... 8:
            type_for_set_repres = "uint8_t";
            break;

        case 9 ... 16:
            type_for_set_repres = "uint16_t";
            break;

        case 17 ... 32:
            type_for_set_repres = "uint32_t";
            break;

        case 33 ... 64:
            type_for_set_repres = "uint64_t";
            break;

        default:
            ;
    }
    result = "static const std::map<char32_t, " + type_for_set_repres +
             "> categories_table = {\n";

    std::vector<std::string> entries;
    for(const auto& z : t){
        auto entry = "{" + show_char32(z.first) + ", " + std::to_string(z.second) + "}";
        entries.push_back(entry);
    }
    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 4;
    f.spaces_between_columns = 1;

    result += string_list_to_columns(entries, f)+ "\n};\n";

    return result;
}

static const std::string get_category_func_str =
    R"~(
uint64_t get_categories_set(char32_t c){
    auto it = categories_table.find(c);
    if(it != categories_table.end()){
        return it->second;
    }else{
        return 1ULL << Other;
    }
}
)~";

std::string Main_parser::generate_category_table(){
    using operations_with_sets::operator+;
    std::string     result;
    Categories_info categories_info;
    Set_of_char     categorized_chars;
    for(const auto& c : category_name){
        /* перебираем имена всех категорий и записываем сведения о
         * категориях символов в вектор categories_info */
        auto cat_idx         = c.first;
        auto set_for_cat_idx = char_cat.get_set(cat_idx);
        auto x               = std::make_pair(c.second, set_for_cat_idx);
        categories_info.push_back(x);
        /* кроме того, собираем в одно множество все символы, входящие
         * в какую--либо категорию */
        categorized_chars    = categorized_chars + set_for_cat_idx;
    }

    /* теперь для каждого символа из множества categorized_chars строим
     * множество категорий, которым он принадлежит */
    std::map<char32_t, uint64_t> splitting_characters_by_category;
    for(char32_t c : categorized_chars){
        splitting_characters_by_category[c] =
            construct_categories_set_for_char(c, categories_info);
    }

    /* затем создаём перечисление из имён категорий символов */
    auto category_enum = construct_category_enum(categories_info);
    auto cat_table_str =
        category_table_string_repres(splitting_characters_by_category,
                                     categories_info.size());

    result = category_enum + "\n" + cat_table_str + "\n" +
             get_category_func_str;
    return result;
}

static const std::string current_lexem_without_comments_proc = R"~(::current_lexem(){
    automaton = A_start; token.code = None;
    lexem_begin = loc->pcurrent_char;
    bool t = true;
    while((ch = *(loc->pcurrent_char)++)){
        char_categories = get_categories_set(ch); //categories_table[ch];
        t = (this->*procs[automaton])();
        if(!t){
            /* Сюда попадаем, лишь если лексема уже прочитана. При этом текущим
             * автоматом уже прочитан символ, идущий сразу за концом прочитанной
             * лексемы, на основании этого символа принято решение о том, что
             * лексема прочитана, и совершён переход к следующему за ним символу.
             * Поэтому для того, чтобы не пропустить первый символ следующей
             * лексемы, нужно уменьшить на единицу указатель pcurrent_char. */
            (loc->pcurrent_char)--;
            return token;
        }
    }
    /* Здесь можем оказаться, только если уже прочли весь обрабатываемый текст.
     * При этом указатель на текущий символ указывает на байт, который находится
     * сразу же после нулевого символа, являющегося признаком конца текста.
     * Чтобы не выйти при последующих вызовах за пределы текста, нужно перейти
     * обратно к нулевому символу. */
    (loc->pcurrent_char)--;
    /* Далее, поскольку мы находимся здесь, то конец текущей лексемы (возможно,
     * неожиданный) ещё не обработан. Надо эту обработку выполнить, и, возможно,
     * вывести какую-то диагностику. */
    (this->*finals[automaton])();
    return token;
}

)~";

std::string Main_parser::current_lexem_without_comments()
{
    std::string result;
    result = lexem_info_name + " " + name_of_scaner_class +
             current_lexem_without_comments_proc;
    return result;
}

static const std::string current_lexem__without_comments_proc = R"~(::current_lexem_(){
    automaton = A_start; token.code = None;
    lexem_begin = loc->pcurrent_char;
    bool t = true;
    while((ch = *(loc->pcurrent_char)++)){
        char_categories = get_categories_set(ch); //categories_table[ch];
        t = (this->*procs[automaton])();
        if(!t){
            /* Сюда попадаем, лишь если лексема уже прочитана. При этом текущим
             * автоматом уже прочитан символ, идущий сразу за концом прочитанной
             * лексемы, на основании этого символа принято решение о том, что
             * лексема прочитана, и совершён переход к следующему за ним символу.
             * Поэтому для того, чтобы не пропустить первый символ следующей
             * лексемы, нужно уменьшить на единицу указатель pcurrent_char. */
            (loc->pcurrent_char)--;
            return token;
        }
    }
    /* Здесь можем оказаться, только если уже прочли весь обрабатываемый текст.
     * При этом указатель на текущий символ указывает на байт, который находится
     * сразу же после нулевого символа, являющегося признаком конца текста.
     * Чтобы не выйти при последующих вызовах за пределы текста, нужно перейти
     * обратно к нулевому символу. */
    (loc->pcurrent_char)--;
    /* Далее, поскольку мы находимся здесь, то конец текущей лексемы (возможно,
     * неожиданный) ещё не обработан. Надо эту обработку выполнить, и, возможно,
     * вывести какую-то диагностику. */
    (this->*finals[automaton])();
    return token;
}

)~";

static const std::string omit_singlelined_comment_proc_str =
    R"~(::omit_singlelined_comment(){
    while((ch = (loc->pcurrent_char)++)){
        if('\n' == ch){
            (loc->pcurrent_char)--;
            return;
        }
    }
    (loc->pcurrent_char)--;
    return;
}

)~";

static const std::string current_lexem_with_omitting_singlelined =
    R"~(::current_lexem(){
    for( ; ; ){
        auto l = current_lexem_();
        if(l.code == SINGLE_LINED_COMMENT_MARK){
            omit_singlelined_comment();
        }else{
            return token;
        }
    }
    return token;
}

)~";

static const std::string current_lexem_with_omitting_multilined =
    R"~(::current_lexem(){
    for( ; ; ){
        auto l = current_lexem_();
        switch(l.code){
            case MULTI_LINED_COMMENT_MARK:
                omit_multilined_comment();
                break;
            case MULTI_LINED_COMMENT_END:
                printf("Неожиданный конец многострочного комментария в строке %zu.\n",
                       lexem_begin_line_number());
                en->increment_number_of_errors();
                break;
            default:
                return token;
        }
    }
    return token;
}

)~";

static const std::string current_lexem_with_omitting_all =
    R"~(::current_lexem(){
    for( ; ; ){
        auto l = current_lexem_();
        switch(l.code){
            case SINGLE_LINED_COMMENT_MARK:
                omit_singlelined_comment();
                break;
            case MULTI_LINED_COMMENT_MARK:
                omit_multilined_comment();
                break;
            case MULTI_LINED_COMMENT_END:
                printf("Неожиданный конец многострочного комментария в строке %zu.\n",
                       lexem_begin_line_number());
                en->increment_number_of_errors();
                break;
            default:
                return token;
        }
    }
    return token;
}

)~";

static const std::string multilined_comment_jump =
    R"~(struct Jump_for_multilined_end{
    uint32_t next_state;
    char32_t jump_char;
};

static const Jump_for_multilined_end multilined_jumps[] = {
)~";

static const std::string omit_not_nested_multilined_comment_proc_str =
    R"~(::omit_multilined_comment(){
    size_t st = 0;
    while((ch = (loc->pcurrent_char)++)){
        auto j  = multilined_jumps[st];
        auto jc = j.jump_char;
        if(!jc){
            (loc->pcurrent_char)--;
            return;
        }
        if(ch == jc){
            st = j.next_state;
        }else{
            st = 0;
        }
    }
    printf("В строке %zu неожиданно закончился многострочный комментарий.\n",
           lexem_begin_line_number());
    en->increment_number_of_errors();
    return;
})~";

std::string Main_parser::there_is_only_singlelined()
{
    std::string result;
    result = "void " + name_of_scaner_class + omit_singlelined_comment_proc_str +
             lexem_info_name + name_of_scaner_class + current_lexem__without_comments_proc +
             lexem_info_name + name_of_scaner_class + current_lexem_with_omitting_singlelined;
    return result;
}

std::string Main_parser::there_is_only_multilined(){
    std::string result;
    result = "void " + name_of_scaner_class + omit_multilined_comment_proc() +
             lexem_info_name + name_of_scaner_class + current_lexem__without_comments_proc +
             lexem_info_name + name_of_scaner_class + current_lexem_with_omitting_multilined;
    return result;
}

std::string Main_parser::there_are_all_comments(){
    std::string result;
    result = "void " + name_of_scaner_class + omit_multilined_comment_proc() +
             "void " + name_of_scaner_class + omit_singlelined_comment_proc_str +
             lexem_info_name + name_of_scaner_class + current_lexem__without_comments_proc +
             lexem_info_name + name_of_scaner_class + current_lexem_with_omitting_all;
    return result;
}

std::string Main_parser::omit_multilined_comment_proc(){
    std::string result;
    result = multilined_is_nested ? omit_nested_multilined() : omit_not_nested_multilined();
    return result;
}

std::string Main_parser::omit_all_comment_proc(){
    std::string result;
    result = "void " + name_of_scaner_class + omit_multilined_comment_proc() +
             "void " + name_of_scaner_class + omit_singlelined_comment_proc_str +
             lexem_info_name + name_of_scaner_class + current_lexem__without_comments_proc +
             lexem_info_name + name_of_scaner_class + current_lexem_with_omitting_all;
    return result;
}

#define BEGIN_MARKER 1
#define END_MARKER   2

std::string generate_init_table_for_nested_comments(const Jumps_and_inits& ji){
    std::string result;
    std::vector<std::string> table_elems;
    auto& inits = ji.init_table;
    for(const auto& i : inits){
        std::string elem = "{" + std::to_string(i.first) + ", " +
                           char32_to_utf8(i.second) + "}";
        table_elems.push_back(elem);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 1;

    result = "static const State_for_char nested_comments_init_table[] = {\n";
    result += string_list_to_columns(table_elems, f) + "\n};\n\n"
              "#define NUM_OF_ELEMS_OF_COMM_INIT_TABLE " +
              std::to_string(table_elems.size()) + "\n\n";
    return result;
}

static const std::string nested_comment_jump_struct =
    R"~(struct Comment_jump{
    /** Указатель на строку , состоящую из символов , по которым возможен переход. */
    char32_t*       symbols;
    uint16_t        marker;
    /** Если текущий символ совпадает с symbols[0], то
        выполняется переход в состояние first_state;
        если текущий символ совпадает с symbols[1], то
        выполняется переход в состояние first_state+1;
        если текущий символ совпадает с symbols[2], то
        выполняется переход в состояние first_state+2,
        и так далее. */
    uint16_t        first_state;
};

static const Comment_jump comments_jump_table[] = {\n)~";

std::string generate_jump_table_for_nested_comments(const Jumps_and_inits& ji){
    std::string result;
    auto& jmps = ji.jumps;
    std::vector<std::string> jmp_table_body;
    for(const auto& j : jmps){
        std::string jump = "{const_cast<char32_t*>(U\"" +
                           u32string_to_utf8(j.jump_chars) + "\"), " +
                           std::to_string(j.code) + ", " +
                           std::to_string(j.first_state) + "}";
        jmp_table_body.push_back(jump);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 1;

    result = nested_comment_jump_struct +
             string_list_to_columns(jmp_table_body, f) + "\n};\n\n";
    return result;
}

std::string generate_table_for_nested_comments(const Jumps_and_inits& ji){
    std::string result;
    result = generate_init_table_for_nested_comments(ji) +
             generate_jump_table_for_nested_comments(ji) +
             "#define BEGIN_MARKER 1\n" "#define END_MARKER 2\n\n";
    return result;
}

static const std::string omit_nested_multilined_proc_str =
    R"~(::omit_multilined_comment(){
    ssize_t st     = -1;
    comment_level  = 1;
    uint16_t m;
    while((ch = (loc->pcurrent_char)++)){
        if(-1 == st){
            st = get_init_state(ch, nested_comments_init_table,
                                NUM_OF_ELEMS_OF_COMM_INIT_TABLE);
            continue;
        }
        auto j   = comments_jump_table[st];
        int  idx = search_char(ch, j.symbols);
        m        = j.marker;
        if(idx != THERE_IS_NO_CHAR){
            st = j.first_state + idx;
        }else{
            switch(m){
                case BEGIN_MARKER:
                    comment_level++;
                    (loc->pcurrent_char)--;
                    st = -1;
                    break;
                case END_MARKER:
                    comment_level--;
                    (loc->pcurrent_char)--;
                    st = -1;
                    if(0 == comment_level){
                        return;
                    }
                    break;
                default:
                    (loc->pcurrent_char)--;
                    st = -1;
            }
        }
    }
    if(-1 == st){
        printf("В строке %zu неожиданно закончился многострочный комментарий.\n",
               lexem_begin_line_number());
        en->increment_number_of_errors();
    }else{
        auto j   = comments_jump_table[st];
        m        = j.marker;
        switch(m){
            case BEGIN_MARKER:
                comment_level++;
                (loc->pcurrent_char)--;
                break;
            case END_MARKER:
                comment_level--;
                (loc->pcurrent_char)--;
                break;
            default:
                (loc->pcurrent_char)--;
                printf("В строке %zu неожиданно закончился многострочный комментарий.\n",
                       lexem_begin_line_number());
                en->increment_number_of_errors();
                return;
        }
        if(comment_level != 0){
            printf("В строке %zu неожиданно закончился многострочный комментарий.\n",
                   lexem_begin_line_number());
            en->increment_number_of_errors();
        }
    }
}

)~";

std::string Main_parser::omit_nested_multilined(){
    std::string          result;
    Attributed_char_trie atrie;
    Attributed_cstring   atrib_cstr_b;
    Attributed_cstring   atrib_cstr_e;

    auto marker_b = et_.strs_trie->get_string(mark_of_multilined_begin);
    auto marker_e = et_.strs_trie->get_string(mark_of_multilined_end);

    atrib_cstr_b.str       = const_cast<char32_t*>(marker_b.c_str());
    atrib_cstr_e.str       = const_cast<char32_t*>(marker_e.c_str());
    atrib_cstr_b.attribute = BEGIN_MARKER;
    atrib_cstr_e.attribute = END_MARKER;
    atrie.insert(attributed_cstring2string(atrib_cstr_b, 0));
    atrie.insert(attributed_cstring2string(atrib_cstr_e, 0));

    Jumps_and_inits jmps = atrie.jumps();

    result = generate_table_for_nested_comments(jmps) +
             "void " + name_of_scaner_class + omit_nested_multilined_proc_str;
    return result;
}

std::string table_for_not_nested_multilined(const std::u32string& end_of_comment){
    std::string result;

    std::vector<std::string> table_body;
    uint32_t st = 0;
    for(const auto c : end_of_comment){
        st++;
        auto temp = "{" + std::to_string(st) + ", " + show_char32(c) + "}";
        table_body.push_back(temp);
    }
    auto last_elem = "{" + std::to_string(0) + ", " + show_char32(0) + "}";

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 4;
    f.spaces_between_columns = 1;

    result = multilined_comment_jump + string_list_to_columns(table_body, f) + "\n};\n\n";
    return result;
}

std::string Main_parser::omit_not_nested_multilined(){
    std::string result;
    auto multilined_end = et_.strs_trie->get_string(mark_of_multilined_end);
    result = table_for_not_nested_multilined(multilined_end)
             + "void " + name_of_scaner_class + omit_not_nested_multilined_comment_proc_str;
    return result;
}

std::string Main_parser::generate_current_lexem_proc(){
    std::string result;

    enum Comment_kind {
        No_comments,              There_is_only_singlelined,
        There_is_only_multilined, There_are_all
    };

    bool t1 = mark_of_single_lined     != 0;
    bool t2 = mark_of_multilined_begin != 0;

    Comment_kind k = static_cast<Comment_kind>(t2 * 2 + t1);

    switch(k){
        case No_comments:
            result = current_lexem_without_comments();
            break;
        case There_is_only_singlelined:
            result                       = there_is_only_singlelined();
            fields_for_comments_handling = "\n" + indent + lexem_info_name +
                                         + " " + "current_lexem_();\n"
                                         + indent + "void omit_singlelined_comment();";
            break;
        case There_is_only_multilined:
            result                       = there_is_only_multilined();
            fields_for_comments_handling = "\n" + indent + lexem_info_name +
                                         + " " + "current_lexem_();\n"
                                         + indent + "void omit_multilined_comment();";
            if(multilined_is_nested){
                fields_for_comments_handling += "\n" + indent + "int comment_level;";
            }
            break;
        case There_are_all:
            result                       = there_are_all_comments();
            fields_for_comments_handling = "\n" + indent + lexem_info_name +
                                         + " " + "current_lexem_();\n"
                                         + indent + "void omit_singlelined_comment();\n"
                                         + indent + "void omit_multilined_comment();";
            if(multilined_is_nested){
                fields_for_comments_handling += "\n" + indent + "int comment_level;";
            }
            break;
    }

    return result;
}

static const std::string start_proc_end =
    R"~(
    automaton = A_unknown;
    return t;
})~";

std::string Main_parser::collect_automata_impls(){
    std::string result;
    aut_impl[Start_aut] += start_proc_end;
    for(const auto s : aut_impl){
        result += s.second + "\n\n";
    }
    for(const auto s : aut_impl_fin_proc){
        result += s.second + "\n\n";
    }
    return result;
}

void Main_parser::generate_scaner_implementation(){
    std::string impl_file_name = scaner_file_name_without_ext + ".cpp";
    std::string header_name    = scaner_file_name_without_ext + ".h";
    std::string impl_text      = impl_includes(header_name) + procs_tables();

    impl_text += generate_category_table() + collect_automata_impls() +
                 generate_current_lexem_proc();

    FILE* fptr = fopen(impl_file_name.c_str(), "w");
    if(fptr){
        fputs(impl_text.c_str(), fptr);
        fputs("\n",fptr);
        fclose(fptr);
    }else{
        printf("Не удалось создать файл реализации сканера.\n");
        et_.ec -> increment_number_of_errors();
    }
}

int Main_parser::get_number_of_errors(){
    return et_.ec -> get_number_of_errors();
}