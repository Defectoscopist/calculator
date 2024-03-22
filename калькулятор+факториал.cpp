#include "std_lib_facilities.h"

//------------------------------------------------------------------------------

class Token{
public:
    char kind;
    double value;
    Token(char ch)
        :kind(ch), value(0){}
    Token(char ch, double val)
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
        // удалить
        full = false;
        return buffer;
    }

    char ch;
    cin >> ch;  

    switch (ch) {
    case '=':    // для отображения
    case 'x':    // для выхода
    case '{': case '}': case '(': case ')': case '+': case '-': case '*': case '/': case '!':
        return Token(ch);        
    case '.':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '9':
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

Token_stream ts;        // 

//------------------------------------------------------------------------------

double expression();    // объявляем существование expression()

//------------------------------------------------------------------------------

// отделяем числа от скобок
double primary()
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

double factorial()
{
        double left = primary();
        Token t = ts.get();   
 
        while(true) {
                switch (t.kind) {
                case '!':
                        {
                                int result = 1;
                                for (int i = 1; i <= left; i++) result *= i;
                                left = result;
                                t = ts.get();
                                break;
                        }
                default: 
                        ts.putback(t);     // возвращаем t
                        return left;
                }
        }
}


//------------------------------------------------------------------------------

// умножение и деление
double term()
{
    double left = factorial();
        Token t = ts.get();
 
        while(true) {
            switch (t.kind) {
                case '!':
                    {
                        int result = 1;
                        for (int i = 1; i <= left; i++) result *= i;
                        left = result;
                        t = ts.get();
                        break;
                    }
                case '*':
                        left *= factorial();
                        t = ts.get();
                        break;
                case '/':
                    {    
                        double d = factorial();
                        if (d == 0) error("divide by zero");
                        left /= d; 
                        t = ts.get();
                        break;
                    }
                default: 
                        ts.putback(t);     // возвращаем t в поток
                        return left;
                }
        }
}

//------------------------------------------------------------------------------

// deal with + and -
double expression()
{
    double left = term();      // считываем и вычисляем Term
    Token t = ts.get();        // Получаем следующую лексему

    while (true) {
        switch (t.kind) {
        case '+':
            left += term();    // Вычисляем Term и добавляем его
            t = ts.get();
            break;
        case '-':
            left -= term();    // Вычисляем Term и вычитаем его
            t = ts.get();
            break;
        default:
            ts.putback(t);     // возвращаем t в поток
            return left;       // возвращаем ответ, если не осталось "+" и "-"
        }
    }
}

//------------------------------------------------------------------------------

int main()
try
{
    cout << "Добро пожаловать в программу–калькулятор!" << '\n'
    << "Пожалуйста, введите выражения, содержащее числа с плавающей точкой." << endl;
    double val;
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