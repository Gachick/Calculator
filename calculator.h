#pragma once
#include "std_lib_facilities.h"

#include "token_stream.h"
#include "variables.h"

//------------------------------------------------------------------------------

// List of available funcions
const vector<string> func_list = {"sqrt", "pow", "tg", "ctg", "sum", "arcctg"};

// Prompts
const string input_prompt = "> ";
const string result_prompt = "= ";

//------------------------------------------------------------------------------
// Helper functions available in calculator
//------------------------------------------------------------------------------

double sqrtCalc();
double powCalc();
double tgCalc();
double ctgCalc();
double sumCalc();
double arcctgCalc();

//------------------------------------------------------------------------------
// Calculator grammar
//------------------------------------------------------------------------------

double primary();
double term();
double expression();
double declaration();
double statement();

//------------------------------------------------------------------------------

void calculate();
