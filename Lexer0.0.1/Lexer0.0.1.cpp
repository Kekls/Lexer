#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <unordered_set>

enum class STATE {
    DEFAULT,
    STRING,
    COMMENT_LINE,
    COMMENT_BLOCK
};

enum class TOKEN_TYPE {
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
    VAR
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
        {"+", TOKEN_TYPE::PLUS},
        {"-", TOKEN_TYPE::MINUS},
        {"*", TOKEN_TYPE::MULT},
        {"/", TOKEN_TYPE::DIV},
        {"(", TOKEN_TYPE::LPAREN},
        {")", TOKEN_TYPE::RPAREN},
        {"%", TOKEN_TYPE::MOD},
        {",", TOKEN_TYPE::COMMA},
        {"=", TOKEN_TYPE::ASSIGN},
        {":", TOKEN_TYPE::COLON},
        { ";", TOKEN_TYPE::SEMICOLON},
        { "{", TOKEN_TYPE::LBRACE},
        { "}", TOKEN_TYPE::RBRACE},
        { "[", TOKEN_TYPE::LBRACK},
        { "]", TOKEN_TYPE::RBRACK},
        { "<", TOKEN_TYPE::LT},
        { ">", TOKEN_TYPE::GT},
        { ".", TOKEN_TYPE::DOT},
        { "!", TOKEN_TYPE::NOT},
        { "+=", TOKEN_TYPE::PLUS_ASSIGN},
        { "-=", TOKEN_TYPE::MINUS_ASSIGN},
        { "*=", TOKEN_TYPE::MULT_ASSIGN},
        { "/=", TOKEN_TYPE::DIV_ASSIGN},
        { "%=", TOKEN_TYPE::MOD_ASSIGN},
        { "==", TOKEN_TYPE::EQUAL},
        { "!=", TOKEN_TYPE::NOT_EQUAL},
        { "<=", TOKEN_TYPE::LT_EQUAL},
        { ">=", TOKEN_TYPE::GT_EQUAL},
        { "&&", TOKEN_TYPE::AND},
        { "||", TOKEN_TYPE::OR},
        { "++", TOKEN_TYPE::INC},
        { "--", TOKEN_TYPE::DEC},
        { "do", TOKEN_TYPE::DO},
        { "times", TOKEN_TYPE::TIMES},
        { "while", TOKEN_TYPE::WHILE},
        { "if", TOKEN_TYPE::IF},
        { "else", TOKEN_TYPE::ELSE},
        { "var", TOKEN_TYPE::VAR}
    };

    std::unordered_set<std::string> stringCut = { "+", "-", "*", "/", "(", ")", "%", ",", ".", "=", ":", ";", "{", "}", "[", "]", "<", ">", ".", "!", "+=", "-=", "*=", "/=", "%=", "==", "!=", "<=", ">=", "&&", "||", "++", "--" };

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
        char stringSep = '\0';
        std::string buffer;
        std::string peek = "\0";

        for (int i = 0; i < s.length(); i++) {
            if (i + 1 < s.length())
                peek = std::string(1, s[i]) + std::string(1, s[i + 1]);

            if (state == STATE::COMMENT_BLOCK || state == STATE::COMMENT_LINE) {

                commentState(buffer, peek, i);
            }
            else if (state == STATE::STRING) {

                stringState(buffer, peek, i, stringSep);
            }
            else if (state == STATE::DEFAULT) {
                
                defaultState(buffer, peek, i, stringSep);
            }
            else {
                std::cout << "FATAL ERROR textDivision\n";
            }

            if (i + 1 == s.length()) {
                stateChange(STATE::DEFAULT);
                if (!buffer.empty()) tokenization(buffer);
                buffer.clear();
                stringSep = '\0';
            }
        }
    }

    void commentState(std::string &buffer, std::string &peek, int &i) {

        if (state == STATE::COMMENT_LINE && s[i] == '\\' && s[i + 1] == 'n') {
            tokenization(buffer);
            buffer.clear();
            stateChange(STATE::DEFAULT);
            buffer += peek;
            tokenization(buffer);
            buffer.clear();
            i++;
        }
        else if (state == STATE::COMMENT_BLOCK && s[i] == '*' && s[i + 1] == '/') {
            buffer += peek;
            tokenization(buffer);
            stateChange(STATE::DEFAULT);
            buffer.clear();
            i++;
        }
        else {
            buffer += std::string(1, s[i]);
        }
    }

    void stringState(std::string& buffer, std::string& peek, int& i, char &stringSep) {

        if (s[i] == stringSep) {
            stateChange(STATE::DEFAULT);
            buffer += std::string(1, s[i]);
            tokenization(buffer);
            buffer.clear();
            stringSep = '\0';
        }
        else {
            buffer += std::string(1, s[i]);
        }
    }

    void defaultState(std::string& buffer, std::string& peek, int& i, char& stringSep) {

        if (s[i] == '/' && s[i + 1] == '*') {
            buffer = peek;
            stateChange(STATE::COMMENT_BLOCK);
            i++;
        }
        else if (s[i] == '/' && s[i + 1] == '/') {
            buffer = peek;
            stateChange(STATE::COMMENT_LINE);
            i++;
        }
        else if (stringCut.contains(peek)) {
            if (!buffer.empty()) tokenization(buffer);
            buffer.clear();
            buffer = peek;
            tokenization(buffer);
            buffer.clear();
            i++;
        }
        else if (stringCut.contains(std::string(1, s[i]))) {
            if (!buffer.empty()) tokenization(buffer);
            buffer.clear();
            buffer += std::string(1, s[i]);
            tokenization(buffer);
            buffer.clear();
        }
        else {
            if (s[i] == '\'' || s[i] == '\"') {
                buffer += std::string(1, s[i]);
                stateChange(STATE::STRING);
                stringSep = s[i];
            }
            else if (isspace(s[i])) {

                if (buffer.empty()) return;

                tokenization(buffer);
                buffer.clear();
            }
            else {
                buffer += std::string(1, s[i]);
            }
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

    void tokenization(std::string t) {
        TOKEN token;

        if (KEYWORDS.count(t) > 0) {
            token.type = KEYWORDS[t];
        }
        else if (isdigit(t[0])) {
            token.type = TOKEN_TYPE::NUMBER;
            token.value = t;
        }
        else if (t[0] == '\'' || t[0] == '\"') {
            token.type = TOKEN_TYPE::STRING;
            token.value = t;
        }
        else if (t[0] == '/' && t[1] == '/') {
            token.type = TOKEN_TYPE::COMMENT_LINE;
            token.value = t;
        }
        else if (t[0] == '/' && t[1] == '*') {
            token.type = TOKEN_TYPE::COMMENT_BLOCK;
            token.value = t;
        }
        else {
            token.type = TOKEN_TYPE::IDENTIFIER;
            token.value = t;
        }

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