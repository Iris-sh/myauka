/*
    File:    generate_idents_and_keywords_automata_impl.cpp
    Created: 05 February 2017 at 10:45 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/generate_idents_and_keywords_automata_impl.h"
#include "../include/belongs.h"
#include "../include/operations_with_sets.h"
#include "../include/attributed_char_trie.h"
#include "../include/add_category.h"
#include "../include/get_act_repres.h"
#include "../include/regexp1_with_regexp2ast.h"
#include "../include/indent.h"
#include "../include/grouped_DFA_by_uwrapped_regexp.h"
#include "../include/groupped_dfa.h"
#include "../include/automata_repres.h"
#include "../include/idx_to_string.h"
#include "../include/regexp1_or_regexp2.h"
#include "../include/u32strings_to_commands.h"
#include "../include/list_to_columns.h"
#include "../include/char_conv.h"
#include "../include/begin_chars_for_aut.h"
#include "../include/jump_table_string_repres.h"
#include <string>
#include <vector>
#include <utility>

static const std::string keywords_begin_cat_name_by_default   = "KEYWORD_BEGIN";
static const std::string identifier_begin_cat_name_by_default = "IDENTIFIER_BEGIN";
static const std::string keyword_jump_table_name              = "keyword_jump_table";
static const std::string keyword_init_table_name              = "init_table_for_keywords";
static const std::string separate_keyword_proc_body_          = R"~(::keyword_proc(){
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
static const std::string idkeyword_begin_cat_name_by_default  = "IDKEYWORD_BEGIN";

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

static std::string dindent(const std::string& s){
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


using Keyword_and_code = std::pair<std::u32string, std::string>;
/* The first element of this pair is a string representation of the keyword, and the
 * second element is a string representation of the identifier that is the
 * corresponding lexeme code. */

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

static std::string kwtable_data(const Keywords_and_codes& kwcs, const std::string& codes_n){
    auto result = keyword_list(kwcs, codes_n) + search_keyword_proc_text;
    return result;
}

