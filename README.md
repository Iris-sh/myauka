# Introduction

Project Myauka is a generator of lexical analyzers, generating the text of a lexical analyzer in C++. By now, there are quite a few such generators, for example [Coco/R](http://www.ssw.uni-linz.ac.at/Coco),
[flex](http://flex.sourceforge.net), [flex++](http://www.mario-konrad.ch/wiki/doku.php?id=programming:flexbison:flexppbisonpp), [flexc++](http://flexcpp.sourceforge.net), and this list is far from complete.
However, all these generators have one common drawback. The disadvantage is that these generators essentially automate only the tasks of checking the correctness of writing and detecting the beginning of lexemes, and the generation of the value of the lexeme by its string representation must be performed by the function written by the user of the generator, called after verification of the correctness of the token. In this case, firstly, the passage through the fragment of the input text is performed twice, and secondly, it is necessary to manually implement part of the finite automaton constructed by the generator of lexical analyzers. The proposed generator is aimed at eliminating this disadvantage.  

# Input file format

The input file with the description of a lexical analyzer consists of the sequence of the following commands (only the command %codes is required of them), which can go in any order:  
%scaner\_name name\_of\_scaner  
%codes\_type name\_of\_type\_of\_lexeme\_codes  
%ident\_name name\_of\_identifier  
%token\_fields added\_fields\_of\_lexeme  
%class\_members added\_members\_of\_scaner\_class  
%header\_additions additions\_to\_the\_header\_file  
%impl\_additions additions\_to\_the\_implementation\_file  
%lexem\_info\_name name\_of\_the\_type\_of\_a\_lexeme\_information  
%newline\_is\_lexem  
%codes name\_of\_lexeme\_code {, name\_of\_lexeme\_code}  
%keywords [actions\_after\_finishing:] string\_representing\_the\_keyword : code\_of\_the\_keyword {, string\_representing\_the\_keyword : code\_of\_the\_keyword}  
%delimiters [actions\_after\_finishing:] string\_representing\_the\_delimiter : code\_of\_the\_delimiter {, string\_representing\_the\_delimiter : code\_of\_the\_delimiter}  
%idents '{'description\_of_the\_identifier\_begin'}' '{'description\_of\_the\_identifier\_body'}'  
%numbers [initialization\_actions]:[actions\_after\_finishing] {%action name\_of\_the\_action action\_definition} '{'expression'}'  
%strings [initialization\_actions]:[actions\_after\_finishing] {%action name\_of\_the\_action action\_definition} '{'expression'}'  
%comments [%single\_lined begin\_of\_a\_single-line\_comment] [%multilined [%nested] begin\_of\_multi-line\_comment : end\_of\_multi-line\_comment]  

Before explaining the meaning of each of the above constructions, we agree that everything enclosed in square brackets is optional, and everything enclosed in braces can be repeated any number of times, including never. In addition, '{' and '}' denote the curly braces themselves.  

Further, we note that under the string literal of Myauka (hereinafter simply a string literal) will be understood any (including empty) character sequence enclosed in double quotes. If you want to specify the double quotation in this sequence, then this quote must be doubled.  

Let's now turn to the explanation of the commands describing the lexical analyzer (hereinafter, for brevity, the scanner).  

First of all, if the command  

>%scaner_name name\_of\_scaner  

is specified, then an entry of the form  
```c++
class name_of_scaner {
    ...
};
```
is appeared.  

And this header file will be called name\_of\_scaner'.h. The corresponding implementation file will be called name\_of\_scaner'.cpp, where name\_of\_scaner' is name\_of\_scaner converted to lowercase. 
Default name\_of\_scaner is Scaner.    

Further, if the command  

>%codes_type name\_of\_type\_of\_lexeme\_codes  

is specified, then the command %codes generates an entry of the form

```c++
enum name_of_type_of_lexeme_codes : unsigned short {  
    NONE,  
    UNKNOWN,  
    name_of_lexeme_code1,  
    ...  
    name_of_lexeme_codeN  
};
```
in the file name\_of\_scaner'.h, where где name\_of\_lexeme\_code1, ..., name\_of\_lexeme\_codeN are names of lexeme codes defined in the section %codes. The default name of the type of lexeme codes is Lexem_code.  

The command  

> %ident\_name name\_of\_identifier  

specifies the name of the code of the lexeme for the lexeme 'identifier'. If there are no identifiers in the language for which the scanner is written, then the command %ident_name is optional.  

If you need to add some fields into the lexeme description, then you need to write the command  

>%token\_fields added\_fields\_of\_lexeme  

where added\_fields\_of\_lexeme{  is the string literal with the description of the needed fields. For example, if the lexeme can take both values of the type \_\_float128, and the values of the type \_\_int128, and (according to the conditions of the problem) the field having the type \_\_float128 must be named x, but the field having the type \_\_int128 must be named y, then a string literal with fields added to the lexeme can look, for example, like this:  

>"\_\_float128 x;  
>\_\_int128    y;"  

In addition, if you need to add members that are necessary for some calculations, then you need to write  

%class_members added\_members\_of\_scaner\_class  

where added\_members\_of\_scaner\_class is a string literal containing a list of members added to scanner. For example, if you need to add 

>\_\_int128   integer\_value;  
>\_\_float128 integer\_part;  
>\_\_float128 fractional\_part;  
>\_\_float128 exponent;  

then instead of added\_members\_of\_scaner\_class you need to write  

>"\_\_int128   integer\_value;  
>\_\_float128 integer\_part;  
>\_\_float128 fractional\_part;  
>\_\_float128 exponent;"  

If it is necessary that the character '\\n' (the newline character) be a separate lexeme, and not whitespace character, then you need to specify the command  
>%newline_is_lexem  

The required section %codes contains the comma separated list of identifiers. These identfiers are names of codes of lexemes. Rules for constructing identifiers are the same as in C++. For instance, if the enumeration name with the token codes is not specified by the command %codes\_type, and the section %codes has the form  

>%codes  
>   Kw_if, Kw_then, Kw_else, Kw_endif  

then the enumeration  

```c++
enum Lexem_code : unsigned short {  
    NONE,    UNKNOWN,  
    Kw_if,   Kw_then,   
    Kw_else, Kw_endif  
};
```  

will be generated. In other words, two special lexeme codes are always defined: NONE, denoting the end of the processed text, and UNKNOWN, which denotes the unknown lexeme.  

In the section %keywords, the keywords of the language for which the scaner is written and the corresponding lexeme codes are specified. Codes of lexemes are taken from the section %codes. For example, if
there are keywords__if__, __then__, __else__, __endif__, and coressponding lexeme codes are Kw\_if, Kw\_then, Kw\_else, Kw\_endif, then the section %keywords should have the following form:  

>   %keywords  
>       ...  
>       "if"    : Kw_if,  
>       "then"  : Kw_then,  
>       "else"  : Kw_else,  
>       "endif" : Kw_endif  
>       ...  

Here the ellipsis indicates (possibly existing) a description of other keywords.  

The %idents section defines the structure of the identifier of the language for which the scanner is written. More precisely, description\_of_the\_identifier\_begin defines what can be at the beginning of the identifier, and description\_of\_the\_identifier\_body defines the structure of the identifier body.  

In the section %delimiters, the operation signs and the delimiters of the language for which the scaner is written and the corresponding lexeme codes are specified. Codes of lexemes are taken from the section %codes. 
For example, if the language has delimiters \<, \>, \<=, \>=, =, !=, and corresponding lexeme codes are del\_LT, del\_GT, del\_LEQ, del\_GEQ, del\_EQ, del\_NEQ, then the section %delimiters should have the form  

>   %delimiters  
>       ...  
>       "<"  : del_LT,  
>       ">"  : del_GT,  
>       "<=" : del_LEQ,  
>       ">=" : del_GEQ,  
>       "="  : del_EQ,  
>       "!=" : del_NEQ  
>       ...  

Here the ellipsis indicates (possibly existing) a description of other delimiters and operation signs.  

The section %numbers specifies the regular expression that defines the structure of numbers. In this regular expression, character processing actions are embedded. Each of actions must be descripted by the command  

>%action name\_of\_the\_action action\_definition  

where name\_of\_the\_action is the C ++ language identifier, which is the name of the action to be determined, and action\_definition is the string literal containing C++ code that performs the required action.  

The section %strings describes the structure of character literals and string literals (if there are such literals). The section %string is organized in the same way as the section %numbers. If the section %string is specified, then
the scanner class contains members std::string buffer и int char\_code.  

Finally, the section %comments describes the structure of comments.  

Here the command   

>%single_lined begin\_of\_a\_single-line\_comment  

defines the structure of the single-line comment, where begin\_of\_a\_single-line\_comment is a string literal representing the sequence of characters that is the beginning of a single-line comment.  

The command  

>%multilined [%nested] begin\_of\_multi-line\_comment : end\_of\_multi-line\_comment  

defines the structure of the multi-line comment. Namely, begin\_of\_multi-line\_comment and end\_of\_multi-line\_comment are string literals that are sequences of characters beginning and ending multi-line comment, respectively.
If the keyword %nested is specified, then the multi-line comment can be nested.  

We now explain that the generator of lexical analyzers Myauka is meant by the description the beginning identifier, a description of the body of the identifier and the regular expression:  

description\_of_the\_identifier\_begin → expr  
description\_of\_the\_identifier\_body → expr  
expr → expr0 {'|' expr0}  
expr0 → expr1 { expr1}  
expr1 → expr2[?|\* |+]  
expr2 → character | character\_class  
character\_class → [:Latin:] | [:latin:] | [:Russian:] | [:russian:] | [:bdigits:] |  [:odigits:] | [:digits:] | [:xdigits:] | [:Letter:] | [:letter:] | [:nsq:] | [:ndq:]  

expression → expression0 {'|' expression0}  
expression0 → expression1 {expression1}  
expression1 → expression2[?|*|+]  
expression2 → expression3[$name\_of\_the\_action]  
expression3 → character | character\_class | (expression)  

В этой грамматике под словом ''символ'' понимается следующее: любой непробельный символ, кроме символов '|', '\*', +', '?', '\\$', '\\', '"', и символа перехода на новую строку, в файле с описанием сканера представляет самого себя. Если же эти символы нужно указать в регулярном выражении, то следует их записывать как '\\|', '\\\*', '\\+', '\\?', '\\$', '\\\\', '\\"', '\\n' соответственно. При этом все пробельные  символы (то есть символы, коды которых не превосходят кода пробела) генератором лексических анализаторов Мяука игнорируются.  

Ниже приводится список допустимых классов символов.

- [:Latin:]   Прописные латинские буквы от 'A' до 'Z'.   
- [:latin:]   Строчные латинские буквы от 'a' до 'z'.  
- [:Russian:] Прописные русские буквы от 'А' до 'Я' (включая букву 'Ё').   
- [:russian:] Строчные русские буквы от 'а' до 'я' (включая букву 'ё').  
- [:bdigits:] Символы двоичных цифр, т.е. символы '0' и '1'.  
- [:odigits:] Символы восьмеричных цифр, т.е. символы '0', '1', '2', '3', '4', '5', '6', '7'.  
- [:digits:]  Символы десятичных цифр, т.е. символы '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'.  
- [:xdigits:] Символы шестнадцатеричных цифр, т.е. символы '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'a', 'b', 'c', 'd', 'e', 'f'.  
- [:Letter:]  Прописные латинские буквы от 'A' до 'Z' и прописные русские буквы от 'А' до 'Я' (включая букву 'Ё').   
- [:letter:]  Строчные латинские буквы от 'a' до 'z' и строчные русские буквы от 'а' до 'я' (включая букву 'ё').  
- [:nsq:]     Символы, отличные от одинарной кавычки (').  
- [:ndq:]     Символы, отличные от двойной кавычки (").  

Из всех этих классов символов классы [:nsq:] и [:ndq:] допускаются только в разделе %strings.

# Building

To build the generator Myauka, you need to use the build system [Murlyka](https://github.com/gavr-vlad-s/murlyka). The only external dependency for the project Myauka is boost::system and boost::filesystem.




