/*
    Файл:    scaner_header.cpp
    Создан:  07 февраля 2017г. в 11:47 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/scaner_header.h"
#include "../include/conv_case.h"
#include "../include/idx_to_string.h"
#include "../include/indent.h"
#include "../include/list_to_columns.h"
#include "../include/belongs.h"
#include <string>
#include <vector>
#include <cstdio>

static const std::string header_includes =
    R"~(#include "../include/abstract_scaner.h"
#include "../include/error_count.h"
#include "../include/location.h"
#include <string>")~";

static std::string generate_lexem_info(Info_for_constructing& info){
    std::string s;
    s = "struct " + info.lexem_info_name + "{\n" +
        indent + info.codes_type_name +
        " code;\n" + indent + "union{\n";

    bool t = belongs(Id_aut, info.set_of_used_automata) ||
             belongs(IdKeyword_aut, info.set_of_used_automata);

    if(t){
        s += double_indent + "size_t    ident_index;\n";
    };
    if(belongs(String_aut, info.set_of_used_automata)){
        s += double_indent + "size_t    string_index;\n" +
             double_indent + "char32_t  c;\n";
    };

    s += idx_to_string(info.et.strs_trie, info.token_fields_idx) +
         "\n" + indent + "};\n};\n\n";
    return s;
}

static std::string generate_lexem_codes_enum(Info_for_constructing& info){
    std::string s;
    s = "enum " + info.codes_type_name + " : unsigned short {\n";
    std::vector<std::string> lexem_codes_names;
    for(const size_t c : info.codes){
        lexem_codes_names.push_back(idx_to_string(info.et.ids_trie,c));
    }
    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 0;
    s += string_list_to_columns(lexem_codes_names, f);
    s += "\n};\n\n";
    return s;
}

static std::string generate_automata_enum(Info_for_constructing& info){
    std::string s;
    s = indent + "enum Automaton_name{\n";
    std::vector<std::string> automata_names;
    for(const auto& ap : info.automaton_info){
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

static std::string generate_automata_proc_protos(Info_for_constructing& info){
    std::string s;
    std::vector<std::string> automata_protos;
    for(const auto ap : info.automaton_info){
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

static std::string generate_automata_final_procs_protos(Info_for_constructing& info){
    std::string s;
    std::vector<std::string> final_procs_protos;
    for(const auto ap : info.automaton_info){
        final_procs_protos.push_back(ap.fin_proc_proto);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    s += string_list_to_columns(final_procs_protos, f, 0);
    return s;
}

static std::string generate_scaner_class(Info_for_constructing& info){
    std::string scaner_class;
    scaner_class = "class " + info.name_of_scaner_class +
        " : public Abstract_scaner<" + info.lexem_info_name + "> {\npublic:\n" +
        indent + info.name_of_scaner_class + "() = default;\n" +
        indent + info.name_of_scaner_class +
        "(Location* location, const Errors_and_tries& et) :\n" +
        double_indent + "Abstract_scaner<" + info.lexem_info_name +
        ">(location, et) {};\n" +
        indent + info.name_of_scaner_class +"(const " +
        info.name_of_scaner_class + "& orig) = default;\n" +
        indent + "virtual ~" + info.name_of_scaner_class + "() = default;\n" +
        indent + "virtual " + info.lexem_info_name + " current_lexem();\n" +
        "private:\n";
    scaner_class += generate_automata_enum(info) +
                    automaton_procs_typedefs(info.name_of_scaner_class) +
                    generate_automata_proc_protos(info) +
                    generate_automata_final_procs_protos(info);
    scaner_class += "\n};\n";
    return scaner_class;
}

void scaner_header(Info_for_constructing& info){
    std::string sentinel = toupper_case(info.name_of_scaner_class) + "_H";

    std::string header = "#ifndef " + sentinel + "\n#define " +
                         sentinel + "\n\n" + header_includes + "\n\n";

    header += generate_lexem_codes_enum(info);
    header += generate_lexem_info(info);
    header += generate_scaner_class(info);

    if(!info.fields_for_comments_handling.empty()){
        header += "\n" + info.fields_for_comments_handling + "\n";
    }

    header += "#endif";

    FILE* fptr = fopen(info.header_name.c_str(), "w");
    if(fptr){
        fputs(header.c_str(), fptr);
        fputs("\n",fptr);
        fclose(fptr);
    }else{
        printf("Не удалось создать заголовочный файл сканера.\n");
        info.et.ec -> increment_number_of_errors();
    }
}