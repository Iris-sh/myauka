/*
    File:    aux_files_generate.cpp
    Created: 14 December 2015 at 15:25 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/aux_files_generate.h"
#include <cstdio>

static const char* abstract_scaner_h =
    R"~(#ifndef ABSTRACT_SCANER_H
#define ABSTRACT_SCANER_H

#include <string>
#include <memory>
#include "../include/error_count.h"
#include "../include/char_trie.h"
#include "../include/location.h"
#include "../include/errors_and_tries.h"

template<typename Lexem_type>
class Abstract_scaner{
public:
    Abstract_scaner<Lexem_type>() = default;
    Abstract_scaner(Location_ptr location, const Errors_and_tries& et);
    Abstract_scaner(const Abstract_scaner<Lexem_type>& orig) = default;
    virtual ~Abstract_scaner() = default;
    /* Функция back() возвращает текущую лексему во входной поток.*/
    void back();
    /* Функция current_lexem() возвращает сведения о текущей
     * лексеме (код лексемы и значение лексемы). */
    virtual Lexem_type current_lexem() = 0;
    /* Функция lexem_begin_line_number() возвращает номер строки
     * обрабатываемого текста, с которой начинается лексема,
     * сведения о которой возвращены функцией current_lexem(). */
    size_t lexem_begin_line_number();
protected:
    int                          state; /* текущее состояние текущего автомата */

    Location_ptr                 loc;
    char32_t*                    lexem_begin; /* указатель на начало лексемы */
    char32_t                     ch;          /* текущий символ */

    /* множество категорий символов, которым принадлежит
     * текущий символ */
    uint64_t                     char_categories;

    /* промежуточное значение сведений о лексеме */
    Lexem_type                   token;

    /* номер строки, с которой начинается текущая лексема */
    size_t                       lexem_begin_line;

    /* указатель на класс, подсчитывающий количество ошибок: */
    std::shared_ptr<Error_count> en;
    /* указатель на префиксное дерево для идентификаторов: */
    std::shared_ptr<Char_trie>   ids;
    /* указатель на префиксное дерево для строк: */
    std::shared_ptr<Char_trie>   strs;

    /*буфер для записи обрабатываемого идентификатора или строки: */
    std::u32string               buffer;
};

template<typename Lexem_type>
Abstract_scaner<Lexem_type>::Abstract_scaner(Location_ptr location,
                                             const Errors_and_tries& et){
    ids = et.ids_trie; strs = et.strs_trie; en = et.ec;
    loc = location;
    lexem_begin = location->pcurrent_char;
    lexem_begin_line = 1;
}

template<typename Lexem_type>
void Abstract_scaner<Lexem_type>::back(){
    loc->pcurrent_char = lexem_begin;
    loc->current_line =  lexem_begin_line;
}

template<typename Lexem_type>
size_t Abstract_scaner<Lexem_type>::lexem_begin_line_number(){
    return lexem_begin_line;
}
#endif)~";

static const char* belongs_h =
    R"~(#ifndef BELONGS_H
#define BELONGS_H
/* Данная функция проверяет, принадлежит ли элемент element множеству s. При этом
 * считаем, что множество s состоит из не более чем 64 элементов, так что в качестве
 * внутреннего представления множества используется тип uint64_t. Если бит с номером
 * i внутреннего представления равен 1, то элемент i принадлежит множеству,
 * иначе --- не принадлежит. */
inline uint64_t belongs(uint64_t element, uint64_t s){
    return s & (1ULL << element);
}
#endif)~";

static const char* char_conv_h =
    R"~(/**
\file

\brief Заголовочный файл с прототипами функций, преобразующих строки из кодировки
UTF-8 в кодировку UTF-32 и наоборот.
*/

#ifndef CHAR_CONV_H
#define CHAR_CONV_H

#include <string>

/**
\function utf8_to_u32string
 Данная функция по строке в кодировке UTF-8 строит строку в кодировке UTF-32.

\param  utf8str – строка в кодировке UTF-8 с завершающим нулевым символом

\return значение типа std::u32string, представляющее собой ту же строку,
 но в кодировке UTF-32
*/
std::u32string utf8_to_u32string(const char* utf8str);

