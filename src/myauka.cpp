#include <cstdlib>
#include <cstdio>
#include <string>
#include <memory>
#include "../include/location.h"
#include "../include/main_parser.h"
#include "../include/char_conv.h"
#include "../include/file_contents.h"
#include <map>
#include "../include/move_by_ext_from_curr_dir.h"

/* Функция, открывающая файл с тестовым текстом. Возвращает строку с текстом, если открыть
   файл удалось и размер файла не равен нулю, и пустую строку в противном случае. */
std::u32string init_testing(const char* name){
    auto contents = get_contents(name);
    auto str      = contents.second;
    switch(contents.first){
        case Get_contents_return_code::Normal:
            if(!str.length()){
                puts("Длина файла равна нулю.");
                return U"";
            }else{
                return utf8_to_u32string(str.c_str());
            }
            break;

        case Get_contents_return_code::Impossible_open:
            puts("Невозможно открыть файл.");
            return U"";

        case Get_contents_return_code::Read_error:
            puts("Ошибка при чтении файла.");
            return U"";
    }
    return U"";
}

static const char* help_str =
    R"~(Usage: myauka [option] file
Options:
    --help    Display this text.
    --version Display version information)~";

static const char* version_str =
    R"~(Myauka, lexical analyzer generator, v.1.0.0
(c) Gavrilov Vladimir Sergeevich 2016)~";

typedef void (*FuncForKey)();

void version(){
    puts(version_str);
}

void help(){
    puts(help_str);
}

std::map<std::string, FuncForKey> option_func = {
    {"--version", version}, {"--help", help}
};


int main(int argc, char* argv[]){
    if(1 == argc){
        puts("Не заданы аргументы.");
        help();
        return 0;
    }

    auto it = option_func.find(argv[1]);
    if(it != option_func.end()){
        (it->second)();
        return 0;
    }

    std::u32string t = init_testing(argv[1]);
    if(t.length()){
        char32_t* p      = const_cast<char32_t*>(t.c_str());
        auto      loc    = std::make_shared<Location>(p);
        auto      mp     = std::make_shared<Main_parser>(loc);
        mp->compile();
        int       errors = mp->get_number_of_errors();
        if(!errors){
            bool t = move_by_ext_from_curr_dir("src", "cpp");
            if(!t){
                puts("Перемещение файлов *.cpp не удалось.");
            }else{
                 t = move_by_ext_from_curr_dir("include", "h");
                 if(!t){
                     puts("Перемещение файлов *.h не удалось.");
                }
            }
        }
    }

    return 0;
}
