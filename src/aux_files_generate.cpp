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
#include "../include/trie.h"
#include "../include/location.h"
#include "../include/errors_and_tries.h"

template<typename Lexem_type>
class Scaner{
public:
    Scaner<Lexem_type>() = default;
    Scaner(Location_ptr location, const Errors_and_tries& et);
    Scaner(const Scaner<Lexem_type>& orig) = default;
    virtual ~Scaner() = default;
    /*  Function back() return the current lexem into the input stream. */
    void back();
    /* Function current_lexem() returns information about current lexem,
     * i.e. returns a lexem code and a lexem value. */
    virtual Lexem_type current_lexem() = 0;
    /* Function lexem_begin_line_number() returns the line number
     * at which the lexem starts. */
    size_t lexem_begin_line_number();
protected:
    int                          state; /* the current state of the current automaton */

    Location_ptr                 loc;
    char32_t*                    lexem_begin; /* pointer to the lexem begin */
    char32_t                     ch;          /* current character */

    /* set of categories for the current character */
    uint64_t                     char_categories;

    /* intermediate value of the lexem information */
    Lexem_type                   token;

    /* the line number at which the current lexem starts */
    size_t                       lexem_begin_line;

    /* a pointer to a class that counts the number of errors: */
    std::shared_ptr<Error_count> en;
    /* a pointer to the prefix tree for identifiers: */
    std::shared_ptr<Char_trie>   ids;
    /* a pointer to the prefix tree for string literals: */
    std::shared_ptr<Char_trie>   strs;

    /* buffer for writing the processed identifier or string: */
    std::u32string               buffer;
};

template<typename Lexem_type>
Scaner<Lexem_type>::Scaner(Location_ptr location, const Errors_and_tries& et){
    ids = et.ids_trie; strs = et.strs_trie; en = et.ec;
    loc = location;
    lexem_begin = location->pcurrent_char;
    lexem_begin_line = 1;
}

template<typename Lexem_type>
void Scaner<Lexem_type>::back(){
    loc->pcurrent_char = lexem_begin;
    loc->current_line =  lexem_begin_line;
}

template<typename Lexem_type>
size_t Scaner<Lexem_type>::lexem_begin_line_number(){
    return lexem_begin_line;
}
#endif)~";

static const char* belongs_h =
    R"~(#ifndef BELONGS_H
#define BELONGS_H
#include <cstdint>
/* If the element e belongs to the set s, then this function returns non-zero value;
 * else this function returns zero. The internal representation of the set is the value of
 * the type uint64_t: if bit with #i is equal to 1, then the number i belongs to the set;
 * else the number i doesn't belong to set. */
inline uint64_t belongs(uint64_t e, uint64_t s){
    return s & (1ULL << e);
}
#endif)~";

static const char* char_conv_h =
    R"~(#ifndef CHAR_CONV_H
#define CHAR_CONV_H

#include <string>

/**
\param [in] utf8str --- UTF-8 string with terminating null character

\return value of the type std::u32string, representing the same string,
but in the encoding UTF-32
*/
std::u32string utf8_to_u32string(const char* utf8str);

/**
\param [in] u32str --- string in the encoding UTF-32

\return value of the type std::string, representing the same string,
but in the encoding UTF-8
*/
std::string u32string_to_utf8(const std::u32string& u32str);

/**
\param [in] с --- character in the encoding UTF-32

\return value of the type std::string, consisting of bytes, representing
the same character, but in the encoding UTF-8.
*/
std::string char32_to_utf8(const char32_t c);
#endif)~";

static const char* char_trie_h =
    R"~(#ifndef CHAR_TRIE_H
#define CHAR_TRIE_H

#include "../include/trie.h"

class Char_trie : public Trie<char32_t>{
public:
    virtual ~Char_trie() { };

    Char_trie(){};

    Char_trie(const Char_trie& orig) = default;

    /* Using the index idx, this function builds C-style string
     * corresponding to the index idx. */
    char32_t* get_cstring(size_t idx);

    /* Using index idx, this function builds a string of the type u32string
     * corresponding to the index idx. */
    std::u32string get_string(size_t idx);

    /* This function outputs the string corresponding to the index idx. */
    void print(size_t idx);

    /* The following function returns the length of the string
     * corresponding to the index idx. */
    size_t get_length(size_t idx);
};
#endif)~";

static const char* error_count_h =
    R"~(#ifndef ERROR_COUNT_H
#define ERROR_COUNT_H
/* A class for calculating the number of errors. */
class Error_count {
public:
    Error_count() : number_of_errors(0) {};
    void increment_number_of_errors();
    void print();
    int get_number_of_errors();
private:
    int number_of_errors;
};
#endif)~";

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

