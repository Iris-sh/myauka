/*
    Файл:    detalize_commands.cpp
    Создан:  29 марта 2016г. в 14:34 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#include "../include/detalize_commands.h"
// #include "../include/char_conv.h" // для отладочной печати
#include <string>

static const std::u32string latin_upper_letters   = U"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const std::u32string latin_lower_letters   = U"abcdefghijklmnopqrstuvwxyz";
static const std::u32string russian_upper_letters = U"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
static const std::u32string russian_lower_letters = U"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
static const std::u32string binary_digits         = U"01";
static const std::u32string octal_digits          = U"01234567";
static const std::u32string decimal_digits        = U"0123456789";
static const std::u32string hexadecimal_digits    = U"0123456789ABCDEFabcdef";
static const std::u32string upper_letters         =
    latin_upper_letters + russian_upper_letters;
static const std::u32string lower_letters =
    latin_lower_letters + russian_lower_letters;

std::u32string tail(const std::u32string& s){
    return s.substr(1);
}

char32_t head(const std::u32string& s){
    return s[0];
}

using Head_and_tail = std::pair<char32_t, std::u32string>;

Head_and_tail head_and_tail(const std::u32string& s){
    return {head(s), tail(s)};
}

static const Head_and_tail pairs_for_char_classes[] = {
    head_and_tail(latin_upper_letters),   head_and_tail(upper_letters),
    head_and_tail(russian_upper_letters), head_and_tail(binary_digits),
    head_and_tail(decimal_digits),        head_and_tail(latin_lower_letters),
    head_and_tail(lower_letters),         head_and_tail(octal_digits),
    head_and_tail(russian_lower_letters), head_and_tail(hexadecimal_digits)
};

void detalize_commandsR(Command_buffer& out, const Command_buffer& inp, size_t curr_idx){
    Command com = inp[curr_idx];
    size_t  fst, snd;
    Head_and_tail ht;
    auto args = com.args;
    size_t act;
    switch(com.name){
        case Cmd_or: case Cmd_concat:
            detalize_commandsR(out, inp, args.first);
            fst = out.size() - 1;
            detalize_commandsR(out, inp, args.second);
            snd = out.size() - 1;
            com.args.first  = fst;
            com.args.second = snd;
            out.push_back(com);
            break;
        case Cmd_Kleene: case Cmd_positive: case Cmd_optional:
            detalize_commandsR(out, inp, args.first);
            fst = out.size() - 1;
            com.args.first  = fst;
            out.push_back(com);
            break;
        case Cmd_char_def: case Cmd_unknown:
            out.push_back(com);
            break;
        case Cmd_char_class_def:
            switch(com.cls){
                case C_ndq: case C_nsq:
                    out.push_back(com);
                    break;
                default:
                    ht       = pairs_for_char_classes[com.cls];
                    com.name = Cmd_char_def;
                    com.c    = ht.first;
                    act      = com.action_name;
//                     {
//                         auto s = char32_to_utf8(com.c);
//                         printf("\'%s\'\n", s.c_str());
//                     }
                    out.push_back(com);
//                     {
//                         auto s = char32_to_utf8(out.back().c);
//                         printf("\'%s\'\n", s.c_str());
//                     }
                    fst      = out.size() - 1;
                    //snd = fst + 1;
                    for(const auto cur_char : ht.second){
                        Command op;
                        com.c = cur_char;
//                         {
//                             auto s = char32_to_utf8(com.c);
//                             printf("\'%s\'\n", s.c_str());
//                         }
                        out.push_back(com);
//                         {
//                             auto s = char32_to_utf8(out.back().c);
//                             printf("\'%s\'\n", s.c_str());
//                         }

                        op.action_name  = act;
                        op.name         = Cmd_or;
                        op.args.first   = fst;
                        op.args.second  = out.size() - 1;

                        out.push_back(op);

                        fst = out.size() - 1;
                    }
            }
            break;
    }
}

void detalize_commands(Command_buffer& buf){
    if(buf.empty()){
        return;
    }
    Command_buffer buf_;
    detalize_commandsR(buf_, buf, buf.size() - 1);
    buf = buf_;
}
