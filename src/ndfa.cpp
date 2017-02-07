/*
    Файл:    ndfa.cpp
    Создан:  13 декабря 2015г. в 09:05 (по Москве)
    Автор:   Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/ndfa.h"
#include "../include/operations_with_sets.h"

static const Generalized_char eps_gc = {.kind = Epsilon};

using namespace operations_with_sets;

typedef void (*NDFA_builder)(NDFA&, const Unwrapped_commands&, size_t, size_t);

static void or_builder(NDFA& a,            const Unwrapped_commands& commands,
                       size_t command_nom, size_t first_state_nom);

static void concat_builder(NDFA& a,            const Unwrapped_commands& commands,
                           size_t command_nom, size_t first_state_nom);

static void kleene_clos_builder(NDFA& a,            const Unwrapped_commands& commands,
                                size_t command_nom, size_t first_state_nom);

static void positive_clos_builder(NDFA& a,            const Unwrapped_commands& commands,
                                  size_t command_nom, size_t first_state_nom);

static void optional_builder(NDFA& a,            const Unwrapped_commands& commands,
                             size_t command_nom, size_t first_state_nom);

static void char_def_builder(NDFA& a,            const Unwrapped_commands& commands,
                             size_t command_nom, size_t first_state_nom);

static void multior_builder(NDFA& a,            const Unwrapped_commands& commands,
                            size_t command_nom, size_t first_state_nom);

static void char_class_builder(NDFA& a,            const Unwrapped_commands& commands,
                               size_t command_nom, size_t first_state_nom);

static void unknown_builder(NDFA& a,            const Unwrapped_commands& commands,
                            size_t command_nom, size_t first_state_nom);

static void multiconcat_builder(NDFA& a,            const Unwrapped_commands& commands,
                                size_t command_nom, size_t first_state_nom);

static void generate_NDFA_for_command(NDFA&  a,       const  Unwrapped_commands& commands,
                                      size_t cmd_nom, size_t first_state_nom);

static const NDFA_builder ndfa_builders[] = {
    or_builder,             concat_builder,      kleene_clos_builder,
    positive_clos_builder,  optional_builder,    char_def_builder,
    multior_builder,        char_class_builder,         char_class_builder,
    unknown_builder,        multiconcat_builder
};

/*!  Данная функция строит  недетерминированный конечный автомат a для
     команды, индекс которой в списке команд commands указан параметром
     command_nom. Наименьший из номеров состояний строящегося автомата
     указан параметром first_state_nom. */
static void generate_NDFA_for_command(NDFA&  a,       const  Unwrapped_commands& commands,
                                      size_t cmd_nom, size_t first_state_nom){
    auto& com = commands[cmd_nom];
    (ndfa_builders[static_cast<uint32_t>(com.kind)])(a, commands, cmd_nom, first_state_nom);
}

void build_NDFA(NDFA& a, const Unwrapped_commands& commands){
    if(!commands.empty()){
        size_t last_command_index = commands.size() - 1;
        generate_NDFA_for_command(a, commands, last_command_index, 0);
    }
}

static void add_state_jumps(NDFA_state_jumps& sj,     Generalized_char c,
                            const States_with_action& added_states){
    auto it = sj.find(c);
    if(it != sj.end()){
        auto sa = it -> second;
        sj[c] = std::make_pair(added_states.first + sa.first, sa.second);
    }else{
        sj[c] = added_states;
    }
}