/** Return codes from the function get_contents. */
enum class Get_contents_return_code{
    Normal,           ///< This code means success.
    Impossible_open,  ///< This code means that file could not open.
    Read_error        ///< This code means that an error occurred while reading the file.
};

using Contents  = std::pair<Get_contents_return_code, std::string>;

/**
   Returns: the contents of the file with the specified name
   \param [in] name file name
   \returns The pair (return code, value), here value is of type std::string.
   If an error occured, then the second component of this pair is an empty string.
*/
Contents get_contents(const char* name);
#endif)~";

static const char* fsize_h =
    R"~(#ifndef FSIZE_H
#define FSIZE_H
#include <cstdio>
/**
 * \param [in] fptr  the file pointer
 * \return           a) (-1), if fptr == NULL
 *                   b) the size of the file in bytes, otherwise
 */
long fsize(FILE* fptr);
#endif)~";

static const char* get_init_state_h =
    R"~(#ifndef GET_INIT_STATE_H
#define GET_INIT_STATE_H

struct State_for_char{
    unsigned st;
    char32_t c;
};

/* The function get_init_state initializes a finite automaton. Namely, this function
 * looking for the character sym in the table sts. Here sts is an array of
 * pairs (state, character), and number of elements in sts is n. The search is
 * performed in accordance with the binary search algorithm B from the section 6.2.1
 * of "Knuth D.E. The Art of Computer programming. V.3. Sorting and search. 2nd ed.
 * --- Addison Wesley, 1998.". */
int get_init_state(char32_t sym, const State_for_char sts[], int n);
#endif)~";

static const char* location_h =
    R"~(#ifndef LOCATION_H
#define LOCATION_H

#include <memory>
//#include <cstddef>
/**
 * \brief The following structure describes the current position in the processed text.
 *        In the scanner's constructor, you must pass a smart pointer to the shared
 *        information about the current location.
 */
struct Location {
    char32_t* pcurrent_char; ///< pointer to the current character
    size_t    current_line;  ///< number of current line

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
 * \brief In this file, set-theoretic operations with
 *        standard containers std :: set are defined.
*/
namespace operations_with_sets{
    /**
     *  \brief The function single_elem returns a set consisting of one element.
     *  \param [in] x element
     *  \return       a set consisting of one element, x.
     */
    template<typename T>
    std::set<T> single_elem(const T& x){
        std::set<T> s;
        s.insert(x);
        return s;
    }

    /**
     *  \brief Prints a set.
     *  \param [in] a          Printed set.
     *  \param [in] print_elem Print function of the set element.
     */
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


    /**
     *  \brief Prints a set.
     *  \param [in] a          Printed set.
     *  \param [in] print_elem Print function of the set element.
     */
    template<typename T>
    void print_set(const std::set<T>& a, void (*print_elem)(const T)){
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

    /**
     *  \brief Checking the membership of x for a set a.
     *  \param [in] a The set a.
     *  \param [in] x The element x.
     *  \return       true if the element x belongs to the set a, and false otherwise
     */
    template<typename T>
    bool is_elem(const T& x, const std::set<T>& a){
        return a.find(x) != a.end();
    }

    /**
     *  \brief The union of the sets a and b.
     *  \param [in] a The set a.
     *  \param [in] b The set b.
     *  \return       The union of the sets a and b. */
    template<typename T>
    std::set<T> operator + (const std::set<T>& a, const std::set<T>& b){
        std::set<T> s = a;
        s.insert(b.begin(), b.end());
        return s;
    }

    /**
     *  \brief   The set-theoretic difference of the sets a and b.
     *  \details The set-theoretic difference of the sets a and b (denoted in a set
     *           theory as a \ b), that is, a set consisting only of those elements
     *           of the set a that do not belong to the set b.
     *  \param [in] a The set a.
     *  \param [in] b The set b.
     *  \return       The set-theoretic difference of the sets a and b.
     */
    template<typename T>
    std::set<T> operator - (const std::set<T>& a, const std::set<T>& b){
        std::set<T> s = a;
        for(const auto x : b){
            s.erase(x);
        }
        return s;
    }

    /**
     *  \brief   The intersection of the sets a and b.
     *  \details The intersection of the sets a and b, that is, the set consisting
     *           precisely of those elements that belong to both a and b.
     *  \param [in] a The set a.
     *  \param [in] b The set b.
     *  \return       The intersection of the sets a and b.
     */
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

    /**
     *  \brief   The symmetric difference of the sets a and b.
     *  \details The symmetric difference of the sets a and b, that is, the union
     *           of these sets, but with the common elements thrown out.
     *  \param [in] a The set a.
     *  \param [in] b The set b.
     *  \return       The symmetric difference of the sets a and b.
     */
    template<typename T>
    std::set<T> operator ^ (const std::set<T>& a, const std::set<T>& b){
        return (a - b) + (b - a);
    }

    /**
     *  \brief Checks whether the set a is a subset of the set b,
     *         possibly coinciding with b.
     *  \param [in] a The set a.
     *  \param [in] b The set b.
     *  \return       true if the set a is a subset of the set b, and false otherwise
     */
    template<typename T>
    bool is_subseteq(const std::set<T>& a, const std::set<T>& b){
        std::set<T> s = (a * b) ^ a;
        return s.empty();
    }
};
#endif)~";

static const char* search_char_h =
    R"~(#ifndef SEARCH_CHAR_H
#define SEARCH_CHAR_H

#define THERE_IS_NO_CHAR (-1)
/**
 * \brief This function searches for a given character of type char32_t
 *        in a string consisting of characters of this type and ending
 *        with a null character.
 *
 * \param [in] c     The wanted character.
 * \param [in] array The string in which the symbol is searched for.
 * \return           Offset (in characters) from the beginning of the line, if the
 *                   desired character is in the string, and (-1) otherwise
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
    Trie<T>();

    ~Trie() = default;

    Trie(const Trie<T>& orig) = default;

    /**
     * \brief The function of inserting into the prefix tree.
     * \param [in] s Inserted string s.
     * \return       Index of the string s in the prefix tree.
     */
    size_t insert(const std::basic_string<T>& s);

