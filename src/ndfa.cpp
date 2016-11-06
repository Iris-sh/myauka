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

const Generalized_char eps_gc = {.kind = Epsilon};

using namespace operations_with_sets;

typedef void (*NDFA_builder)(NDFA&, const Command_buffer&, size_t, size_t);

void or_builder            (NDFA& a,            const Command_buffer& commands,
                            size_t command_nom, size_t first_state_nom);

void concat_builder        (NDFA& a,            const Command_buffer& commands,
                            size_t command_nom, size_t first_state_nom);

void kleene_clos_builder   (NDFA& a,            const Command_buffer& commands,
                            size_t command_nom, size_t first_state_nom);

void positive_clos_builder (NDFA& a,            const Command_buffer& commands,
                            size_t command_nom, size_t first_state_nom);

void optional_builder      (NDFA& a,            const Command_buffer& commands,
                            size_t command_nom, size_t first_state_nom);

void char_def_builder      (NDFA& a,            const Command_buffer& commands,
                            size_t command_nom, size_t first_state_nom);

void char_class_def_builder(NDFA& a,            const Command_buffer& commands,
                            size_t command_nom, size_t first_state_nom);

void unknown_builder       (NDFA& a,            const Command_buffer& commands,
                            size_t command_nom, size_t first_state_nom);

NDFA_builder ndfa_builders[] = {
    or_builder,             concat_builder,   kleene_clos_builder,
    positive_clos_builder,  optional_builder, char_def_builder,
    char_class_def_builder, unknown_builder
};

void add_state_jumps(NDFA_state_jumps& sj,     Generalized_char c,
                     const States_with_action& added_states){
    auto it = sj.find(c);
    if(it != sj.end()){
        auto sa = it -> second;
        sj[c] = std::make_pair(added_states.first + sa.first, sa.second);
    }else{
        sj[c] = added_states;
    }
}

void generate_NDFA_for_command(NDFA& a,            const Command_buffer& commands,
                               size_t command_nom, size_t first_state_nom);

void or_builder(NDFA& a,            const Command_buffer& commands,
                size_t command_nom, size_t first_state_nom){
    Command com = commands[command_nom];
    NDFA a1, a2;
    NDFA_state_jumps state_jumps;
    generate_NDFA_for_command(a1, commands, com.args.first, first_state_nom + 1);
    generate_NDFA_for_command(a2, commands, com.args.second, a1.final_state + 1);
    state_jumps[eps_gc] =
        std::make_pair(single_elem(a1.begin_state) + single_elem(a2.begin_state), 0);
    /* Далее склейка a1 и a2 с дописыванием состояний в a. */
    size_t final_st = a2.final_state + 1;
    auto last_state = std::make_pair(single_elem(final_st),0);
    add_state_jumps(a1.jumps.back(), eps_gc, last_state);  // a1.jumps[a1.jumps.size() - 1]
    add_state_jumps(a2.jumps.back(), eps_gc, last_state); // a2.jumps[a2.jumps.size() - 1]
    a.jumps.push_back(state_jumps);
    a.jumps.insert(a.jumps.end(), a1.jumps.begin(), a1.jumps.end());
    a.jumps.insert(a.jumps.end(), a2.jumps.begin(), a2.jumps.end());
    a.jumps.push_back(NDFA_state_jumps());
    a.begin_state = first_state_nom;
    a.final_state = final_st;
}

