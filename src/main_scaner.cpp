/* 
    Файл:    main_scaner.cpp
    Создан:  14 декабря 2015г. в 15:25 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstdlib>
#include <cstdio>
#include <map>
#include "../include/main_scaner.h"
#include "../include/belongs.h"
#include "../include/search_char.h"
#include "../include/get_init_state.h"

static const std::map<char32_t, uint8_t> categories_table = {
    {   1,   1},  {   2,   1},  {   3,   1},  {   4,   1},  {   5,   1},  
    {   6,   1},  {   7,   1},  {   8,   1},  {   9,   1},  {  10,   1},  
    {  11,   1},  {  12,   1},  {  13,   1},  {  14,   1},  {  15,   1},  
    {  16,   1},  {  17,   1},  {  18,   1},  {  19,   1},  {  20,   1},  
    {  21,   1},  {  22,   1},  {  23,   1},  {  24,   1},  {  25,   1},  
    {  26,   1},  {  27,   1},  {  28,   1},  {  29,   1},  {  30,   1},  
    {  31,   1},  {  32,   1},  {U'"', 128},  {U'%',   4},  {U',',  64},  
    {U'0',  32},  {U'1',  32},  {U'2',  32},  {U'3',  32},  {U'4',  32},  
    {U'5',  32},  {U'6',  32},  {U'7',  32},  {U'8',  32},  {U'9',  32},  
    {U':',  64},  {U'A',  48},  {U'B',  48},  {U'C',  48},  {U'D',  48},  
    {U'E',  48},  {U'F',  48},  {U'G',  48},  {U'H',  48},  {U'I',  48},  
    {U'J',  48},  {U'K',  48},  {U'L',  48},  {U'M',  48},  {U'N',  48},  
    {U'O',  48},  {U'P',  48},  {U'Q',  48},  {U'R',  48},  {U'S',  48},  
    {U'T',  48},  {U'U',  48},  {U'V',  48},  {U'W',  48},  {U'X',  48},  
    {U'Y',  48},  {U'Z',  48},  {U'_',  48},  {U'a',  56},  {U'b',  48},  
    {U'c',  56},  {U'd',  56},  {U'e',  48},  {U'f',  48},  {U'g',  48},  
    {U'h',  48},  {U'i',  56},  {U'j',  48},  {U'k',  56},  {U'l',  48},  
    {U'm',  56},  {U'n',  56},  {U'o',  48},  {U'p',  48},  {U'q',  48},  
    {U'r',  48},  {U's',  56},  {U't',  56},  {U'u',  48},  {U'v',  48},  
    {U'w',  48},  {U'x',  48},  {U'y',  48},  {U'z',  48},  {U'{',  64},  
    {U'}',  64}
};

uint64_t Main_scaner::get_categories_set(char32_t c){
    auto it = categories_table.find(c);
    if(it != categories_table.end()){
        return it->second;
    }else{
        return 1ULL << Other;
    }
}

/**
 * Элемент таблицы переходов автомата обработки ключевых слов.
 */
struct Elem {
    /** Указатель на строку , состоящую из символов , по которым
        возможен переход. */
    char32_t*       symbols;
    /** код лексемы */
    Main_lexem_code code; 
    /** Если текущий символ совпадает с symbols[0], то
        выполняется переход в состояние first_state;
        если текущий символ совпадает с symbols[1], то
        выполняется переход в состояние first_state+1;
        если текущий символ совпадает с symbols[2], то
        выполняется переход в состояние first_state+2,
        и так далее. */
    uint16_t        first_state;
};
/* Для автомата обработки ключевых слов член state класса Main_scaner
 * является индексом элемента в таблице переходов, обозначенной
 * ниже как a_keyword_jump_table. */
