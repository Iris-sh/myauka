/*
    File:    jump_table_string_repres.cpp
    Created: 04 February 2017 at 11:43 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/jump_table_string_repres.h"
#include "../include/list_to_columns.h"
#include "../include/idx_to_string.h"
#include "../include/char_conv.h"
#include "../include/indent.h"
#include <string>

std::string generate_Elem(const std::string& s)
{
    std::string result = R"~(struct Elem {
    /** A pointer to a string of characters that can be crossed. */
    char32_t*       symbols;
    /** A lexeme code. */
    )~" + s + R"~( code;
    /** If the current character matches symbols[0], then the transition to the state
     *  first_state;
     *  if the current character matches symbols[1], then the transition to the state
     *  first_state + 1;
     *  if the current character matches symbols[2], then the transition to the state
     *  first_state + 2, and so on. */
    uint16_t        first_state;
};

)~";
    return result;
}

std::string delim_table_gen(const Info_for_constructing& info, const Jumps_and_inits& ji,
                            const std::string& table_name)
{
    std::string result = "static const Elem " + table_name + "[] = {\n";
    std::vector<std::string> del_jumps;
    for(const auto& j : ji.jumps){
        std::string temp = "{const_cast<char32_t*>(U\"" +
                           u32string_to_utf8(j.jump_chars) + "\"), " +
                           idx_to_string(info.et.ids_trie, info.codes[j.code]) + ", " +
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

static std::string init_table(const Jumps_and_inits& ji,
                              const std::string&     init_table_name){
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

std::string jump_table_string_repres(Info_for_constructing& info,
                                     const Jumps_and_inits& ji,
                                     const std::string&     table_name,
                                     const std::string&     init_table_name)
{
    auto result = init_table(ji, init_table_name);
    auto temp   = delim_table_gen(info, ji, table_name);
    if(!info.there_is_Elem_definition){
        result += generate_Elem(info.codes_type_name) + temp;
        info.there_is_Elem_definition = true;
    }else{
        result += temp;
    }
    return result;
}