/**
\function u32string_to_utf8
 Данная функция по строке в кодировке UTF-32 строит строку в кодировке UTF-8.

\param [in] u32str – строка в кодировке UTF-32

\return значение типа std::string, представляющее собой ту же строку,
 но в кодировке UTF-8
*/
std::string u32string_to_utf8(const std::u32string& u32str);

/**
\function char32_to_utf8
По символу в кодировке UTF-32 строит строку, состоящую из байтов, представляющих
тот же символ, но в кодировке UTF-8.

\param [in] с - символ в кодировке UTF-32

\return Значение типа std::string, состоящее из байтов, представляющих
тот же символ, но в кодировке UTF-8.
*/
std::string char32_to_utf8(const char32_t c);
#endif
)~";

static const char* char_trie_h =
    R"~(#ifndef CHAR_TRIE_H
#define CHAR_TRIE_H

#include "../include/trie.h"

struct Char_trie_as_map {
    std::map<size_t,char32_t*> *m;
    ~Char_trie_as_map();
};

class Char_trie : public Trie<char32_t>{
public:
    virtual ~Char_trie() { };
    /* Конструктор по умолчанию. */
    Char_trie(){};
    /* Копирующий конструктор. */
    Char_trie(const Char_trie& orig) = default;
    /* Функция, по индексу idx строящая строку в стиле C,
     * соответствующую индексу idx. */
    char32_t* get_cstring(size_t idx);
    /* Функция, по индексу idx строящая строку типа u32string,
       соответствующую индексу idx. */
    std::u32string get_string(size_t idx);
    /* Функция, возвращающая префиксное дерево в виде
     * отображения индексов строк в строки в стиле C. */
    Char_trie_as_map as_map();
    /* Функция вывода строки, которой соответствует индекс idx, на экран. */
    void print(size_t idx);
    /* Следующая функция по индексу строки возвращает длину этой строки. */
    size_t get_length(size_t idx);
};

#endif
)~";

static const char* error_count_h =
    R"~(#ifndef ERROR_COUNT_H
#define ERROR_COUNT_H
/* Класс для подсчёта количества ошибок. */
class Error_count {
public:
    Error_count() : number_of_errors(0) {};
    void increment_number_of_errors();
    void print();
    int get_number_of_errors();
private:
    int number_of_errors;
};
#endif
)~";

static const char* errors_and_tries_h =
    R"~(#ifndef ERRORS_AND_TRIES_H
#define ERRORS_AND_TRIES_H

#include "../include/error_count.h"
#include "../include/char_trie.h"
#include <memory>
struct Errors_and_tries{
    std::shared_ptr<Error_count> ec;
    std::shared_ptr<Char_trie>   ids_trie;
    std::shared_ptr<Char_trie>   strs_trie;

    Errors_and_tries()  = default;
    ~Errors_and_tries() = default;
};
#endif)~";

static const char* file_contents_h =
    R"~(#ifndef FILE_CONTENTS_H
#define FILE_CONTENTS_H
#include <string>
#include <utility>

/** Коды возврата из функции get_contents. */
enum class Get_contents_return_code{
    Normal,           ///< Этот код означает, что всё прошло успешно.
    Impossible_open,  ///< Этот код означает, что не удалось открыть файл.
    Read_error        ///< Этот код означает, что во время чтения файла произошла ошибка.
};

using Contents  = std::pair<Get_contents_return_code, std::string>;

/**
   Возвращает всё содержимое файла с заданным именем.
   \param [in] name --- имя читаемого файла
   \returns Пару из кода возврата (первая компонента) и значения, имеющего
тип std::string (вторая компонента). При возникновении ошибки вторая компонента
возвращаемого значения представляет собой пустую строку.
*/
Contents get_contents(const char* name);
#endif)~";

static const char* fsize_h =
    R"~(#ifndef FSIZE_H
