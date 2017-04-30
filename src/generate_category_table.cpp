/*
    File:    generate_category_table.cpp
    Created: 03 February 2017 at 14:47 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/generate_category_table.h"
#include "../include/operations_with_sets.h"
#include "../include/list_to_columns.h"
#include "../include/print_char32.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>

using Set_of_char           = std::set<char32_t>;
using Category_name_and_set = std::pair<std::string, Set_of_char>;
using Categories_info       = std::vector<Category_name_and_set>;

/* The next function for a given character of type char32_t builds the set of categories
 * to which it belongs. Information about categories is transmitted in a vector whose
 * elements are of type Category_name_and_set. The set of categories is represented as a
 * number of the type uint64_t: the equality to zero of some category means that the
 * category with the corresponding number does not belong to this set, and the equality
 * to one that belongs to. */
static uint64_t construct_categories_set_for_char(char32_t c,
                                                  const Categories_info& categories_info)
{
    using operations_with_sets::is_elem;
    uint64_t result  = 0;
    uint64_t counter = 0;
    for(const auto& z : categories_info){
        if(is_elem(c, z.second)){
            result |= 1ULL << counter;
        }
        counter++;
    }
    return result;
}

#define INDENT_WIDTH 4

static std::string construct_category_enum(const Categories_info& categories_info){
    std::string result = "enum Category {\n";
    std::vector<std::string> category_names;
    for(const auto& z : categories_info){
        category_names.push_back(z.first);
    }
    category_names.push_back("Other");
    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;
    result += string_list_to_columns(category_names, f)+ "\n};\n";
    return result;
}

static std::string category_table_string_repres(const std::map<char32_t, uint64_t>& t,
                                                size_t num_of_categories)
{
    std::string result;
    std::string type_for_set_repres;
    switch(num_of_categories){
        case 1 ... 8:
            type_for_set_repres = "uint8_t";
            break;

        case 9 ... 16:
            type_for_set_repres = "uint16_t";
            break;

        case 17 ... 32:
            type_for_set_repres = "uint32_t";
            break;

        case 33 ... 64:
            type_for_set_repres = "uint64_t";
            break;

        default:
            ;
    }
    result = "static const std::map<char32_t, " + type_for_set_repres +
             "> categories_table = {\n";

    std::vector<std::string> entries;
    for(const auto& z : t){
        auto entry = "{" + show_char32(z.first) + ", " + std::to_string(z.second) + "}";
        entries.push_back(entry);
    }
    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 4;
    f.spaces_between_columns = 1;

    result += string_list_to_columns(entries, f)+ "\n};\n";

    return result;
}

static const std::string get_category_func_str =
    R"~(
uint64_t get_categories_set(char32_t c){
    auto it = categories_table.find(c);
    if(it != categories_table.end()){
        return it->second;
    }else{
        return 1ULL << Other;
    }
}
)~";

std::string generate_category_table(Info_for_constructing& info){
    using operations_with_sets::operator+;
    std::string     result;
    Categories_info categories_info;
    Set_of_char     categorized_chars;
    for(const auto& c : info.category_name){
        /* Look through the names of all categories and write down information
         * about the categories of characters in the categories_info vector. */
        auto cat_idx         = c.first;
        auto set_for_cat_idx = info.char_cat.get_set(cat_idx);
        auto x               = std::make_pair(c.second, set_for_cat_idx);
        categories_info.push_back(x);
        /* In addition, we collect in one set all the symbols belonging to a category. */
        categorized_chars    = categorized_chars + set_for_cat_idx;
    }

    /* Now for each character from the set categorized_chars we build a set of
     * categories to which it belongs */
    std::map<char32_t, uint64_t> splitting_characters_by_category;
    for(char32_t c : categorized_chars){
        splitting_characters_by_category[c] =
            construct_categories_set_for_char(c, categories_info);
    }

    /* Then create an enumeration of the names of the character categories. */
    auto category_enum = construct_category_enum(categories_info);
    auto cat_table_str =
        category_table_string_repres(splitting_characters_by_category,
                                     categories_info.size());

    result = category_enum + "\n" + cat_table_str + "\n" +
             get_category_func_str;
    return result;
}