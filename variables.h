#pragma once
#include "std_lib_facilities.h"

struct Variable
{
    string name;
    double value;
    Variable(string n, double v) : name(n), value(v) {}
};

class VariableList
{
    vector<Variable> var_table{};

public:
    double get_value(const string &s);
    void set_value(const string &s, double d);
    bool is_declared(const string &s);
    double define_name(const string &var, double val);
};