#define FSIZE_H
#include <cstdio>
/* Данная функция выдаёт размер файла в байтах, если
 * fptr != NULL, и (-1) в противном случае. */
long fsize(FILE* fptr);
#endif)~";

static const char* get_init_state_h =
    R"~(#ifndef GET_INIT_STATE_H
#define GET_INIT_STATE_H

struct State_for_char{
    unsigned st;
    char32_t c;
};

/* Функция get_init_state инициализирует конечный автомат. Делает она это так: ищет
 * символ sym в таблице sts, состоящей из пар (состояние, символ) и имеющей размер
 * n, двоичным поиском по второму компоненту пары. После нахождения выдаётся
 * первая компонента пары. В качестве алгоритма двоичного поиска используется
 * алгоритм B из раздела 6.2.1 монографии "Кнут Д.Э. Искусство программирования.
 * Т.3. Сортировка и поиск. 2-е изд.: Пер. с англ. --- М.: Вильямс, 2008.". При
 * этом в нашем случае не может быть, чтобы нужный элемент в таблице sts
 * отсутствовал. */
int get_init_state(char32_t sym, const State_for_char sts[], int n);
#endif)~";

static const char* location_h =
    R"~(#ifndef LOCATION_H
#define LOCATION_H

#include <memory>
/* Следующая структура описывает текущее положение в обрабатываемом тексте.
 * В конструктор сканера нужно передавать умный указатель на
 * разделяемые сведения о текущем местоположении. */

//#include <cstddef>
struct Location {
    char32_t* pcurrent_char; /* указатель на текущий символ */
    size_t    current_line; /* номер текущей строки обрабатываемого текста */

    Location() : pcurrent_char(nullptr), current_line(1) {};
    Location(char32_t* txt) : pcurrent_char(txt), current_line(1) {};
};

using Location_ptr = std::shared_ptr<Location>;
#endif)~";

static const char* operation_with_sets_h =
    R"~(#ifndef OPERATIONS_WITH_SETS_H
#define OPERATIONS_WITH_SETS_H

#include <set>
#include <cstdio>
/**
    В данном файле определяются теоретико--множественные операции
    со стандартными контейнерами std::set.
*/
namespace operations_with_sets{
    //! Функция single_elem возвращает множество, состоящее из одного элемента.
    template<typename T>
    std::set<T> single_elem(const T& x){
        std::set<T> s;
        s.insert(x);
        return s;
    }

    /** Функция печати элементов множества. Принимает в качестве
        аргумента функцию печати элемента множества. */
    template<typename T>
    void print_set(const std::set<T>& a, void (*print_elem)(const T&)){
        if(a.empty()){
            printf("{}");
            return;
        }
        auto first       = a.begin();
        auto before_last = --a.end();
        putchar('{');
        for(auto i = first; i != before_last; ++i){
            print_elem(*i);
            putchar(',');
        }
        print_elem(*before_last);
        putchar('}');
    }

    /** Проверка принадлежности элемента x множеству a. Если элемент x
        множеству a принадлежит, то возвращается true, иначе ---
        возвращается false. */
    template<typename T>
    bool is_elem(const T& x, const std::set<T>& a){
        return a.find(x) != a.end();
    }

    /** Объединение множеств a и b, то есть множество, содержащее и
        элементы множества a, и элементы множества b. */
    template<typename T>
    std::set<T> operator + (const std::set<T>& a, const std::set<T>& b){
        std::set<T> s = a;
        s.insert(b.begin(), b.end());
        return s;
    }

    /** Теоретико--множественная разность множеств a и b (обозначается в
        теории множеств как a \ b), то есть множество, состоящее лишь из тех
        элементов множества a, которые не принадлежат множеству b. */
    template<typename T>
    std::set<T> operator - (const std::set<T>& a, const std::set<T>& b){
        std::set<T> s = a;
        for(const auto x : b){
            s.erase(x);
        }
        return s;
    }

