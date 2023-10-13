#pragma once
#include "std_lib_facilities.h"
#include "calculator.h"

//------------------------------------------------------------------------------

// Lexeme types
const char let = 'L';
const char func = 'F';
const char print = ';';
const char number = '8';
const char name = 'a';
const char quit = 'q';

// Keywords
const string declkey = "let";
const string exitkey = "exit";

//------------------------------------------------------------------------------

struct Token
{
    char kind;
    double value;
    string name;
    Token(char ch) : kind(ch), value(0) {}
    Token(char ch, double val) : kind(ch), value(val) {}
    Token(char ch, string val) : kind(ch), name(val) {}
};

class Token_stream
{
    stringstream stream{""};

    bool full{false};
    Token buffer{'\0'};

public:
    Token_stream() {}

    Token get();
    void putback(Token t);
    void ignore(char);

    // We make calculations line by line
    bool lineEnded();
    void getNewLine();

    // Making sure to not lose anything after an exception
    template <typename... Args>
    [[noreturn]] void error_with_putback(Token t, Args... args)
    {
        putback(t);
        error(args...);
    }
};