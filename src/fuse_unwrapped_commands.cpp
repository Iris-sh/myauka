/*
    File:    fuse_unwrapped_commands.cpp
    Created: 25 January 2017г. в 17:09 (по Москве)
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/operations_with_sets.h"
#include "../include/belongs.h"
#include "../include/fuse_unwrapped_commands.h"

using operations_with_sets::operator+;
using operations_with_sets::single_elem;

static const uint64_t set_of_leaf_commands =
    (1ULL << static_cast<uint64_t>(Unwrapped_kind::Char_def)) |
    (1ULL << static_cast<uint64_t>(Unwrapped_kind::Ndq))      |
    (1ULL << static_cast<uint64_t>(Unwrapped_kind::Nsq))      |
    (1ULL << static_cast<uint64_t>(Unwrapped_kind::Unkwnown));

class Wrapped_set{
public:
    Wrapped_set()                   = default;
    Wrapped_set(const Wrapped_set&) = default;
    Wrapped_set(const Unwrapped_command& uc);
    Wrapped_set(const std::set<char32_t>& s) : s_(s) {};
    ~Wrapped_set()                  = default;

    friend Unwrapped_command operator + (const Wrapped_set& lhs, const Wrapped_set& rhs);
private:
    std::set<char32_t> s_;
};

Wrapped_set::Wrapped_set(const Unwrapped_command& uc){
    switch(uc.kind){
        case Unwrapped_kind::Char_def:
            s_ = single_elem(uc.c);
            break;
        case Unwrapped_kind::Multior:
            s_ = uc.s;
            break;
        default:
            s_ = std::set<char32_t>();
    }
}

Unwrapped_command operator + (const Wrapped_set& lhs, const Wrapped_set& rhs){
    Unwrapped_command result;
    result.s    = lhs.s_ + rhs.s_;
    result.kind = Unwrapped_kind::Multior;
    return result;
}

void fuse_unwrapped_commandsR(Unwrapped_commands& out, const Unwrapped_commands& inp,
                              size_t curr_idx)
{
    Unwrapped_command com  = inp[curr_idx];
    auto              kind = com.kind;
    size_t            fst_idx, snd_idx;
    switch(kind){
        case Unwrapped_kind::Or:
            {
                fuse_unwrapped_commandsR(out, inp, com.first_arg);
                fst_idx                             = out.size() - 1;
                fuse_unwrapped_commandsR(out, inp, com.second_arg);
                snd_idx                             = out.size() - 1;
                Unwrapped_command fst_arg           = out[fst_idx];
                Unwrapped_command snd_arg           = out[snd_idx];
                Unwrapped_kind    fst_kind          = fst_arg.kind;
                Unwrapped_kind    snd_kind          = snd_arg.kind;
                bool              fst_arg_is_char32 = fst_kind == Unwrapped_kind::Char_def;
                bool              snd_arg_is_char32 = snd_kind == Unwrapped_kind::Char_def;
                bool              fst_arg_is_mor    = Unwrapped_kind::Multior == fst_kind;
                bool              snd_arg_is_mor    = Unwrapped_kind::Multior == snd_kind;
                Unwrapped_command new_command       = com;
                new_command.first_arg               = 0;
                new_command.second_arg              = 0;
                size_t            fst_action        = fst_arg.action_name;
                size_t            snd_action        = snd_arg.action_name;
                new_command.action_name             = fst_action ? fst_action : snd_action;
                if((fst_arg_is_char32 || fst_arg_is_mor) && (snd_arg_is_char32 || snd_arg_is_mor)){
                    Wrapped_set s1 = fst_arg;
                    Wrapped_set s2 = snd_arg;
                    new_command = s1 + s2;
                    out.pop_back();
                    out.pop_back();
                }else{
                    new_command.first_arg               = fst_idx;
                    new_command.second_arg              = snd_idx;
                    new_command.action_name             = 0;
                }
                out.push_back(new_command);
            }
            break;
        case Unwrapped_kind::Concat:
            fuse_unwrapped_commandsR(out, inp, com.first_arg);
            fst_idx        = out.size() - 1;
            fuse_unwrapped_commandsR(out, inp, com.second_arg);
            snd_idx        = out.size() - 1;
            com.first_arg  = fst_idx;
            com.second_arg = snd_idx;
            out.push_back(com);
            break;
        case Unwrapped_kind::Kleene:   case Unwrapped_kind::Positive:
        case Unwrapped_kind::Optional:
            fuse_unwrapped_commandsR(out, inp, com.first_arg);
            fst_idx       = out.size() - 1;
            com.first_arg = fst_idx;
            out.push_back(com);
            break;
        case Unwrapped_kind::Char_def: case Unwrapped_kind::Multior:
        case Unwrapped_kind::Ndq:      case Unwrapped_kind::Nsq:
        case Unwrapped_kind::Unkwnown:
            out.push_back(com);
            break;
        case Unwrapped_kind::Multiconcat:
            fst_idx = out.size();
            for(size_t idx = com.first_arg; idx <= com.second_arg; idx++){
                out.push_back(inp[idx]);
            }
            snd_idx = out.size() - 1;
            com.first_arg  = fst_idx;
            com.second_arg = snd_idx;
            out.push_back(com);
            break;
    }
}

Unwrapped_commands fuse_unwrapped_commands(const Unwrapped_commands& buf)
{
    Unwrapped_commands result;
    if(!buf.empty()){
        fuse_unwrapped_commandsR(result, buf, buf.size() - 1);
    }
    return result;
}