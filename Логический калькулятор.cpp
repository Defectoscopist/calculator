#include "std_lib_facilities.h"

//------------------------------------------------------------------------------

class Token{
public:
    char kind;
    bool value;
    Token(char ch)
        :kind(ch), value(0){}
    Token(char ch, bool val)
        :kind(ch), value(val){}
};

//------------------------------------------------------------------------------

class Token_stream {
public:
    Token_stream();   // создаем объект класса Token_stream, считывающий данные из потока cin
    Token get();      // получаем объект класса Token
    void putback(Token t);    // возварщаем Token
private:
    bool full;        // находится ли в буфере объект класса Token?
    Token buffer;     // здесь хранится объект класса Token, возвращаемый в поток функцией putback()
};

//------------------------------------------------------------------------------

// Конструктор (проверяем, все ли пусто)
Token_stream::Token_stream()
    :full(false), buffer(0)    // no Token in buffer
{
}

//------------------------------------------------------------------------------

// putback() закидывает токен обратно в буффер:
void Token_stream::putback(Token t)
{
    if (full) error("putback() into a full buffer");
    buffer = t;       // копируем t в буффер
    full = true;      // теперь он заполнен
}

//------------------------------------------------------------------------------

Token Token_stream::get() // считывание токенов из потока
{
    if (full) {       // если что-то есть
        full = false; // удалить
        return buffer;
    }

    char ch;
    cin >> ch;  

    switch (ch) {
    case '=':    // для отображения
    case 'x':    // для выхода
    case '{': case '}': case '(': case ')': case '|': case '^': case '&': case '~': case '!':
        return Token(ch);
    case '0': case '1':
    {
        cin.putback(ch);    
        double val;
        cin >> val;             
        return Token('8', val);   // Токен'8' с числом
    }
    default:
        error("Bad token");
        return Token(NULL);
    }
}

//------------------------------------------------------------------------------

Token_stream ts;        // хранит get() и putback()

//------------------------------------------------------------------------------

bool expression();    // объявляем существование expression()

//------------------------------------------------------------------------------

// отделяем числа от скобок
bool primary()
{
    double d = 0;
    Token t = ts.get();
    switch (t.kind) {
    
    case '{':    // для '{'выражения'}'
    {
        double d = expression();
        t = ts.get();
        if (t.kind != '}') error("'}' expected");
            return d;
    }
    
    case '(':    // для '('выражения')'
    {
        double d = expression();
        t = ts.get();
        if (t.kind != ')') error("')' expected");
            return d;
    }
    
    case '8':            // '8' значит "число"
        return t.value;  // которое мы и возвращаем
    default:
        error("primary expected");
        return NULL;
    }
}

bool conjunction()
{
        bool left = primary();
        Token t = ts.get();   
 
        while(true) {
                switch (t.kind) {
                case '&':
                        {
                            left &= primary();
                            t = ts.get();
                                break;
                        }
                default: 
                        ts.putback(t);     // возвращаем t
                        return left;
                }
        }
}

// & и ^
bool term()
{
    bool left = conjunction();
        Token t = ts.get();
 
        while(true) {
            switch (t.kind) {
                case '&':
                    {
                        left &= conjunction();
                        t = ts.get();
                        break;
                    }
                case '^':
                        left ^= conjunction();
                        t = ts.get();
                        break;
                default:
                        ts.putback(t);     // возвращаем t в поток
                        return left;
                }
        }
}

//------------------------------------------------------------------------------

// deal with + and -
bool expression()
{
    bool left = term();     
    Token t = ts.get();        

    while (true) {
        switch (t.kind) {
        case '|':
            left |= term();    
            t = ts.get();
            break;
        case '!':
            left = !left;    
            t = ts.get();
            break;
        default:
            ts.putback(t); 
            return left;     
        }
    }
}

//------------------------------------------------------------------------------

int main()
try
{
    cout << "Добро пожаловать в логический калькулятор!" << '\n'
    << "Пожалуйста, введите выражения, содержащее \"1\" и \"0\" и логические операторы: \"&\",\"|\",\"!\",\"^\"." << endl;
    bool val;
    while (cin) {
        Token t = ts.get();
        if (t.kind == 'x') break; // для выхода
        if (t.kind == '=')        // в конце выражания пишем равно, чтобы посчитать
            cout << val;
        else
            ts.putback(t);
        val = expression();
    }
    keep_window_open();
}
catch (exception& e) {
    cerr << "error: " << e.what() << '\n';
    keep_window_open();
    return 1;
}
catch (...) {
    cerr << "Oops: unknown exception!\n";
    keep_window_open();
    return 2;
}