static void or_builder(NDFA& a,            const Unwrapped_commands& commands,
                       size_t command_nom, size_t first_state_nom)
{
    auto& com = commands[command_nom];
    NDFA a1, a2;
    NDFA_state_jumps state_jumps;
    generate_NDFA_for_command(a1, commands, com.first_arg, first_state_nom + 1);
    generate_NDFA_for_command(a2, commands, com.second_arg, a1.final_state + 1);
    state_jumps[eps_gc] =
        std::make_pair(single_elem(a1.begin_state) + single_elem(a2.begin_state), 0);
    /* Далее склейка a1 и a2 с дописыванием состояний в a. */
    size_t final_st = a2.final_state + 1;
    auto last_state = std::make_pair(single_elem(final_st),0);
    add_state_jumps(a1.jumps.back(), eps_gc, last_state);
    add_state_jumps(a2.jumps.back(), eps_gc, last_state);
    a.jumps.push_back(state_jumps);
    a.jumps.insert(a.jumps.end(), a1.jumps.begin(), a1.jumps.end());
    a.jumps.insert(a.jumps.end(), a2.jumps.begin(), a2.jumps.end());
    a.jumps.push_back(NDFA_state_jumps());
    a.begin_state = first_state_nom;
    a.final_state = final_st;
}

static void concat_builder(NDFA& a,            const Unwrapped_commands& commands,
                           size_t command_nom, size_t first_state_nom)
{
    auto& com = commands[command_nom];
    NDFA a1, a2;
    NDFA_state_jumps state_jumps1, state_jumps2;
    generate_NDFA_for_command(a1, commands, com.first_arg, first_state_nom);
    generate_NDFA_for_command(a2, commands, com.second_arg, a1.final_state);
    /* Далее склейка a1 и a2 с дописыванием состояний в a. */
    /* Cклеим начальное состояние автомата a2 с конечным состоянием автомата a1. */
    state_jumps1 = a1.jumps.back();
    state_jumps2 = a2.jumps[0];
    for(auto sj : state_jumps2){
        add_state_jumps(state_jumps1, sj.first, sj.second);
    }
    /* Затем добавляем состояния автомата a1 (кроме конечного). */
    a.jumps.insert(a.jumps.end(), a1.jumps.begin(), a1.jumps.end() - 1);
    /* Добавляем склеенное состояние. */
    a.jumps.push_back(state_jumps1);
    /* Наконец, добавляем прочие состояния автомата a2. */
    a.jumps.insert(a.jumps.end(), a2.jumps.begin() + 1, a2.jumps.end());
    a.begin_state = first_state_nom;
    a.final_state = a2.final_state;
}

static void kleene_clos_builder(NDFA& a,            const Unwrapped_commands& commands,
                                size_t command_nom, size_t first_state_nom)
{
    auto& com = commands[command_nom];
    NDFA              a_without_clos;
    NDFA_state_jumps  state_jumps;

    generate_NDFA_for_command(a_without_clos, commands, com.first_arg,
                              first_state_nom + 1);

    size_t final_st = a_without_clos.final_state + 1;
    States_with_action temp_jumps =
        std::make_pair(single_elem(a_without_clos.begin_state) + single_elem(final_st),0);

    add_state_jumps(a_without_clos.jumps.back(), eps_gc, temp_jumps);

    state_jumps[eps_gc] = temp_jumps;

    a.jumps.push_back(state_jumps);
    a.jumps.insert(a.jumps.end(), a_without_clos.jumps.begin(),
                   a_without_clos.jumps.end());
    a.jumps.push_back(NDFA_state_jumps());
    a.begin_state = first_state_nom;
    a.final_state = final_st;
}

