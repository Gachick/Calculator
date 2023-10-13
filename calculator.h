#pragma once
#include "std_lib_facilities.h"

#include "token_stream.h"
#include "variables.h"

//------------------------------------------------------------------------------

// List of available funcions
const vector<string> func_list = {"sqrt", "pow"};

// Prompts
const string input_prompt = "> ";
const string result_prompt = "= ";

//------------------------------------------------------------------------------
// Helper functions available in calculator
//------------------------------------------------------------------------------

double sqrt_calc();
double pow_calc();

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
