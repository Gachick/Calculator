#include "calculator.h"

//------------------------------------------------------------------------------

Token_stream ts;
VariableList varList;

//------------------------------------------------------------------------------
// Helper functions available in calculator
//------------------------------------------------------------------------------

double sqrtCalc()
{
  double arg = primary();
  if (arg < 0)
    error("'sqrt' can't take negative arguments");
  return sqrt(arg);
}

double powCalc()
{
  Token t = ts.get();
  if (t.kind != '(')
    ts.error_with_putback(t, "'pow' expected an opening bracket");

  double left = expression();
  t = ts.get();
  if (t.kind != ',')
    ts.error_with_putback(t, "'pow' expected a coma");

  double right = expression();
  t = ts.get();
  if (t.kind != ')')
    ts.error_with_putback(t, "'pow' expected a closing bracket");

  return pow(left, right);
}

double tgCalc()
{
  double arg = primary();
  return tan(arg);
}

double ctgCalc()
{
  double arg = primary();
  return 1 / tan(arg);
}

double arcctgCalc()
{
  double arg = primary();
  return M_PI_2 - atan(arg);
}

double sumCalc()
{
  double res = 0;

  Token t = ts.get();
  if (t.kind != '(')
    ts.error_with_putback(t, "'sum' expected an opening bracket");

  while (t.kind != ')')
  {
    double arg = expression();
    res += arg;
    t = ts.get();
    if (t.kind != ',' && t.kind != ')')
      ts.error_with_putback(t, " invalid argument in 'sum'");
  }

  return res;
}

//------------------------------------------------------------------------------
// Calculator grammar
//------------------------------------------------------------------------------

double
primary()
{
  Token t = ts.get();
  switch (t.kind)
  {
  case '(':
  {
    double d = expression();
    t = ts.get();
    if (t.kind != ')')
      ts.error_with_putback(t, "')' expected");
    return d;
  }

  case '-':
    return -primary();
  case '+':
    return +primary();

  case func:
    if (t.name == "sqrt")
      return sqrtCalc();
    else if (t.name == "pow")
      return powCalc();
    else if (t.name == "tg")
      return tgCalc();
    else if (t.name == "ctg")
      return ctgCalc();
    else if (t.name == "arcctg")
      return arcctgCalc();
    else if (t.name == "sum")
      return sumCalc();

  case number:
    return t.value;

  case name:
    return varList.get_value(t.name);

  default:
    ts.error_with_putback(t, "primary expected");
  }
}

double term()
{
  double left = primary();
  while (true)
  {
    Token t = ts.get();

    switch (t.kind)
    {
    case '*':
      left *= primary();
      break;

    case '/':
    {
      double d = primary();
      if (d == 0)
        error("divide by zero");
      left /= d;
      break;
    }

    case '%':
    {
      double d = primary();
      if (d == 0)
        error("'%': divide by zero");
      left = fmod(left, d);
      break;
    }

    default:
      ts.putback(t);
      return left;
    }
  }
}

double expression()
{
  double left = term();
  while (true)
  {
    Token t = ts.get();

    switch (t.kind)
    {
    case '+':
      left += term();
      break;

    case '-':
      left -= term();
      break;

    default:
      ts.putback(t);
      return left;
    }
  }
}

double declaration()
{
  Token t = ts.get();
  if (t.kind != name)
    ts.error_with_putback(t, "name expected in declaration");

  string var = t.name;
  if (varList.is_declared(var))
    ts.error_with_putback(t, var, " declared twice");

  t = ts.get();
  if (t.kind != '=')
    ts.error_with_putback(t, "'=' missing in declaration of ", var);

  return varList.define_name(var, expression());
}

double statement()
{
  double res = 0;
  Token t = ts.get();
  switch (t.kind)
  {
  case let:
    res = declaration();
    break;
  default:
    ts.putback(t);
    res = expression();
  }
  t = ts.get();
  if (t.kind != ';')
    ts.error_with_putback(t, "invalid statement");
  return res;
}

void calculate()
{
  while (true)
  {
    if (ts.lineEnded())
    {
      cout << input_prompt;
      ts.getNewLine();
    }
    try
    {
      Token t = ts.get();
      if (t.kind == print)
        continue;
      if (t.kind == quit)
        return;
      ts.putback(t);

      double result = statement();
      cout << result_prompt << result << endl;
    }
    catch (runtime_error &err)
    {
      cerr << err.what() << endl;
      ts.ignore(print);
    }
  }
}

//------------------------------------------------------------------------------
#define E 2.718281828459045
#define PI 3.141592653589793

int main()
try
{
  varList.define_name("pi", PI);
  varList.define_name("e", E);

  calculate();
}
catch (exception &err)
{
  cerr << "exception: " << err.what() << endl;
  return 1;
}
catch (...)
{
  cerr << "Oops, unknown exception" << endl;
  return 2;
}