static const Elem a_keyword_jump_table[] = {
    {const_cast<char32_t*>(U"c"),  M_Kw_action,          1},          // 0:   %a...
    {const_cast<char32_t*>(U"t"),  M_Kw_action,          2},          // 1:   %ac...
    {const_cast<char32_t*>(U"i"),  M_Kw_action,          3},          // 2:   %act...
    {const_cast<char32_t*>(U"o"),  M_Kw_action,          4},          // 3:   %acti...
    {const_cast<char32_t*>(U"n"),  M_Kw_action,          5},          // 4:   %actio...
    {const_cast<char32_t*>(U""),   Kw_action,            0},          // 5:   %action

    {const_cast<char32_t*>(U"lo"), M_Kw_codes,           7},          // 6:   %c...
    {const_cast<char32_t*>(U"a"),  M_Kw_class_members,   9},          // 7:   %cl...
    {const_cast<char32_t*>(U"dm"), M_Kw_codes,           10},         // 8:   %co...
    {const_cast<char32_t*>(U"s"),  M_Kw_class_members,   12},         // 9:   %cla...
    {const_cast<char32_t*>(U"e"),  M_Kw_codes,           13},         // 10:  %cod...
    {const_cast<char32_t*>(U"m"),  M_Kw_comments,        14},         // 11:  %com...
    {const_cast<char32_t*>(U"s"),  M_Kw_class_members,   15},         // 12:  %clas...
    {const_cast<char32_t*>(U"s"),  M_Kw_codes,           16},         // 13:  %code...
    {const_cast<char32_t*>(U"e"),  M_Kw_comments,        17},         // 14:  %comm...
    {const_cast<char32_t*>(U"_"),  M_Kw_class_members,   18},         // 15:  %class...
    {const_cast<char32_t*>(U"_"),  Kw_codes,             19},         // 16:  %codes...
    {const_cast<char32_t*>(U"n"),  M_Kw_comments,        20},         // 17:  %comme...
    {const_cast<char32_t*>(U"m"),  M_Kw_class_members,   21},         // 18:  %class_...
    {const_cast<char32_t*>(U"t"),  M_Kw_codes_type,      22},         // 19:  %codes_...
    {const_cast<char32_t*>(U"t"),  M_Kw_comments,        23},         // 20:  %commen...
    {const_cast<char32_t*>(U"e"),  M_Kw_class_members,   24},         // 21:  %class_m...
    {const_cast<char32_t*>(U"y"),  M_Kw_codes_type,      25},         // 22:  %codes_t...
    {const_cast<char32_t*>(U"s"),  M_Kw_comments,        26},         // 23:  %comment...
    {const_cast<char32_t*>(U"m"),  M_Kw_class_members,   27},         // 24:  %class_me...
    {const_cast<char32_t*>(U"p"),  M_Kw_codes_type,      28},         // 25:  %codes_ty...
    {const_cast<char32_t*>(U""),   Kw_comments,          0},          // 26:  %comments
    {const_cast<char32_t*>(U"b"),  M_Kw_class_members,   29},         // 27:  %class_mem...
    {const_cast<char32_t*>(U"e"),  M_Kw_codes_type,      30},         // 28:  %codes_typ...
    {const_cast<char32_t*>(U"e"),  M_Kw_class_members,   31},         // 29:  %class_memb...
    {const_cast<char32_t*>(U""),   Kw_codes_type,        0},          // 30:  %codes_type
    {const_cast<char32_t*>(U"r"),  M_Kw_class_members,   32},         // 31:  %class_membe...
    {const_cast<char32_t*>(U"s"),  M_Kw_class_members,   33},         // 32:  %class_member...
    {const_cast<char32_t*>(U""),   Kw_class_members,     0},          // 33:  %class_members

    {const_cast<char32_t*>(U"e"),  M_Kw_delimiters,      35},         // 34:  %d...
    {const_cast<char32_t*>(U"l"),  M_Kw_delimiters,      36},         // 35:  %de...
    {const_cast<char32_t*>(U"i"),  M_Kw_delimiters,      37},         // 36:  %del...
    {const_cast<char32_t*>(U"m"),  M_Kw_delimiters,      38},         // 37:  %deli...
    {const_cast<char32_t*>(U"i"),  M_Kw_delimiters,      39},         // 38:  %delim...
    {const_cast<char32_t*>(U"t"),  M_Kw_delimiters,      40},         // 39:  %delimi...
    {const_cast<char32_t*>(U"e"),  M_Kw_delimiters,      41},         // 40:  %delimit...
    {const_cast<char32_t*>(U"r"),  M_Kw_delimiters,      42},         // 41:  %delimite...
    {const_cast<char32_t*>(U"s"),  M_Kw_delimiters,      43},         // 42:  %delimiter...
    {const_cast<char32_t*>(U""),   Kw_delimiters,        0},          // 43:  %delimiters

    {const_cast<char32_t*>(U"d"),  M_Kw_idents,          45},         // 44:  %i...
    {const_cast<char32_t*>(U"e"),  M_Kw_idents,          46},         // 45:  %id...
    {const_cast<char32_t*>(U"n"),  M_Kw_idents,          47},         // 46:  %ide...
    {const_cast<char32_t*>(U"t"),  M_Kw_idents,          48},         // 47:  %iden...
    {const_cast<char32_t*>(U"_s"), M_Kw_idents,          49},         // 48:  %ident...
    {const_cast<char32_t*>(U"n"),  M_Kw_ident_name,      51},         // 49:  %ident_...
    {const_cast<char32_t*>(U""),   Kw_idents,            0},          // 50:  %idents
    {const_cast<char32_t*>(U"a"),  M_Kw_ident_name,      52},         // 51:  %ident_n...
    {const_cast<char32_t*>(U"m"),  M_Kw_ident_name,      53},         // 52:  %ident_na...
    {const_cast<char32_t*>(U"e"),  M_Kw_ident_name,      54},         // 53:  %ident_nam...
    {const_cast<char32_t*>(U""),   Kw_ident_name,        0},          // 54:  %ident_name

    {const_cast<char32_t*>(U"e"),  M_Kw_keywords,        56},         // 55:  %k...
    {const_cast<char32_t*>(U"y"),  M_Kw_keywords,        57},         // 56:  %ke...
    {const_cast<char32_t*>(U"w"),  M_Kw_keywords,        58},         // 57:  %key...
    {const_cast<char32_t*>(U"o"),  M_Kw_keywords,        59},         // 58:  %keyw...
    {const_cast<char32_t*>(U"r"),  M_Kw_keywords,        60},         // 59:  %keywo...
    {const_cast<char32_t*>(U"d"),  M_Kw_keywords,        61},         // 60:  %keywor...
    {const_cast<char32_t*>(U"s"),  M_Kw_keywords,        62},         // 61:  %keyword...
    {const_cast<char32_t*>(U""),   Kw_keywords,          0},          // 62:  %keywords

    {const_cast<char32_t*>(U"u"),  M_Kw_multilined,      64},         // 63:  %m...
    {const_cast<char32_t*>(U"l"),  M_Kw_multilined,      65},         // 64:  %mu...
    {const_cast<char32_t*>(U"t"),  M_Kw_multilined,      66},         // 65:  %mul...
    {const_cast<char32_t*>(U"i"),  M_Kw_multilined,      67},         // 66:  %mult...
    {const_cast<char32_t*>(U"l"),  M_Kw_multilined,      68},         // 67:  %multi...
    {const_cast<char32_t*>(U"i"),  M_Kw_multilined,      69},         // 68:  %multil...
    {const_cast<char32_t*>(U"n"),  M_Kw_multilined,      70},         // 69:  %multili...
    {const_cast<char32_t*>(U"e"),  M_Kw_multilined,      71},         // 70:  %multilin...
    {const_cast<char32_t*>(U"d"),  M_Kw_multilined,      72},         // 71:  %multiline...
    {const_cast<char32_t*>(U""),   Kw_multilined,        0},          // 72:  %multilined

    {const_cast<char32_t*>(U"eu"), M_Kw_numbers,         74},         // 73:  %n...
    {const_cast<char32_t*>(U"sw"), M_Kw_nested,          76},         // 74:  %ne...
    {const_cast<char32_t*>(U"m"),  M_Kw_numbers,         78},         // 75:  %nu...
    {const_cast<char32_t*>(U"t"),  M_Kw_nested,          79},         // 76:  %nes...
    {const_cast<char32_t*>(U"l"),  M_Kw_newline_is_lexem,80},         // 77:  %new...
    {const_cast<char32_t*>(U"b"),  M_Kw_numbers,         81},         // 78:  %num...
    {const_cast<char32_t*>(U"e"),  M_Kw_nested,          82},         // 79:  %nest...
    {const_cast<char32_t*>(U"i"),  M_Kw_newline_is_lexem,83},         // 80:  %newl...
    {const_cast<char32_t*>(U"e"),  M_Kw_numbers,         84},         // 81:  %numb...
    {const_cast<char32_t*>(U"d"),  M_Kw_nested,          85},         // 82:  %neste...
    {const_cast<char32_t*>(U"n"),  M_Kw_newline_is_lexem,86},         // 83:  %newli...
    {const_cast<char32_t*>(U"r"),  M_Kw_numbers,         87},         // 84:  %numbe...
    {const_cast<char32_t*>(U""),   Kw_nested,            0},          // 85:  %nested
    {const_cast<char32_t*>(U"e"),  M_Kw_newline_is_lexem,88},         // 86:  %newlin...
    {const_cast<char32_t*>(U"s"),  M_Kw_numbers,         89},         // 87:  %number...
    {const_cast<char32_t*>(U"_"),  M_Kw_newline_is_lexem,90},         // 88:  %newline...
    {const_cast<char32_t*>(U""),   Kw_numbers,           0},          // 89:  %numbers
    {const_cast<char32_t*>(U"i"),  M_Kw_newline_is_lexem,91},         // 90:  %newline_...
    {const_cast<char32_t*>(U"s"),  M_Kw_newline_is_lexem,92},         // 91:  %newline_i...
    {const_cast<char32_t*>(U"_"),  M_Kw_newline_is_lexem,93},         // 92:  %newline_is...
    {const_cast<char32_t*>(U"l"),  M_Kw_newline_is_lexem,94},         // 93:  %newline_is_...
    {const_cast<char32_t*>(U"e"),  M_Kw_newline_is_lexem,95},         // 94:  %newline_is_l...
    {const_cast<char32_t*>(U"x"),  M_Kw_newline_is_lexem,96},         // 95:  %newline_is_le...
    {const_cast<char32_t*>(U"e"),  M_Kw_newline_is_lexem,97},         // 96:  %newline_is_lex...
    {const_cast<char32_t*>(U"m"),  M_Kw_newline_is_lexem,98},         // 97:  %newline_is_lexe...
    {const_cast<char32_t*>(U""),   Kw_newline_is_lexem,  0},          // 98:  %newline_is_lexem

    {const_cast<char32_t*>(U"cit"),M_Kw_scaner_name,     100},        // 99:  %s...
    {const_cast<char32_t*>(U"a"),  M_Kw_scaner_name,     103},        // 100: %sc...
    {const_cast<char32_t*>(U"n"),  M_Kw_single_lined,    104},        // 101: %si...
    {const_cast<char32_t*>(U"r"),  M_Kw_strings,         105},        // 102: %st...
    {const_cast<char32_t*>(U"n"),  M_Kw_scaner_name,     106},        // 103: %sca...
    {const_cast<char32_t*>(U"g"),  M_Kw_single_lined,    107},        // 104: %sin...
    {const_cast<char32_t*>(U"i"),  M_Kw_strings,         108},        // 105: %str...
    {const_cast<char32_t*>(U"e"),  M_Kw_scaner_name,     109},        // 106: %scan...
    {const_cast<char32_t*>(U"l"),  M_Kw_single_lined,    110},        // 107: %sing...
    {const_cast<char32_t*>(U"n"),  M_Kw_strings,         111},        // 108: %stri...
    {const_cast<char32_t*>(U"r"),  M_Kw_scaner_name,     112},        // 109: %scane...
    {const_cast<char32_t*>(U"e"),  M_Kw_single_lined,    113},        // 110: %singl...
    {const_cast<char32_t*>(U"g"),  M_Kw_strings,         114},        // 111: %strin...
    {const_cast<char32_t*>(U"_"),  M_Kw_scaner_name,     115},        // 112: %scaner...
    {const_cast<char32_t*>(U"_"),  M_Kw_single_lined,    116},        // 113: %single...
    {const_cast<char32_t*>(U"s"),  M_Kw_strings,         117},        // 114: %string...
    {const_cast<char32_t*>(U"n"),  M_Kw_scaner_name,     118},        // 115: %scaner_...
    {const_cast<char32_t*>(U"l"),  M_Kw_single_lined,    119},        // 116: %single_...
    {const_cast<char32_t*>(U""),   Kw_strings,           0},          // 117: %strings
    {const_cast<char32_t*>(U"a"),  M_Kw_scaner_name,     120},        // 118: %scaner_n...
    {const_cast<char32_t*>(U"i"),  M_Kw_single_lined,    121},        // 119: %single_l...
    {const_cast<char32_t*>(U"m"),  M_Kw_scaner_name,     122},        // 120: %scaner_na...
    {const_cast<char32_t*>(U"n"),  M_Kw_single_lined,    123},        // 121: %single_li...
    {const_cast<char32_t*>(U"e"),  M_Kw_scaner_name,     124},        // 122: %scaner_nam...
    {const_cast<char32_t*>(U"e"),  M_Kw_single_lined,    125},        // 123: %single_lin...
    {const_cast<char32_t*>(U""),   Kw_scaner_name,       0},          // 124: %scaner_name
    {const_cast<char32_t*>(U"d"),  M_Kw_single_lined,    126},        // 125: %single_line...
    {const_cast<char32_t*>(U""),   Kw_single_lined,      0},          // 126: %single_lined

    {const_cast<char32_t*>(U"o"),  M_Kw_token_fields,    128},        // 127: %t...
    {const_cast<char32_t*>(U"k"),  M_Kw_token_fields,    129},        // 128: %to...
    {const_cast<char32_t*>(U"e"),  M_Kw_token_fields,    130},        // 129: %tok...
    {const_cast<char32_t*>(U"n"),  M_Kw_token_fields,    131},        // 130: %toke...
    {const_cast<char32_t*>(U"_"),  M_Kw_token_fields,    132},        // 131: %token...
    {const_cast<char32_t*>(U"f"),  M_Kw_token_fields,    133},        // 132: %token_...
    {const_cast<char32_t*>(U"i"),  M_Kw_token_fields,    134},        // 133: %token_f...
    {const_cast<char32_t*>(U"e"),  M_Kw_token_fields,    135},        // 134: %token_fi...
    {const_cast<char32_t*>(U"l"),  M_Kw_token_fields,    136},        // 135: %token_fie...
    {const_cast<char32_t*>(U"d"),  M_Kw_token_fields,    137},        // 136: %token_fiel...
    {const_cast<char32_t*>(U"s"),  M_Kw_token_fields,    138},        // 137: %token_field...
    {const_cast<char32_t*>(U""),   Kw_token_fields,      0}           // 138: %token_fields
};