    /**
     * \brief Calculation of the maximum degree of the vertices of the prefix tree
     *        (the root of the tree is not taken into account).
     * \return The maximum degree of the vertices of the prefix tree
     *         (the root of the tree is not taken into account)
     */
    size_t maximal_degree();
protected:
    /**
     * \struct node
     * \brief Node type of the prefix tree.
     * \details All child nodes of the current node are organized in the form of a
     *          simply-connected list, the first element of which is an element with
     *          the index first_child. The  field parent contains the index of the
     *          parent node, and in the next field the next descendant of the parent
     *          node. If the current node has no children, then the field  first_child
     *          contains zero. Similarly, the last element in the list of children in
     *          the  field next contains zero. Here, the subscript is the index in the
     *          field node_buffer, which is a vector (in the sense of the STL library)
     *          of the nodes of the prefix tree.
     */
    struct node{
      size_t parent, first_child, next;

      /// \brief The length of the path from the current node to the root of the tree.
      size_t path_len;

      /// \brief The degree of the node, that is, the number of edges emerging
      /// from the node
      size_t degree;

      /// \brief The character of the inserted string that is the
      /// label of the current node
      T c;

      node(){
        next = parent = path_len = first_child = 0;
        degree = 0; c = T();
      }
    };

    std::vector<node>   node_buffer;
    std::vector<size_t> nodes_indeces;

    /**
     * \brief This function adds a node marked with a value of x of type T to the list of
     *        children of the node parent_idx.
     * \param [in] parent_idx An index of a parent.
     * \param [in] x          An inserted value.
     * \return                The index of inserted node.
     */
    size_t add_child(size_t parent_idx, T x);

    /// \brief This function performs (possibly necessary) actions after the last
    /// character is inserted.
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
    /* The variable temp contains a node that you might need to insert. */
    temp.c = x; temp.degree = 0;
    temp.next = 0; temp.parent = parent_idx;
    temp.path_len = node_buffer[parent_idx].path_len + 1;
    if(!current){
        /* We can be here only if the node with the parent_idx index has no children at
         * all. This means that the added node will be the first in the list of children.
         * In this case the degree of node parent_idx will increase by one, and will
         * become equal to 1.
         */
        node_buffer.push_back(temp);
        size_t child_idx = node_buffer.size() - 1;
        node_buffer[parent_idx].first_child = child_idx;
        node_buffer[parent_idx].degree = 1;
        return child_idx;
    }
    while(current){
        // If there are children, then you need to go through the list of children.
        node current_node = node_buffer[current];
        if(current_node.c == x){
          /* If there is a child marked with the desired symbol (the symbol x),
           * then we need to return the index of this child. */
          return current;
        }else{
          previous = current; current = current_node.next;
        }
    }
    /* If there is no such child, then we need to add this child to the end
     * of the list of children. */
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

char32_t* Char_trie::get_cstring(size_t idx){
    size_t id_len = node_buffer[idx].path_len;
    char32_t* p = new char32_t[id_len + 1];
    p[id_len] = 0;
    size_t current = idx;
    size_t i       = id_len-1;
    /* Since idx is the index of the element in node_buffer containing the last
     * character of the inserted string, and each element of the vector node_buffer
     * contains the field parent that points to the element with the previous
     * character of the string, then to get the inserted string, which corresponds
     * to the index idx, as an array of characters, it is necessary to walk from
     * the element with index idx to the root. The characters of the inserted
     * string will be read from the end to the beginning. */
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