void concat_builder(NDFA& a,            const Command_buffer& commands,
                    size_t command_nom, size_t first_state_nom){
    Command com = commands[command_nom];
    NDFA a1, a2;
    NDFA_state_jumps state_jumps1, state_jumps2;
    generate_NDFA_for_command(a1, commands, com.args.first, first_state_nom);
    generate_NDFA_for_command(a2, commands, com.args.second, a1.final_state);
    /* Далее склейка a1 и a2 с дописыванием состояний в a. */
    /* Cклеим начальное состояние автомата a2 с конечным состоянием автомата a1. */
    state_jumps1 = a1.jumps.back(); // a1.jumps[a1.jumps.size() - 1];
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

void kleene_clos_builder(NDFA& a,            const Command_buffer& commands,
                         size_t command_nom, size_t first_state_nom){
    Command com = commands[command_nom];
    NDFA a_without_clos;
    NDFA_state_jumps state_jumps;

    generate_NDFA_for_command(a_without_clos, commands, com.args.first,
                              first_state_nom + 1);

    size_t final_st = a_without_clos.final_state + 1;
    States_with_action temp_jumps =
        std::make_pair(single_elem(a_without_clos.begin_state) + single_elem(final_st),0);

    add_state_jumps(a_without_clos.jumps.back(), eps_gc, temp_jumps); //a_without_clos.jumps[a_without_clos.jumps.size() - 1]

    state_jumps[eps_gc] = temp_jumps;

    a.jumps.push_back(state_jumps);
    a.jumps.insert(a.jumps.end(), a_without_clos.jumps.begin(),
                   a_without_clos.jumps.end());
    a.jumps.push_back(NDFA_state_jumps());
    a.begin_state = first_state_nom;
    a.final_state = final_st;
}

void positive_clos_builder(NDFA& a,            const Command_buffer& commands,
                           size_t command_nom, size_t first_state_nom){
    Command com = commands[command_nom];
    NDFA a1, a2;
    NDFA_state_jumps state_jumps;
    generate_NDFA_for_command(a1, commands, com.args.first, first_state_nom);
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

    add_state_jumps(a1.jumps[a1.jumps.size() - 1], eps_gc, temp_jumps);
    add_state_jumps(a2.jumps[a2.jumps.size() - 1], eps_gc, temp_jumps);

    a.jumps.insert(a.jumps.end(), a1.jumps.begin(), a1.jumps.end());
    a.jumps.insert(a.jumps.end(), a2.jumps.begin(), a2.jumps.end());
    a.jumps.push_back(NDFA_state_jumps());
    a.begin_state = first_state_nom;
    a.final_state = final_st;
}

void optional_builder(NDFA& a,            const Command_buffer& commands,
                      size_t command_nom, size_t first_state_nom){
    Command com = commands[command_nom];
    NDFA a_without_opt;
    generate_NDFA_for_command(a_without_opt, commands, com.args.first, first_state_nom + 1);

    NDFA_state_jumps begin_st, begin_st_for_eps, end_st_for_eps;

    size_t final_st = a_without_opt.final_state + 3;
    begin_st[eps_gc] =
        std::make_pair(single_elem(first_state_nom + 1) +
                       single_elem(a_without_opt.final_state + 1), 0);
    begin_st_for_eps[eps_gc] = std::make_pair(single_elem(a_without_opt.final_state + 2), 0);
    end_st_for_eps[eps_gc]   = std::make_pair(single_elem(final_st), 0);
    add_state_jumps(a_without_opt.jumps[a_without_opt.jumps.size() - 1], eps_gc,
                    std::make_pair(single_elem(final_st),0));

    a.jumps.push_back(begin_st);
    a.jumps.insert(a.jumps.end(), a_without_opt.jumps.begin(), a_without_opt.jumps.end());
    a.jumps.push_back(begin_st_for_eps);
    a.jumps.push_back(end_st_for_eps);
    a.jumps.push_back(NDFA_state_jumps());

    a.begin_state = first_state_nom;
    a.final_state = final_st;
}

void char_def_builder(NDFA& a,            const Command_buffer& commands,
                      size_t command_nom, size_t first_state_nom){
    Command com = commands[command_nom];
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

void char_class_def_builder(NDFA& a,            const Command_buffer& commands,
                            size_t command_nom, size_t first_state_nom){
    Command com = commands[command_nom];
    NDFA_state_jumps begin_st;
    Generalized_char gc;
    gc.kind = Char_class; gc.cls = static_cast<Char_group>(com.cls);
    begin_st[gc] =
        std::make_pair(single_elem(first_state_nom + 1), com.action_name);
    a.jumps.push_back(begin_st);
    a.jumps.push_back(NDFA_state_jumps());

    a.begin_state = first_state_nom;
    a.final_state = first_state_nom + 1;
}

void unknown_builder(NDFA& a,            const Command_buffer& commands,
                     size_t command_nom, size_t first_state_nom){
}

/*!  Данная функция строит  недетерминированный конечный автомат a для
     команды, индекс которой в списке команд commands указан параметром
     command_nom. Наименьший из номеров состояний строящегося автомата
     указан параметром first_state_nom. */
void generate_NDFA_for_command(NDFA& a,        const Command_buffer& commands,
                               size_t cmd_nom, size_t first_state_nom){
    Command com = commands[cmd_nom];
    (ndfa_builders[com.name])(a, commands, cmd_nom, first_state_nom);
}

void build_NDFA(NDFA& a, const Command_buffer& commands){
    size_t last_command_index = commands.size() - 1;
    generate_NDFA_for_command(a, commands, last_command_index, 0);
}

// void print_NDFA(const NDFA& a){
    // printf("begin_state: %zu\n", a.begin_state);
    // printf("final_state: %zu\n", a.final_state);
    // printf("transitions:\n");
    // size_t state_idx = 0;
    // for(const auto& j : a.jumps){
        // for(const auto& sj : j){
            // printf("delta(");
            // printf("%zu, ", state_idx);
            // print_generalized_char(sj.first);
            // printf(") = ");
            // auto sa = sj.second;
            // print_set(sa.first, print_size_t);
            // printf(" with action having index %zu\n",sa.second);
        // }
        // state_idx++;
    // }
// }