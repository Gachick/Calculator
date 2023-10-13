
#include "std_lib_facilities.h"

//------------------------------------------------------------------------------

// Типы лексем
const char let = 'L';
const char func = 'F';
const char print = ';';
const char number = '8';
const char name = 'a';
const char quit = 'q';

// Ключевые слова
const string declkey = "let";
const string exitkey = "exit";

// Функции
const vector<string> func_list = {"sqrt", "pow"};

// Приглашение и результат
const string input_prompt = "> ";
const string result_prompt = "= ";

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

  bool lineEnded();
  void getNewLine();

  template <typename... Args>
  [[noreturn]] void error_with_putback(Token t, Args... args);
};

//------------------------------------------------------------------------------

bool Token_stream::lineEnded()
{
  stream.peek();
  return stream.eof();
}

void Token_stream::getNewLine()
{
  string newLine;
  getline(cin, newLine);
  newLine += ";";
  stream = stringstream{newLine};
}

void Token_stream::putback(Token t)
{
  if (full)
    error("putback() into a full buffer");

  buffer = t;
  full = true;
}

Token Token_stream::get()
{
  if (full)
  {
    full = false;
    return buffer;
  }

  if (lineEnded())
    return Token(print);

  char ch;
  stream >> ch;

  switch (ch)
  {
  case '(':
  case ')':
  case '+':
  case '-':
  case '*':
  case '/':
  case '%':
  case ';':
  case '=':
  case ',':
    return Token(ch);

  case '.':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  {
    stream.putback(ch);
    double val;
    stream >> val;
    return Token(number, val);
  }

  default:
    if (isalpha(ch))
    {
      string s;
      s += ch;
      while (stream.get(ch) && (isalpha(ch) || isdigit(ch)))
        s += ch;
      stream.putback(ch);

      if (s == declkey)
        return Token(let);
      if (s == exitkey)
        return Token(quit);

      for (string func : func_list)
      {
        if (s == func)
          return Token('F', func);
      }

      return Token(name, s);
    }
    error("Bad token");
  }
}

void Token_stream::ignore(char c)
{
  if (full && c == buffer.kind)
  {
    full = false;
    return;
  }
  full = false;

  for (char ch; !lineEnded() && stream >> ch;)
  {
    if (ch == c)
      return;
  }
}

template <typename... Args>
void Token_stream::error_with_putback(Token t, Args... args)
{
  putback(t);
  error(args...);
}

Token_stream ts;

//------------------------------------------------------------------------------

struct Variable
{
  string name;
  double value;
  Variable(string n, double v) : name(n), value(v) {}
};

vector<Variable> var_table;

double get_value(const string &s)
{
  for (const auto &var : var_table)
  {
    if (var.name == s)
      return var.value;
  }

  error("get: undefined name ", s);
}

void set_value(const string &s, double d)
{
  for (auto &var : var_table)
  {
    if (var.name == s)
    {
      var.value = d;
      return;
    }
  }

  error("set: undefined name ", s);
}

bool is_declared(const string &s)
{
  for (const auto &var : var_table)
  {
    if (var.name == s)
      return true;
  }

  return false;
}

double define_name(const string &var, double val)
{
  if (is_declared(var))
    error(var, " declared twice");

  var_table.push_back(Variable{var, val});

  return val;
}

//------------------------------------------------------------------------------
double sqrt_calc();
double pow_calc();
//------------------------------------------------------------------------------

double expression();

double primary()
{
  Token t = ts.get();
  switch (t.kind)
  {
  case '(':
  {
    double d = expression();
    t = ts.get();
    if (t.kind != ')')
      ts.error_with_putback(t, "'(' expected");
    return d;
  }

  case '-':
    return -primary();
  case '+':
    return +primary();

  case func:
    if (t.name == "sqrt")
      return sqrt_calc();
    else if (t.name == "pow")
      return pow_calc();

  case number:
    return t.value;

  case name:
    return get_value(t.name);

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
  if (is_declared(var))
    ts.error_with_putback(t, var, " declared twice");

  t = ts.get();
  if (t.kind != '=')
    ts.error_with_putback(t, "'=' missing in declaration of ", var);

  return define_name(var, expression());
}

double statement()
{
  Token t = ts.get();
  switch (t.kind)
  {
  case let:
    return declaration();
  default:
    ts.putback(t);
    return expression();
  }
}

void clean_up_mess()
{
  ts.ignore(print);
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
      while (t.kind == print)
        t = ts.get();
      if (t.kind == quit)
        return;
      ts.putback(t);
      double result = statement();
      cout << result_prompt << result << endl;
    }
    catch (runtime_error &e)
    {
      cerr << e.what() << endl;
      clean_up_mess();
    }
  }
}

//------------------------------------------------------------------------------

double sqrt_calc()
{
  double arg = primary();
  if (arg < 0)
    error("'sqrt' can't take negative arguments");
  return sqrt(arg);
}

double pow_calc()
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

//------------------------------------------------------------------------------

int main()
try
{
  define_name("pi", 3.141592653589793);
  define_name("e", 2.718281828459045);

  calculate();
}
catch (exception &e)
{
  cerr << "exception: " << e.what() << endl;
  return 1;
}
catch (...)
{
  cerr << "Oops, unknown exception" << endl;
  return 2;
}