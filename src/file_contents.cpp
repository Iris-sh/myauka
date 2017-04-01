/*
    File:    file_contents.cpp
    Author: Гаврилов Владимир Сергеевич
    Created:4 февраля 2016, 13:10
    e-mails: vladimir.s.gavrilov@gmail.com,
             gavrilov.vladimir.s@mail.ru,
             gavvs1977@yandex.ru
*/

#include "../include/file_contents.h"
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
}