    /** Пересечение множеств a и b, то есть множество, состоящее в точности из
        тех элементов, которые принадлежат и a, и b. */
    template<typename T>
    std::set<T> operator * (const std::set<T>& a, const std::set<T>& b){
        std::set<T> s;
        for(const auto& x : a){
            if(is_elem(x, b)){
                s.insert(x);
            }
        }
        return s;
    }

    /** Симметрическая разность множеств a и b, то есть объединение этих
        множеств с выкинутыми общими элементами. */
    template<typename T>
    std::set<T> operator ^ (const std::set<T>& a, const std::set<T>& b){
        return (a - b) + (b - a);
    }

    /** Проверяет, является ли множество a подмножеством множества b,
        возможно, совпадающим с b. */
    template<typename T>
    bool is_subseteq(const std::set<T>& a, const std::set<T>& b){
        std::set<T> s = (a * b) ^ a;
        return s.empty();
    }
};
#endif)~";

static const char* search_char_h =
    R"~(#define THERE_IS_NO_CHAR (-1)
/**
 * \function search_char
 * Данная функция ищет заданный символ типа char32_t в строке,
 * состоящей из символов такого типа и завершающейся нулевым
 * символом.
 *
 * \param [in] c --- искомый символ
 * \param [in] array --- строка в которой ищется символ
 * \return смещение (в символах) от начала строки, если
 * искомый символ в строке есть, и (-1) в противном случае
 */
int search_char(char32_t c, const char32_t* array);
#endif)~";

static const char* trie_h =
    R"~(#ifndef TRIE_H
#define TRIE_H

#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <string>
#include <set>

template<typename T>
class Trie {
public:
    /* Конструктор по умолчанию. */
    Trie<T>();
    /* Деструктор. */
    ~Trie() = default;
    /* Копирующий конструктор. */
    Trie(const Trie<T>& orig) = default;
    /* Функция вставки в префиксное дерево. */
    size_t insert(const std::basic_string<T>& s);
    /* Функция, вычисляющая максимальную степень вершин префиксного
     * дерева (корень дерева не учитывается). */
    size_t maximal_degree();
protected:
    /* тип узла префиксного дерева: */
    struct node{
      size_t parent, first_child, next;
      /* Все узлы-потомки текущего узла организованы в виде односвязного списка, первым
       * элементом которого является элемент с индексом first_child. В поле parent
       * содержится индекс родительского узла, а в поле next -- следующего потомка
       * родительского узла. Если у текущего узла потомков нет, то в поле first_child
       * содержится нуль. Аналогично, последний элемент в списке потомков в поле next
       * содержит нуль. Здесь под индексом понимается индекс в поле node_buffer,
       * представляющем собой вектор (в смысле библиотеки STL) из узлов префиксного
       * дерева. */
      size_t path_len; /* в этом поле содержится длина пути
                        * от текущего узла до корня дерева */
      size_t degree; /* В этом поле содержится степень узла,
                      * то есть количество выходящих из узла рёбер. */
      T c; /* в этом поле содержится символ вставленной строки,
            * являющийся меткой текущего узла. */
      node(){
        next = parent = path_len = first_child = 0;
        degree = 0; c = T();
      }
    };
    std::vector<node>   node_buffer;
    std::vector<size_t> nodes_indeces;
    /* Функция, добавляющая к списку потомков узла parent_idx узел, помеченный
     * значением x типа T. Функция возвращает индекс вставленного узла. */
    size_t add_child(size_t parent_idx, T x);
    /* Эта функция выполняет (возможно, необходимые) действия
     * по окончании вставки последнего символа. */
    virtual void post_action(const std::basic_string<T>& s, size_t n){ };
};

template<typename T>
Trie<T>::Trie(){
    node_buffer = std::vector<node>(1);
    nodes_indeces = std::vector<size_t>();
}

template<typename T>
size_t Trie<T>::maximal_degree(){
    size_t deg = 0;
    size_t len = node_buffer.size();
    for(int i = 1; i < len; i++){
        deg = std::max(deg,node_buffer[i].degree);
    }
    return deg;
}

