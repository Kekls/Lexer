#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>

enum class STATE {
    DEFAULT,
    STRING,
    COMMENT_LINE,
    COMMENT_BLOCK
};

enum class TOKEN_TYPE {
    NONE,
    NUMBER,
    STRING,
    LPAREN,
    RPAREN,
    PLUS,
    MINUS,
    MULT,
    DIV,
    MOD,
    IDENTIFIER,
    COMMA,
    ASSIGN,
    COLON,
    SEMICOLON,
    LBRACE,
    RBRACE,
    LBRACK,
    RBRACK,
    LT,
    GT,
    DOT,
    NOT,
    COMMENT_LINE,
    COMMENT_BLOCK,
    PLUS_ASSIGN,
    MINUS_ASSIGN,
    MULT_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    EQUAL,
    NOT_EQUAL,
    LT_EQUAL,
    GT_EQUAL,
    AND,
    OR,
    INC,
    DEC,
    DO,
    TIMES,
    WHILE,
    IF,
    ELSE,
    VAR,
    BITWISE_AND,
    BITWISE_OR,
    NEW_LINE
};

std::ostream& operator<<(std::ostream& os, STATE state) {
    switch (state) {
    case STATE::DEFAULT: return os << "DEFAULT\n";
    case STATE::STRING: return os << "STRING\n";
    case STATE::COMMENT_LINE: return os << "COMMENT_LINE\n";
    case STATE::COMMENT_BLOCK: return os << "COMMENT_BLOCK\n";
    default: return os << "FATAL ERROR STATE\n";
    }
}

struct TOKEN {
    TOKEN_TYPE type;
    std::string value;
};

class LEXER {
private:
    std::string s;
    std::vector<std::string> lexems;
    STATE state = STATE::DEFAULT;

    std::unordered_map<std::string, TOKEN_TYPE> KEYWORDS = {
        { "do", TOKEN_TYPE::DO},
        { "times", TOKEN_TYPE::TIMES},
        { "while", TOKEN_TYPE::WHILE},
        { "if", TOKEN_TYPE::IF},
        { "else", TOKEN_TYPE::ELSE},
        { "var", TOKEN_TYPE::VAR}
    };

public:
    LEXER(std::string s) {
        this->s = s;
    }

    std::string getString() {
        return s;
    }

    void setString(std::string s) {
        this->s = s;
    }

    void start() {
        char stringSep = {};
        std::string buffer = {};

        char peek = {};

        for (int i = 0; i < s.length(); i++) {
            if (i + 1 < s.length()) {
                peek = s[i + 1];
            }

            if (state == STATE::COMMENT_BLOCK || state == STATE::COMMENT_LINE) {

                commentState(buffer, peek, i, stringSep);
            }
            else if (state == STATE::STRING) {

                stringState(buffer, i, stringSep);
            }
            else if (state == STATE::DEFAULT) {

                defaultState(buffer, peek, i, stringSep);
            }
            else {
                std::cout << "FATAL ERROR textDivision\n";
            }

            if (i + 1 == s.length()) {
                stateChange(STATE::DEFAULT);
                tokenize(buffer);
                stringSep = {};
            }
        }
    }

    void commentState(std::string& buffer, char peek, int& i, char& stringSep) {

        if (state == STATE::COMMENT_LINE && s[i] == '\\' && peek == 'n') {
            TOKEN token = {};
            token.type = TOKEN_TYPE::COMMENT_LINE;
            token.value = buffer;
            lexemification(token);

            buffer.clear();

            token = {};
            stateChange(STATE::DEFAULT);
            token.type = TOKEN_TYPE::NEW_LINE;
            lexemification(token);

            i++;
        }
        else if (state == STATE::COMMENT_BLOCK && s[i] == '*' && peek == '/') {
            TOKEN token = {};

            buffer += std::string(1, s[i]) + std::string(1, peek);
            token.type = TOKEN_TYPE::COMMENT_LINE;
            token.value = buffer;
            lexemification(token);

            stateChange(STATE::DEFAULT);
            buffer.clear();
            i++;
        }
        else {
            buffer += std::string(1, s[i]);
        }
    }

    void stringState(std::string& buffer, int& i, char& stringSep) {

        if (s[i] == stringSep) {
            TOKEN token = {};

            buffer += std::string(1, s[i]);
            token.type = TOKEN_TYPE::STRING;
            token.value = buffer;

            buffer.clear();
            stringSep = {};

            stateChange(STATE::DEFAULT);
            lexemification(token);
        }
        else {
            buffer += std::string(1, s[i]);
        }
    }

