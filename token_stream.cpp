#include "token_stream.h"

bool Token_stream::lineEnded()
{
    stream.peek();
    return stream.eof();
}

void Token_stream::getNewLine()
{
    string newLine;

    if (cin.eof())
        throw runtime_error("End of input");

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