static void generate_idkeyword_automat(Info_for_constructing& info){
    auto ident_commands = regexp1_with_regexp2ast(info.id_begin, info.id_body);

    Keywords_and_codes          kwcs;
    std::vector<std::u32string> keyword_strings;
    for(size_t kw_idx : info.kw_repres){
        auto keyword             = info.et.strs_trie->get_string(kw_idx);
        keyword_strings.push_back(keyword);
        auto num_value_of_code   = (info.scope->strsc[kw_idx]).code;
        auto str_repres_for_code = idx_to_string(info.et.ids_trie, info.codes[num_value_of_code]);
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
        com.action_name = info.write_action_name_idx;
    }

    G_DFA gdfa;
    grouped_DFA_by_uwrapped_regexp(gdfa, idkeyword_commands, true);

    auto kw_postact = get_act_repres(info, info.keyword_postaction);
    Str_data_for_automaton f;
    f.automata_name         = info.possible_automata_name_str[IdKeyword_aut];
    f.proc_name             = info.possible_proc_ptr[IdKeyword_aut];
    f.category_name_prefix  = "IDKEYWORD";
    f.diagnostic_msg        = "At line %zu unexpectedly ended identifier or keyword.";
    f.final_states_set_name = "final_states_for_idkeywords";
    f.final_actions         = dindent(kw_postact) + idkeyword_final_actions;

    auto kwtable = kwtable_data(kwcs, info.codes_type_name);

    using operations_with_sets::operator+;

    auto symbols = info.begin_chars[Id_beg_char].s + info.begin_chars[Keyword_beg_char].s;
    auto cat_res = add_category(info, symbols, idkeyword_begin_cat_name_by_default);
    auto idkeyword_begin_category_name = cat_res.second;

    info.aut_impl[Start_aut] += "\n    if(belongs(" + idkeyword_begin_category_name +
        ", char_categories)){\n        (loc->pcurrent_char)--; " +
        "automaton = A_idKeyword;\n        state = 0;\n        return t;\n    }\n";

    info.aut_impl[IdKeyword_aut] = kwtable + "\n\n" + automata_repres(info, gdfa, f);

    auto temp = "void " + info.name_of_scaner_class + "::" +
                info.possible_fin_proc_ptr[IdKeyword_aut] + "{\n" +
                indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
                double_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
                double_indent + "en->increment_number_of_errors();\n" +
                indent + "}\n" + sindent(kw_postact) + idkeyword_final_actions1 + "\n}";
    info.aut_impl_fin_proc[IdKeyword_aut] = temp;
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

static void generate_separate_identifier_automat(Info_for_constructing& info){
    auto ident_commands = regexp1_with_regexp2ast(info.id_begin, info.id_body);

    for(auto& com : ident_commands){
        com.action_name = info.write_action_name_idx;
    }

    G_DFA gdfa;
    grouped_DFA_by_uwrapped_regexp(gdfa, ident_commands, true);

    Str_data_for_automaton f;
    f.automata_name         = info.possible_automata_name_str[Id_aut];
    f.proc_name             = info.possible_proc_ptr[Id_aut];
    f.category_name_prefix  = "IDENTIFIER";
    f.diagnostic_msg        = "At line %zu unexpectedly ended identifier.";
    f.final_states_set_name = "final_states_for_identiers";
    f.final_actions         = "token.ident_index = ids -> insert(buffer);";

    info.aut_impl[Id_aut] = automata_repres(info, gdfa, f);
    auto temp = "void " + info.name_of_scaner_class + "::" +
                info.possible_fin_proc_ptr[Id_aut] + "{\n" +
                indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
                double_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
                double_indent + "en->increment_number_of_errors();\n" +
                indent + "}\n" +
                indent + f.final_actions + "\n}";
    info.aut_impl_fin_proc[Id_aut] = temp;

    auto symbols = info.begin_chars[Id_beg_char].s;
    auto cat_res = add_category(info, symbols, identifier_begin_cat_name_by_default);
    auto id_begin_category_name = cat_res.second;

    info.aut_impl[Start_aut] += "\n    if(belongs(" + id_begin_category_name +
        ", char_categories)){\n        (loc->pcurrent_char)--; " +
        "automaton = A_id;\n        state = 0;\n        return t;\n    }\n";
}

static void generate_separate_keywords_automat(Info_for_constructing& info){
    /* This function builds an implementation of a separate automaton that
     * processes keywords. */
    auto                 first_chars_for_keywords = info.begin_chars[Keyword_beg_char].s;
    Attributed_char_trie atrie;

    std::vector<std::u32string> keyword_strings;
    for(size_t kw_idx : info.kw_repres){
        auto keyword = info.et.strs_trie->get_string(kw_idx);
        keyword_strings.push_back(keyword);
    }

    size_t counter = 0;
    for(size_t kw_idx : info.kw_repres){
        Attributed_cstring atrib_cstr;
        atrib_cstr.str       = const_cast<char32_t*>(keyword_strings[counter].c_str());
        atrib_cstr.attribute = (info.scope->strsc[kw_idx]).code;
        atrie.insert(attributed_cstring2string(atrib_cstr, 1));
        counter++;
    }

    Jumps_and_inits jmps = atrie.jumps(); /* We built a workpiece for
                                             the transition table. */
    /* Now we need to add the desired text to the implementation of the start automaton
     * and generate a function that handles the keywords. */
    auto cat_res = add_category(info, first_chars_for_keywords,
                                keywords_begin_cat_name_by_default);
    std::string keyword_begin_cat_name = cat_res.second;

    info.aut_impl[Start_aut] += "\n    if(belongs(" + keyword_begin_cat_name +
        ", char_categories)){\n        (loc->pcurrent_char)--; " +
        "automaton = A_keyword;\n        state = -1;\n        return t;\n    }\n";

    auto kw_postact = get_act_repres(info, info.keyword_postaction);

    info.aut_impl[Keyword_aut] = jump_table_string_repres(info, jmps, keyword_jump_table_name,
                                                          keyword_init_table_name) +
                              "bool " + info.name_of_scaner_class +
                              separate_keyword_proc_body(kw_postact);

    info.aut_impl_fin_proc[Keyword_aut] = "void " + info.name_of_scaner_class +
                                          R"~(::keyword_final_proc(){
    )~" + indent + kw_postact +
    R"~(
    token.code = keyword_jump_table[state].code;
    )~" + "\n}";
}

void generate_idents_and_keywords_automata_impl(Info_for_constructing& info){
    bool t1 = belongs(Id_aut, info.set_of_used_automata)      != 0;
    bool t2 = belongs(Keyword_aut, info.set_of_used_automata) != 0;

    using operations_with_sets::operator*;

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
            /* Here keyword processing will be similar to the processing of delimiters. */
            generate_separate_keywords_automat(info);
            break;
        case There_is_id_and_there_are_no_keywords:
            generate_separate_identifier_automat(info);
            break;
        case There_is_id_and_there_are_keywords:
            if(!(info.begin_chars[Keyword_beg_char] * info.begin_chars[Id_beg_char])){
                /* If the set of first characters of keywords does not intersect with
                 * the set of first characters of identifier, then create a separate
                 * identifier handling automaton and a separate keyword processing
                 * automaton, without the possibility of switching between them. In
                 * this case, the processing of keywords will be performed similarly
                 * to the processing of delimiters. */
                generate_separate_keywords_automat(info);
                generate_separate_identifier_automat(info);
            }else{
                /* If the specified sets intersect, then we need to glue the regular
                 * expression for the identifiers with regular expression for the
                 * keywords, create a minimized deterministic finite automaton with the
                 * grouped transitions from this regular expression, and from the resulting
                 * automaton we need to build its implementation. */
                info.set_of_used_automata &= ~((1ULL << Id_aut) | (1ULL << Keyword_aut));
                info.set_of_used_automata |= 1ULL << IdKeyword_aut;
                generate_idkeyword_automat(info);
            }
            break;
    }
}