template<typename T>
size_t Trie<T>::add_child(size_t parent_idx, T x){
    size_t current, previous;
    node   temp;
    current = previous = node_buffer[parent_idx].first_child;
    /* В переменной temp содержится узел, который, возможно, придётся вставить. */
    temp.c = x; temp.degree = 0;
    temp.next = 0; temp.parent = parent_idx;
    temp.path_len = node_buffer[parent_idx].path_len + 1;
    if(!current){
        /* Здесь можем оказаться, лишь если у узла с индексом parent_idx потомков
         * вообще нет. Значит добавляемый узел будет первым в списке потомков. При
         * этом степень узла parent_idx увеличится на единицу, и станет равна 1. */
        node_buffer.push_back(temp);
        size_t child_idx = node_buffer.size() - 1;
        node_buffer[parent_idx].first_child = child_idx;
        node_buffer[parent_idx].degree = 1;
        return child_idx;
    }
    while(current){
        // Если же потомки есть, то нужно пройти по списку потомков.
        node current_node = node_buffer[current];
        if(current_node.c == x){
          /* Если потомок, помеченный нужным символом (символом x),
           * есть, то нужно вернуть индекс этого потомка. */
          return current;
        }else{
          previous = current; current = current_node.next;
        }
    }
    /* Если же такого потомка нет, то нужно этого потомка добавить
     * в конец списка потомков.*/
    node_buffer.push_back(temp);
    size_t next_child = node_buffer.size() - 1;
    node_buffer[previous].next = next_child;
    node_buffer[parent_idx].degree++;
    return next_child;
}

template<typename T>
size_t Trie<T>::insert(const std::basic_string<T>& s){
    ssize_t len = s.length();
    size_t current_root = 0;
    for (ssize_t i = 0; i < len; i++) {
        current_root = add_child(current_root,s[i]);
    }
    nodes_indeces.push_back(current_root);
    post_action(s,current_root);
    return current_root;
}
#endif)~";

static const char* char_conv_cpp =
    R"~(#include "../include/char_conv.h"