Main_scaner::Automaton_proc Main_scaner::procs[] = {
    &Main_scaner::start_proc,     &Main_scaner::unknown_proc,
    &Main_scaner::id_proc,        &Main_scaner::keyword_proc,
    &Main_scaner::delimiter_proc, &Main_scaner::string_proc
};

Main_scaner::Final_proc Main_scaner::finals[] = {
    &Main_scaner::none_final_proc,
    &Main_scaner::unknown_final_proc,
    &Main_scaner::id_final_proc,
    &Main_scaner::keyword_final_proc,
    &Main_scaner::delimiter_final_proc,
    &Main_scaner::string_final_proc
};

bool Main_scaner::start_proc(){
    bool t = true;
    state = -1;
    /* Для автомата, обрабатывающего какую-либо лексему, состояние с номером (-1)
     * является состоянием, в котором происходит инициализация этого автомата. */
    if(belongs(Spaces, char_categories)){
        loc->current_line += U'\n' == ch;
        return t;
    }
    lexem_begin_line = loc->current_line;
    if(belongs(Percent, char_categories)){
        automaton = A_keyword; token.code = Unknown;
    }else if(belongs(Id_begin, char_categories)){
        automaton = A_id; buffer = U""; buffer += ch;
        token.code = Id;
    }else if(belongs(Delimiters, char_categories)){
        automaton = A_delimiter; token.code = Unknown;
        (loc->pcurrent_char)--;
    }else if(belongs(Double_quote, char_categories)){
         automaton = A_string; token.code = String;
         buffer = U""; (loc->pcurrent_char)--;
    }else{
        automaton = A_unknown; token.code = Unknown;
    }
    return t;
}