    void defaultState(std::string& buffer, char peek, int& i, char& stringSep) {

        TOKEN token = {};

        if (isspace(s[i])) {
            tokenize(buffer);
            return;
        }


        switch (s[i]) {

        case '/':
            if (peek == '*') { stateChange(STATE::COMMENT_BLOCK); i++; buffer = std::string(1, s[i - 1]) + std::string(1, s[i]); }
            else if (peek == '/') { stateChange(STATE::COMMENT_LINE); i++; buffer = std::string(1, s[i - 1]) + std::string(1, s[i]); }
            else if (peek == '=') { token.type = TOKEN_TYPE::DIV_ASSIGN; }
            else { token.type = TOKEN_TYPE::DIV; }
            break;
        case '\"':
        case '\'':
            buffer += std::string(1, s[i]);
            stateChange(STATE::STRING);
            stringSep = s[i];
            break;
        case '+':
            if (peek == '=') { i++; token.type = TOKEN_TYPE::PLUS_ASSIGN; }
            else if (peek == '+') { i++; token.type = TOKEN_TYPE::INC; }
            else { token.type = TOKEN_TYPE::PLUS; }
            break;
        case '-':
            if (peek == '=') { i++; token.type = TOKEN_TYPE::MINUS_ASSIGN; }
            else if (peek == '-') { i++; token.type = TOKEN_TYPE::DEC; }
            else { token.type = TOKEN_TYPE::MINUS; }
            break;
        case '*':
            if (peek == '=') { i++; token.type = TOKEN_TYPE::MULT_ASSIGN; }
            else { token.type = TOKEN_TYPE::MULT; }
            break;
        case '%':
            if (peek == '=') { i++; token.type = TOKEN_TYPE::MOD_ASSIGN; }
            else { token.type = TOKEN_TYPE::MOD; }
            break;
        case '=':
            if (peek == '=') { i++; token.type = TOKEN_TYPE::EQUAL; }
            else { token.type = TOKEN_TYPE::ASSIGN; }
            break;
        case '<':
            if (peek == '=') { i++; token.type = TOKEN_TYPE::LT_EQUAL; }
            else { token.type = TOKEN_TYPE::LT; }
            break;
        case '>':
            if (peek == '=') { i++; token.type = TOKEN_TYPE::GT_EQUAL; }
            else { token.type = TOKEN_TYPE::GT; }
            break;
        case '!':
            if (peek == '=') { i++; token.type = TOKEN_TYPE::NOT_EQUAL; }
            else { token.type = TOKEN_TYPE::NOT; }
            break;
        case '&':
            if (peek == '&') { i++; token.type = TOKEN_TYPE::AND; }
            else { token.type = TOKEN_TYPE::BITWISE_AND; }
            break;
        case '|':
            if (peek == '&') { i++; token.type = TOKEN_TYPE::OR; }
            else { token.type = TOKEN_TYPE::BITWISE_OR; }
            break;
        case '(':
            token.type = TOKEN_TYPE::LPAREN; break;
        case ')':
            token.type = TOKEN_TYPE::RPAREN; break;
        case '[':
            token.type = TOKEN_TYPE::LBRACK; break;
        case ']':
            token.type = TOKEN_TYPE::RBRACK; break;
        case '{':
            token.type = TOKEN_TYPE::LBRACE; break;
        case '}':
            token.type = TOKEN_TYPE::RBRACE; break;
        case '.':
            token.type = TOKEN_TYPE::DOT; break;
        case ',':
            token.type = TOKEN_TYPE::COMMA; break;
        case ':':
            token.type = TOKEN_TYPE::COLON; break;
        case ';':
            token.type = TOKEN_TYPE::SEMICOLON; break;
        case '\\':
            if (peek == 'n') { i++; token.type = TOKEN_TYPE::NEW_LINE; }
            break;
        default:
            buffer += std::string(1, s[i]);
        }


        if (token.type != TOKEN_TYPE::NONE) {
            tokenize(buffer);

            lexemification(token);
            return;
        }
    }

    void stateChange(STATE state) {
        switch (state) {
        case STATE::DEFAULT:
            std::cout << "DEFAULT\n";
            this->state = STATE::DEFAULT;
            break;
        case STATE::STRING:
            std::cout << "STRING\n";
            this->state = STATE::STRING;
            break;
        case STATE::COMMENT_LINE:
            std::cout << "COMMENT_LINE\n";
            this->state = STATE::COMMENT_LINE;
            break;
        case STATE::COMMENT_BLOCK:
            std::cout << "COMMENT_BLOCK\n";
            this->state = STATE::COMMENT_BLOCK;
            break;
        default: std::cout << "FATAL ERROR LEXER.stateChange\n";
        }
    }