static void positive_clos_builder(NDFA& a,            const Unwrapped_commands& commands,
                                  size_t command_nom, size_t first_state_nom)
{
    auto& com = commands[command_nom];
    NDFA a1, a2;
    NDFA_state_jumps state_jumps;
    generate_NDFA_for_command(a1, commands, com.first_arg, first_state_nom);
    a2 = a1;
    size_t number_of_states_in_a1 = a1.jumps.size();

    /* Цикл ниже увеличивает номера состояний для второй копии автомата для выражения,
     * стоящего под знаком положительного замыкания. */
    /* Внешний цикл --- по состояниям автомата a2. */
    for(auto& state_jmps : a2.jumps){
        /* Следующий цикл --- по переходам для текущего состояния. */
        for(auto& jump : state_jmps){
            Generalized_char   c  = jump.first;
            States_with_action sa = jump.second;
            Set_of_states      new_set_of_states;
            for(const auto s : sa.first){
                new_set_of_states.insert(s + number_of_states_in_a1);
            }
            state_jmps[c] = std::make_pair(new_set_of_states, sa.second);
        }
    }
    a2.begin_state += number_of_states_in_a1;
    a2.final_state += number_of_states_in_a1;

    size_t final_st = a2.final_state + 1;

    States_with_action temp_jumps =
        std::make_pair(single_elem(a2.begin_state) + single_elem(final_st),0);

    add_state_jumps(a1.jumps.back(), eps_gc, temp_jumps);
    add_state_jumps(a2.jumps.back(), eps_gc, temp_jumps);

    a.jumps.insert(a.jumps.end(), a1.jumps.begin(), a1.jumps.end());
    a.jumps.insert(a.jumps.end(), a2.jumps.begin(), a2.jumps.end());
    a.jumps.push_back(NDFA_state_jumps());
    a.begin_state = first_state_nom;
    a.final_state = final_st;
}

static void optional_builder(NDFA& a,            const Unwrapped_commands& commands,
                             size_t command_nom, size_t first_state_nom)
{
    auto& com = commands[command_nom];
    NDFA a_without_opt;
    generate_NDFA_for_command(a_without_opt, commands, com.first_arg, first_state_nom + 1);

    NDFA_state_jumps begin_st, begin_st_for_eps, end_st_for_eps;

    size_t final_st  = a_without_opt.final_state + 3;
    begin_st[eps_gc] =
        std::make_pair(single_elem(first_state_nom + 1) +
                       single_elem(a_without_opt.final_state + 1), 0);
    begin_st_for_eps[eps_gc] = std::make_pair(single_elem(a_without_opt.final_state + 2), 0);
    end_st_for_eps[eps_gc]   = std::make_pair(single_elem(final_st), 0);
    add_state_jumps(a_without_opt.jumps.back(), eps_gc,
                    std::make_pair(single_elem(final_st),0));

    a.jumps.push_back(begin_st);
    a.jumps.insert(a.jumps.end(), a_without_opt.jumps.begin(), a_without_opt.jumps.end());
    a.jumps.push_back(begin_st_for_eps);
    a.jumps.push_back(end_st_for_eps);
    a.jumps.push_back(NDFA_state_jumps());

    a.begin_state = first_state_nom;
    a.final_state = final_st;
}

static void char_def_builder(NDFA& a,            const Unwrapped_commands& commands,
                             size_t command_nom, size_t first_state_nom)
{
    auto& com = commands[command_nom];
    NDFA_state_jumps begin_st;
    Generalized_char gc;
    gc.kind = Char; gc.c = com.c;
    begin_st[gc] =
        std::make_pair(single_elem(first_state_nom + 1), com.action_name);
    a.jumps.push_back(begin_st);
    a.jumps.push_back(NDFA_state_jumps());

    a.begin_state = first_state_nom;
    a.final_state = first_state_nom + 1;
}

/* Следующая функция строит множество целых чисел, принадлежащих
 * отрезку [min_state, max_state]. */
Set_of_states range(size_t min_state, size_t max_state){
    Set_of_states result;
    for(size_t e = min_state; e <= max_state; e++){
        result.insert(e);
    }
    return result;
}