static const char* keyword_strings[] = {
    "%action",             "%class_members",   "%codes",
    "%codes_type",         "%comments",        "%delimiters",
    "%ident_name",         "%idents",          "%keywords",
    "%multilined",         "%nested",          "%newline_is_lexem",
    "%numbers",            "%scaner_name",     "%single_lined",
    "%strings",            "%token_fields"
};

void Main_scaner::correct_keyword(){
    /* Данная функция корректирует код лексемы, скорее всего являющейся
     * ключевым словом, и выводит необходимую диагностику. */
    if(token.code >= M_Kw_action){
        int y = token.code - M_Kw_action;
        printf("В строке %zu ожидается %s.\n",
               loc->current_line, keyword_strings[y]);
        token.code = static_cast<Main_lexem_code>(y + Kw_action);
        en -> increment_number_of_errors();
    }
}
//
Main_lexem_info Main_scaner::current_lexem(){
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
            if(Id == token.code){
                /* Если текущая лексема является идентификатором, то этот
                 * идентификатор нужно записать в таблицу идентификаторов. */
                token.ident_index = ids -> insert(buffer);
            }else if(String == token.code){
                /* Если текущая лексема является строковым литералом, то её
                 * нужно записать в таблицу строковых литералов. */
                token.string_index = strs -> insert(buffer);
            }else if(A_keyword == automaton){
                /* Если закончили обрабатывать ключевое слово, то нужно
                 * скорректировать его код, и, возможно, вывести диагностику. */
                correct_keyword();
            }
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

bool Main_scaner::unknown_proc(){
    return belongs(Other, char_categories);
}

bool Main_scaner::id_proc(){
    bool t = belongs(Id_body, char_categories);
    /* Переменная t равна true, если идентификатор полностью
     * ещё не прочитан, и false в противном случае. */
    if(t){
        buffer += ch;
    }
    return t;
}

/* Данный массив состоит из пар вида (состояние, символ) и используется для инициализации
 * автомата обработки ключевых слов. Смысл элемента массива таков: если в состоянии (-1)
 * текущий символ совпадает со второй компонентой элемента, то работа начинается с
 * состояния, которое является первой компонентой элемента. Рассмотрим, например, элемент
 * {6, U'c'}. Если текущий символ совпадает с первой компонентой этого элемента, то
 * работа начинается с состояния, являющегося первой компонентой, т.е. с состояния 6.
 * Массив должен быть отсортирован по возрастанию второй компоненты. */
static const State_for_char init_table_for_keywords[] = {
    {0,  U'a'}, {6,  U'c'}, {34, U'd'}, {44,  U'i'}, {55, U'k'},
    {63, U'm'}, {73, U'n'}, {99, U's'}, {127, U't'}
};

bool Main_scaner::keyword_proc(){
    bool t = false;
    if(state != -1){
        Elem elem = a_keyword_jump_table[state];
        token.code = elem.code;
        int y = search_char(ch, elem.symbols);
        if(y != THERE_IS_NO_CHAR){
            state = elem.first_state + y; t = true;
        }
        return t;
    }
    if(belongs(After_percent, char_categories)){
        state = get_init_state(ch, init_table_for_keywords,
                               sizeof(init_table_for_keywords)/
                               sizeof(State_for_char));
        token.code = a_keyword_jump_table[state].code;
        t = true;
    }else{
        printf("В строке %zu ожидается один из следующих "
               "символов: a, c, d, i, k, m, n, s, t.\n",
               loc->current_line);
        en -> increment_number_of_errors();
    }
    return t;
}

bool Main_scaner::delimiter_proc(){
    bool t = false;
    switch(ch){
        case U',':
            token.code = Comma;
            break;
        case U':':
            token.code = Colon;
            break;
        case U'{':
            token.code = Opened_fig_brack;
            break;
        case U'}':
            token.code = Closed_fig_brack;
            break;
    }
    (loc->pcurrent_char)++;
    return t;
}

enum {Begin_string = -1, String_body, End_string};
/* Это имена состояний автомата обработки строковых литералов. */

bool Main_scaner::string_proc(){
/* Эта функция реализует конечный автомат для обработки строковых литералов.
 * Допускаемые Мяукой строковые литералы можно описать регулярным выражением
 *         b(a|bb)*b (1),
 * где под b понимается двойная кавычка ("), а под a --- любой символ, отличный
 * от двойной кавычки. Иными словами, строковые литералы заключаются в двойные
 * кавычки. Если же в строковом литерале нужно указать такую кавычку, то её
 * нужно удвоить. Кроме того, из приведённого регулярного выражения следует,
 * что строковый литерал может занимать несколько строк исходного текста.
 *
 * Далее, если по регулярному выражению построить сначала НКА
 * (недетерминированный конечный автомат), потом по построенному НКА
 * построить соответствующий ДКА (детерминированный конечный автомат), и
 * полученный ДКА минимизировать, то получим ДКА со следующей таблицей
 * переходов:
 *
 * --------------------------------------------
 * | Состояние    | a           | b           |
 * --------------------------------------------
 * | Begin_string |             | String_body |
 * --------------------------------------------
 * | String_body  | String_body | End_string  |
 * --------------------------------------------
 * | End_string   |             | String_body |
 * --------------------------------------------
 *
 * В этой таблице состояние Begin_string является начальным,
 * а состояние End_string --- конечным. */
    bool t = true;
    switch(state){
        case Begin_string:
            state = String_body;
            break;
        case String_body:
            if(ch != U'\"'){
                buffer += ch;
            }else{
                state = End_string;
            }
            break;
        case End_string:
            if(U'\"' == ch){
                buffer += ch; state = String_body;
            }else{
                t = false;
            }
            break;
    }
    return t;
}

void Main_scaner::none_final_proc(){
    /* Данная подпрограмма будет вызвана, если по прочтении входного текста
     * оказались в автомате A_start. Тогда ничего делать не нужно. */
}

void Main_scaner::unknown_final_proc(){
    /* Данная подпрограмма будет вызвана, если по прочтении входного текста
     * оказались в автомате A_unknown. Тогда ничего делать не нужно. */
}

void Main_scaner::id_final_proc(){
    /* Данная функция будет вызвана, если по прочтении входного потока
     * оказались в автомате обработки идентификаторов, автомате A_id.
     * Тогда идентификатор нужно записать в таблицу идентификаторов. */
    token.ident_index = ids -> insert(buffer);
}

void Main_scaner::keyword_final_proc(){
    token.code = a_keyword_jump_table[state].code;
    correct_keyword();
}

void Main_scaner::delimiter_final_proc(){
}

void Main_scaner::string_final_proc(){
    /* Данная функция будет вызвана, если по прочтении входного потока
     * оказались в автомате обработки строковых литералов. Если при этом
     * находимся не в конечном состоянии этого автомата, то нужно вывести
     * диагностику. */
    token.string_index = strs -> insert(buffer);
    if(state != End_string){
        printf("Неожиданное окончание строкового литерала в строке %zu.\n",
               loc->current_line);
        en -> increment_number_of_errors();
    }
}