/*
    Файл:    implement_scaner.cpp
    Создан:  06 февраля 2017г. в 11:55 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/implement_scaner.h"
#include "../include/list_to_columns.h"
#include "../include/indent.h"
#include "../include/generate_category_table.h"
#include "../include/attributed_char_trie.h"
#include "../include/char_conv.h"
#include "../include/print_char32.h"
#include <cstdio>
#include <string>

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

static const std::string start_proc_end =
    R"~(
    automaton = A_unknown;
    return t;
})~";

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

static const std::string multilined_comment_jump =
    R"~(struct Jump_for_multilined_end{
    uint32_t next_state;
    char32_t jump_char;
};

static const Jump_for_multilined_end multilined_jumps[] = {
)~";

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

#define BEGIN_MARKER 1
#define END_MARKER   2

static std::string current_lexem_without_comments(Info_for_constructing& info)
{
    std::string result;
    result = info.lexem_info_name + " " + info.name_of_scaner_class +
             current_lexem_without_comments_proc;
    return result;
}

static std::string there_is_only_singlelined(Info_for_constructing& info)
{
    std::string result;
    result = "void " + info.name_of_scaner_class + omit_singlelined_comment_proc_str +
             info.lexem_info_name + " " + info.name_of_scaner_class +
             current_lexem__without_comments_proc +
             info.lexem_info_name + " " + info.name_of_scaner_class +
             current_lexem_with_omitting_singlelined;
    return result;
}

static std::string generate_init_table_for_nested_comments(const Jumps_and_inits& ji){
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

static std::string generate_table_for_nested_comments(const Jumps_and_inits& ji){
    std::string result;
    result = generate_init_table_for_nested_comments(ji) +
             generate_jump_table_for_nested_comments(ji) +
             "#define BEGIN_MARKER 1\n" "#define END_MARKER 2\n\n";
    return result;
}

static std::string omit_nested_multilined(Info_for_constructing& info){
    std::string          result;
    Attributed_char_trie atrie;
    Attributed_cstring   atrib_cstr_b;
    Attributed_cstring   atrib_cstr_e;

    auto marker_b = info.et.strs_trie->get_string(info.mark_of_multilined_begin);
    auto marker_e = info.et.strs_trie->get_string(info.mark_of_multilined_end);

    atrib_cstr_b.str       = const_cast<char32_t*>(marker_b.c_str());
    atrib_cstr_e.str       = const_cast<char32_t*>(marker_e.c_str());
    atrib_cstr_b.attribute = BEGIN_MARKER;
    atrib_cstr_e.attribute = END_MARKER;
    atrie.insert(attributed_cstring2string(atrib_cstr_b, 0));
    atrie.insert(attributed_cstring2string(atrib_cstr_e, 0));

    Jumps_and_inits jmps = atrie.jumps();

    result = generate_table_for_nested_comments(jmps) +
             "void " + info.name_of_scaner_class + omit_nested_multilined_proc_str;
    return result;
}

static std::string table_for_not_nested_multilined(const std::u32string& end_of_comment){
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

static std::string omit_not_nested_multilined(Info_for_constructing& info){
    std::string result;
    auto multilined_end = info.et.strs_trie->get_string(info.mark_of_multilined_end);
    result = table_for_not_nested_multilined(multilined_end)
             + "void " + info.name_of_scaner_class + omit_not_nested_multilined_comment_proc_str;
    return result;
}

static std::string omit_multilined_comment_proc(Info_for_constructing& info){
    std::string result;
    result = info.multilined_is_nested ?
             omit_nested_multilined(info) : omit_not_nested_multilined(info);
    return result;
}

static std::string there_is_only_multilined(Info_for_constructing& info){
    std::string result;
    result = "void " + info.name_of_scaner_class + omit_multilined_comment_proc(info) +
             info.lexem_info_name + " " + info.name_of_scaner_class +
             current_lexem__without_comments_proc +
             info.lexem_info_name + " " + info.name_of_scaner_class +
             current_lexem_with_omitting_multilined;
    return result;
}

static std::string there_are_all_comments(Info_for_constructing& info){
    std::string result;
    result = "void " + info.name_of_scaner_class + omit_multilined_comment_proc(info) +
             "void " + info.name_of_scaner_class + omit_singlelined_comment_proc_str +
             info.lexem_info_name + info.name_of_scaner_class + current_lexem__without_comments_proc +
             info.lexem_info_name + info.name_of_scaner_class + current_lexem_with_omitting_all;
    return result;
}

static std::string generate_current_lexem_proc(Info_for_constructing& info){
    std::string result;

    enum Comment_kind {
        No_comments,              There_is_only_singlelined,
        There_is_only_multilined, There_are_all
    };

    bool t1 = info.mark_of_single_lined     != 0;
    bool t2 = info.mark_of_multilined_begin != 0;

    Comment_kind k = static_cast<Comment_kind>(t2 * 2 + t1);

    switch(k){
        case No_comments:
            result = current_lexem_without_comments(info);
            break;
        case There_is_only_singlelined:
            result                       = there_is_only_singlelined(info);
            info.fields_for_comments_handling = "\n" + indent + info.lexem_info_name +
                                                " " + "current_lexem_();\n" +
                                                indent + "void omit_singlelined_comment();";
            break;
        case There_is_only_multilined:
            result                       = there_is_only_multilined(info);
            info.fields_for_comments_handling = "\n" + indent + info.lexem_info_name +
                                         + " " + "current_lexem_();\n"
                                         + indent + "void omit_multilined_comment();";
            if(info.multilined_is_nested){
                info.fields_for_comments_handling += "\n" + indent + "int comment_level;";
            }
            break;
        case There_are_all:
            result                       = there_are_all_comments(info);
            info.fields_for_comments_handling = "\n" + indent + info.lexem_info_name +
                                         + " " + "current_lexem_();\n"
                                         + indent + "void omit_singlelined_comment();\n"
                                         + indent + "void omit_multilined_comment();";
            if(info.multilined_is_nested){
                info.fields_for_comments_handling += "\n" + indent + "int comment_level;";
            }
            break;
    }

    return result;
}

static std::string automata_table(const Info_for_constructing& info){
    std::string result = info.name_of_scaner_class + "::Automaton_proc " +
                         info.name_of_scaner_class + "::procs[] = {\n";

    std::vector<std::string> procs_list;
    for(const auto ap : info.automaton_info){
        procs_list.push_back(ap.proc_ptr);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result += string_list_to_columns(procs_list, f) + "\n};";
    return result;
}

static std::string final_table(const Info_for_constructing& info){
    std::string result = info.name_of_scaner_class + "::Final_proc " +
                         info.name_of_scaner_class + "::finals[] = {\n";

    std::vector<std::string> fprocs_list;
    for(const auto ap : info.automaton_info){
        fprocs_list.push_back(ap.fin_proc_ptr);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result += string_list_to_columns(fprocs_list, f) + "\n};";
    return result;
}

static std::string procs_tables(const Info_for_constructing& info){
    std::string result = automata_table(info) + "\n\n" + final_table(info) + "\n\n";
    return result;
}

static std::string collect_automata_impls(Info_for_constructing& info){
    std::string result;
    info.aut_impl[Start_aut] += start_proc_end;
    for(const auto s : info.aut_impl){
        result += s.second + "\n\n";
    }
    for(const auto s : info.aut_impl_fin_proc){
        result += s.second + "\n\n";
    }
    return result;
}

void implement_scaner(Info_for_constructing& info){
//     std::string impl_file_name = scaner_file_name_without_ext + ".cpp";
//     std::string header_name    = scaner_file_name_without_ext + ".h";
    std::string impl_text      = impl_includes(info.header_name) + procs_tables(info);

    impl_text += generate_category_table(info) + collect_automata_impls(info) +
                 generate_current_lexem_proc(info);

    FILE* fptr = fopen(info.impl_file_name.c_str(), "w");
    if(fptr){
        fputs(impl_text.c_str(), fptr);
        fputs("\n",fptr);
        fclose(fptr);
    }else{
        printf("Не удалось создать файл реализации сканера.\n");
        info.et.ec -> increment_number_of_errors();
    }
}