std::string char32_to_utf8(const char32_t c){
    std::string s;
    char c1, c2, c3, c4;
    char32_t temp = c;
    switch(c){
        case 0x0000'0000 ... 0x0000'007f:
            s += static_cast<char>(c);
            break;

        case 0x0000'0080 ... 0x0000'07ff:
            c1 = 0b110'0'0000 | (temp >> 6);
            c2 = 0b10'00'0000 | (temp & 0b111'111);
            s += c1; s += c2;
            break;

        case 0x0000'0800 ... 0x0000'ffff:
            c3 = 0b10'00'0000 | (temp & 0b111'111);
            temp >>= 6;
            c2 = 0b10'00'0000 | (temp & 0b111'111);
            temp >>= 6;
            c1 = 0b1110'0000 | c;
            s += c1; s += c2; s += c3;
            break;

        case 0x0001'0000 ... 0x001f'ffff:
            c4 = 0b10'00'0000 | (temp & 0b111'111);
            temp >>= 6;
            c3 = 0b10'00'0000 | (temp & 0b111'111);
            temp >>= 6;
            c2 = 0b10'00'0000 | (temp & 0b111'111);
            temp >>= 6;
            c1 = 0b11110'000 | c;
            s += c1; s += c2; s += c3; s += c4;
            break;

        default:
            ;
    }
    return s;
}

std::string u32string_to_utf8(const std::u32string& u32str){
    std::string s;
    for(const char32_t c : u32str){
        s += char32_to_utf8(c);
    }
    return s;
}

std::u32string utf8_to_u32string(const char* utf8str){
    std::u32string s;
    enum class State{
        Start_state,                 Three_byte_char_second_byte,
        Four_byte_char_second_byte,  Four_byte_char_third_byte,
        Last_byte_of_char
    };
    State state = State::Start_state;
    char32_t current_char = 0;
    while(char c = *utf8str++){
        switch(state){
            case State::Start_state:
                if(c >= 0){
                    s += c;
                }else if((c & 0b1110'0000) == 0b1100'0000){
                    current_char = c & 0b0001'1111;
                    state = State::Last_byte_of_char;
                }else if((c & 0b1111'0000) == 0b1110'0000){
                    current_char = c & 0b0000'1111;
                    state = State::Three_byte_char_second_byte;
                }else if((c & 0b1111'1000) == 0b1111'0000){
                    current_char = c & 0b0000'0111;
                    state = State::Four_byte_char_second_byte;
                }
                break;

            case State::Last_byte_of_char:
                current_char = (current_char << 6) | (c & 0b0011'1111);
                s += current_char;
                state = State::Start_state;
                break;

            case State::Three_byte_char_second_byte:
                current_char = (current_char << 6) | (c & 0b0011'1111);
                state = State::Last_byte_of_char;
                break;

            case State::Four_byte_char_second_byte:
                current_char = (current_char << 6) | (c & 0b0011'1111);
                state = State::Four_byte_char_third_byte;
                break;

            case State::Four_byte_char_third_byte:
                current_char = (current_char << 6) | (c & 0b0011'1111);
                state = State::Last_byte_of_char;
                break;
        }
    }
    return s;
})~";

static const char* char_trie_cpp =
    R"~(#include "../include/char_conv.h"
#include "../include/char_trie.h"
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <string>
#include <set>

Char_trie_as_map::~Char_trie_as_map(){
    for(auto x : *m){
        delete [] x.second;
    }
}

Char_trie_as_map Char_trie::as_map(){
    Char_trie_as_map t;
    t.m = new std::map<size_t,char32_t*>();
    for(auto x : nodes_indeces){
        t.m -> insert({x,get_cstring(x)});
    }
    return t;
}

char32_t* Char_trie::get_cstring(size_t idx){
    size_t id_len = node_buffer[idx].path_len;
    char32_t* p = new char32_t[id_len + 1];
    p[id_len] = 0;
    size_t current = idx;
    size_t i       = id_len-1;
    /* Поскольку idx -- индекс элемента в node_buffer, содержащего последний символ
     * вставленной строки, а каждый элемент вектора node_buffer содержит поле parent,
     * указывающее на элемент с предыдущим символом строки, то для получения
     * вставленной строки, которой соответствует индекс idx, в виде массива символов,
     * нужно пройтись от элемента с индексом idx к корню. При этом символы вставленной
     * строки будут читаться от её конца к началу. */
    for( ; current; current = node_buffer[current].parent){
        p[i--] = node_buffer[current].c;
    }
    return p;
}

std::u32string Char_trie::get_string(size_t idx){
    char32_t* p = get_cstring(idx);
    std::u32string s = std::u32string(p);
    delete [] p;
    return s;
}

void Char_trie::print(size_t idx){
    std::u32string s32 = get_string(idx);
    std::string    s8  = u32string_to_utf8(s32);
    printf("%s",s8.c_str());
}

size_t Char_trie::get_length(size_t idx){
    return node_buffer[idx].path_len;
})~";

static const char* error_count_cpp =
    R"~(#include "../include/error_count.h"
#include <cstdio>

void Error_count::increment_number_of_errors(){
    number_of_errors++;
}

int Error_count::get_number_of_errors(){
    return number_of_errors;
}

void Error_count::print(){
    printf("\nВсего ошибок: %d\n", number_of_errors);
})~";

static const char* file_contents_cpp =
    R"~(#include "../include/file_contents.h"
#include "../include/fsize.h"
#include <cstdio>
#include <memory>

Contents get_contents(const char* name){
    Contents result = std::make_pair(Get_contents_return_code::Normal, "");
    FILE* fptr = fopen(name, "rb");
    if(!fptr){
        result.first = Get_contents_return_code::Impossible_open;
        return result;
    }
    long file_size = fsize(fptr);
    if(!file_size){
        return result;
    }
    auto   test_text = std::make_unique<char[]>(file_size + 1);
    char*  q         = test_text.get();
    size_t fr        = fread(q, 1, file_size, fptr);
    if(fr < (unsigned long)file_size){
        fclose(fptr);
        result.first = Get_contents_return_code::Read_error;
        return result;
    }
    test_text[file_size] = 0;
    fclose(fptr);
    result.second = std::string(test_text.get());
    return result;
})~";

static const char* fsize_cpp =
    R"~(#include "../include/fsize.h"
#include <cstdio>
long fsize(FILE* fptr){
    long ret_val = -1;
    if(fptr){
        long current_pos = ftell(fptr);
        fseek(fptr, 0L, SEEK_END);
        ret_val = ftell(fptr);
        fseek(fptr, current_pos, SEEK_SET);
    }
    return ret_val;
})~";

static const char* get_init_state_cpp =
    R"~(#include "../include/get_init_state.h"
int get_init_state(char32_t sym, const State_for_char sts[], int n){
    int lower, upper, middle;
    lower = 0; upper = n - 1;
    while(lower <= upper){
        middle = (lower + upper) >> 1;
        char32_t c_ = sts[middle].c;
        if(sym == c_){
            return sts[middle].st;
        }else if(sym > c_){
            lower = middle + 1;
        }else{
            upper = middle - 1;
        }
    }
    return -1;
})~";

static const char* search_char_cpp =
    R"~(#include "../include/search_char.h"
int search_char(char32_t c, const char32_t* array){
    char32_t ch;
    int      curr_pos = 0;
    for(char32_t* p = const_cast<char32_t*>(array); (ch = *p++); ){
        if(ch == c){
            return curr_pos;
        }
        curr_pos++;
    }
    return THERE_IS_NO_CHAR;
})~";

struct Name_and_contents{
    char* file_name;
    char* content;
};

static const Name_and_contents generated_files[] = {
    {const_cast<char*>("abstract_scaner.h"),     const_cast<char*>(abstract_scaner_h)    },
    {const_cast<char*>("belongs.h"),             const_cast<char*>(belongs_h)            },
    {const_cast<char*>("char_conv.h"),           const_cast<char*>(char_conv_h)          },
    {const_cast<char*>("char_trie.h"),           const_cast<char*>(char_trie_h)          },
    {const_cast<char*>("error_count.h"),         const_cast<char*>(error_count_h)        },
    {const_cast<char*>("errors_and_tries.h"),    const_cast<char*>(errors_and_tries_h)   },
    {const_cast<char*>("file_contents.h"),       const_cast<char*>(file_contents_h)      },
    {const_cast<char*>("fsize.h"),               const_cast<char*>(fsize_h)              },
    {const_cast<char*>("get_init_state.h"),      const_cast<char*>(get_init_state_h)     },
    {const_cast<char*>("location.h"),            const_cast<char*>(location_h)           },
    {const_cast<char*>("operation_with_sets.h"), const_cast<char*>(operation_with_sets_h)},
    {const_cast<char*>("search_char.h"),         const_cast<char*>(search_char_h)        },
    {const_cast<char*>("trie.h"),                const_cast<char*>(trie_h)               },
    {const_cast<char*>("char_conv.cpp"),         const_cast<char*>(char_conv_cpp)        },
    {const_cast<char*>("char_trie.cpp"),         const_cast<char*>(char_trie_cpp)        },
    {const_cast<char*>("error_count.cpp"),       const_cast<char*>(error_count_cpp)      },
    {const_cast<char*>("file_contents.cpp"),     const_cast<char*>(file_contents_cpp)    },
    {const_cast<char*>("fsize.cpp"),             const_cast<char*>(fsize_cpp)            },
    {const_cast<char*>("get_init_state.cpp"),    const_cast<char*>(get_init_state_cpp)   },
    {const_cast<char*>("search_char.cpp"),       const_cast<char*>(search_char_cpp)      }
};

void aux_files_generate(){
    for(auto n : generated_files){
        FILE* fptr = fopen(n.file_name, "w");
        if(fptr){
            fputs(n.content, fptr);
            fputs("\n",fptr);
            fclose(fptr);
        }else{
            printf("Не удалось создать файл %s.\n",n.file_name);
        }
    }
}