    void tokenize(std::string& buffer) {
        TOKEN token = {};

        if (buffer.empty()) return;

        if (KEYWORDS.count(buffer) > 0) {
            token.type = KEYWORDS[buffer];
        }
        else if (isdigit(buffer[0])) {
            token.type = TOKEN_TYPE::NUMBER;
            token.value = buffer;
        }
        else {
            token.type = TOKEN_TYPE::IDENTIFIER;
            token.value = buffer;
        }

        buffer.clear();
        lexemification(token);
    }

    void lexemification(TOKEN token) {
        std::string s;

        s = tokenToString(token.type);
        if (!token.value.empty()) {
            s += "(" + token.value + ")";
        }

        lexems.push_back(s);

    }

    std::string tokenToString(TOKEN_TYPE t) {
        switch (t) {
        case TOKEN_TYPE::NONE: return "NONE";
        case TOKEN_TYPE::NUMBER: return "NUMBER";
        case TOKEN_TYPE::STRING: return "STRING";
        case TOKEN_TYPE::LPAREN: return "LPAREN";
        case TOKEN_TYPE::RPAREN: return "RPAREN";
        case TOKEN_TYPE::PLUS: return "PLUS";
        case TOKEN_TYPE::MINUS: return "MINUS";
        case TOKEN_TYPE::MULT: return "MULT";
        case TOKEN_TYPE::DIV: return "DIV";
        case TOKEN_TYPE::MOD: return "MOD";
        case TOKEN_TYPE::IDENTIFIER: return "IDENTIFIER";
        case TOKEN_TYPE::COMMA: return "COMMA";
        case TOKEN_TYPE::ASSIGN: return "ASSIGN";
        case TOKEN_TYPE::COLON: return "COLON";
        case TOKEN_TYPE::SEMICOLON: return "SEMICOLON";
        case TOKEN_TYPE::LBRACE: return "LBRACE";
        case TOKEN_TYPE::RBRACE: return "RBRACE";
        case TOKEN_TYPE::LBRACK: return "LBRACK";
        case TOKEN_TYPE::RBRACK: return "RBRACK";
        case TOKEN_TYPE::LT: return "LT";
        case TOKEN_TYPE::GT: return "GT";
        case TOKEN_TYPE::DOT: return "DOT";
        case TOKEN_TYPE::NOT: return "NOT";
        case TOKEN_TYPE::COMMENT_LINE: return "COMMENT_LINE";
        case TOKEN_TYPE::COMMENT_BLOCK: return "COMMENT_BLOCK";
        case TOKEN_TYPE::PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TOKEN_TYPE::MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TOKEN_TYPE::MULT_ASSIGN: return "MULT_ASSIGN";
        case TOKEN_TYPE::DIV_ASSIGN: return "DIV_ASSIGN";
        case TOKEN_TYPE::MOD_ASSIGN: return "MOD_ASSIGN";
        case TOKEN_TYPE::EQUAL: return "EQUAL";
        case TOKEN_TYPE::NOT_EQUAL: return "EQUAL";
        case TOKEN_TYPE::LT_EQUAL: return "LT_EQUAL";
        case TOKEN_TYPE::GT_EQUAL: return "GT_EQUAL";
        case TOKEN_TYPE::AND: return "AND";
        case TOKEN_TYPE::OR: return "OR";
        case TOKEN_TYPE::INC: return "INC";
        case TOKEN_TYPE::DEC: return "DEC";
        case TOKEN_TYPE::DO: return "DO";
        case TOKEN_TYPE::TIMES: return "TIMES";
        case TOKEN_TYPE::WHILE: return "WHILE";
        case TOKEN_TYPE::IF: return "IF";
        case TOKEN_TYPE::ELSE: return "ELSE";
        case TOKEN_TYPE::VAR: return "VAR";
        case TOKEN_TYPE::BITWISE_AND: return "BITWISE_AND";
        case TOKEN_TYPE::BITWISE_OR: return "BITWISE_OR";
        case TOKEN_TYPE::NEW_LINE: return "NEW_LINE";
        default: return "UNKNOWN";
        }
    }

    std::vector<std::string> getLexems() {
        return lexems;
    }
};

int main()
{
    std::string s;
    std::getline(std::cin, s);

    LEXER lexer(s);
    lexer.start();

    std::vector<std::string> lexems = lexer.getLexems();
    for (const auto& l : lexems) {
        std::cout << l;
        std::cout << std::endl;
    }

}
/*
    "ala ma kota o imieniu ' jarek ' ma on " 5 + 3 * ( 2 + 1 - 2 ) "lat"

    std::string ala += "ala ma kotka";

    /*-473=3===-(0 "dziala jej+ su'per'"'napewno?' + "tak'!'-0(" )(

    for (i = 0; i < 10; i++) {

    if ( i += 4 == 5 && 3-1 != 2 ){ do 5 times{print("hello world")}}
*/