static void multior_builder(NDFA& a,            const Unwrapped_commands& commands,
                            size_t command_nom, size_t first_state_nom)
{
    auto&  com           = commands[command_nom];
    auto   s             = com.s;
    size_t num_of_chars  = s.size();
    size_t dnum_of_chars = 2 * num_of_chars;
    auto   jumps         = std::vector<NDFA_state_jumps>(dnum_of_chars + 2);
    size_t y             = first_state_nom + num_of_chars;
    size_t last_state    = first_state_nom + 2 * num_of_chars + 1;
    auto   last_sa       = std::make_pair(single_elem(last_state), 0);
    auto   sts           = range(first_state_nom + 1, first_state_nom + num_of_chars);
    auto&  j0            = jumps[0];
    j0[eps_gc]           = std::make_pair(sts, 0);

    size_t i = 1;

    NDFA_state_jumps last_jump;
    last_jump[eps_gc]    = last_sa;

    for(const auto c : s){
        Generalized_char gc;
        gc.kind                 = Char;
        gc.c                    = c;
        NDFA_state_jumps t;
        t[gc]                   = std::make_pair(single_elem(y + i), com.action_name);
        jumps[i]                = t;
        jumps[num_of_chars + i] = last_jump;
        i++;
    }

    a.jumps = jumps;
    a.begin_state = first_state_nom;
    a.final_state = last_state;
}

static void char_class_builder(NDFA& a,            const Unwrapped_commands& commands,
                        size_t command_nom, size_t first_state_nom)
{
    auto& com = commands[command_nom];
    NDFA_state_jumps begin_st;
    Generalized_char gc;
    gc.kind = Char_class;
    gc.cls  = (Unwrapped_kind::Ndq == com.kind) ? G_ndq : G_nsq;
    begin_st[gc] =
        std::make_pair(single_elem(first_state_nom + 1), com.action_name);
    a.jumps.push_back(begin_st);
    a.jumps.push_back(NDFA_state_jumps());

    a.begin_state = first_state_nom;
    a.final_state = first_state_nom + 1;
}

static void unknown_builder(NDFA& a,            const Unwrapped_commands& commands,
                            size_t command_nom, size_t first_state_nom)
{}

static void multiconcat_builder(NDFA& a,            const Unwrapped_commands& commands,
                                size_t command_nom, size_t first_state_nom)
{
    NDFA   accumulator;
    auto&  com               = commands[command_nom];
    size_t fst               = com.first_arg;
    size_t snd               = com.second_arg;
    size_t last_state;
    size_t current_fst_state = first_state_nom;
    generate_NDFA_for_command(accumulator, commands, fst, current_fst_state);
    current_fst_state        = accumulator.final_state;
    last_state               = accumulator.final_state;
    for(size_t i = fst + 1; i <= snd; i++){
        NDFA current;
        generate_NDFA_for_command(current, commands, i, current_fst_state);
        /* Породили конечный автомат для очередной команды, из тех, что связаны
         * командой Multiconcat. Теперь нужно приклеить этот автомат к аккумулятору. */
        /* Начинаем приклеивание. */
        NDFA_state_jumps state_jumps1;
        NDFA_state_jumps state_jumps2;
        state_jumps1      = accumulator.jumps.back();
        state_jumps2      = current.jumps[0];
        for(auto sj : state_jumps2){
            add_state_jumps(state_jumps1, sj.first, sj.second);
        }
        current.jumps[0] = state_jumps1;
        accumulator.jumps.pop_back();
        accumulator.jumps.insert(accumulator.jumps.end(), current.jumps.begin(),
                                 current.jumps.end());
        current_fst_state = current.final_state;
        last_state        = current.final_state;
    }
    accumulator.final_state = last_state;
    a                       = accumulator;
}

void print_NDFA(const NDFA& a){
    printf("begin_state: %zu\n", a.begin_state);
    printf("final_state: %zu\n", a.final_state);
    printf("transitions:\n");
    size_t state_idx = 0;
    for(const auto& j : a.jumps){
        for(const auto& sj : j){
            printf("delta(");
            printf("%zu, ", state_idx);
            print_generalized_char(sj.first);
            printf(") = ");
            auto sa = sj.second;
            print_set(sa.first, print_size_t);
            printf(" with action having index %zu\n",sa.second);
        }
        state_idx++;
    }
}