#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <fstream>

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
    NEW_LINE,
    END_OF_FILE
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
    int line;
    int column;
    std::string value;
};

class LEXER {
private:
    std::vector<std::string> lexems;
    STATE state = STATE::DEFAULT;
    std::ifstream file;
    int currentLine;
    int currentColumn;
    bool debug;

    std::unordered_map<std::string, TOKEN_TYPE> KEYWORDS = {
        { "do", TOKEN_TYPE::DO},
        { "times", TOKEN_TYPE::TIMES},
        { "while", TOKEN_TYPE::WHILE},
        { "if", TOKEN_TYPE::IF},
        { "else", TOKEN_TYPE::ELSE},
        { "var", TOKEN_TYPE::VAR}
    };

public:
    LEXER(std::string path, bool debug) {
        file.open(path, std::ios::binary);

        if (!file) {
            std::cerr << "File '" << path << "' could not be opened!\n";
            exit;
        }

        if (debug == true) {
            this->debug = true;
        }
    }

    void start() {
        char stringSep = {};
        std::string buffer = {};

        char peek = {};
        char current = {};

        currentLine = 1;
        currentColumn = 1;

        while (file.get(current)) {
            int next = file.peek();

            if (next != EOF) { peek =  static_cast<char>(next); }
            else { peek = {}; }

            if (state == STATE::COMMENT_BLOCK || state == STATE::COMMENT_LINE) {

                commentState(buffer, current, peek, stringSep);
            }
            else if (state == STATE::STRING) {

                stringState(buffer, current, peek, stringSep);
            }
            else if (state == STATE::DEFAULT) {

                defaultState(buffer, current, peek, stringSep);
            }
            else {
                std::cout << "FATAL ERROR textDivision\n";
            }

            currentColumn++;

            if (next == EOF) {
                stateChange(STATE::DEFAULT);
                tokenize(buffer);

                stringSep = {};
                TOKEN token = {};

                token.type = TOKEN_TYPE::END_OF_FILE;

                lexemification(token);
            }
        }
    }

    void commentState(std::string& buffer, char current, char peek, char& stringSep) {

        if (state == STATE::COMMENT_LINE && current == '\n') {
            TOKEN token = {};
            token.type = TOKEN_TYPE::COMMENT_LINE;
            token.value = buffer;
            token.line = currentLine;
            token.column = currentColumn - buffer.length();
            lexemification(token);

            buffer.clear();

            token = {};
            stateChange(STATE::DEFAULT);
            token.type = TOKEN_TYPE::NEW_LINE;
            currentColumn = 0;
            currentLine++;
            lexemification(token);

        }
        else if (state == STATE::COMMENT_BLOCK && current == '*' && peek == '/') {
            TOKEN token = {};

            buffer += std::string(1, current) + std::string(1, peek);
            token.type = TOKEN_TYPE::COMMENT_BLOCK;
            token.value = buffer;
            lexemification(token);

            stateChange(STATE::DEFAULT);
            buffer.clear();
            file.get();
        }
        else {
            if (current == '\r') {
                return;
            }

            if (state == STATE::COMMENT_BLOCK && current == '\n') {
                currentColumn = 0;
                currentLine++;
            }
            buffer += std::string(1, current);
        }
    }

    void stringState(std::string& buffer, char current, char peek, char& stringSep) {

        if (current == stringSep) {
            TOKEN token = {};

            buffer += std::string(1, current);
            token.type = TOKEN_TYPE::STRING;
            token.value = buffer;
            token.line = currentLine;
            token.column = currentColumn - buffer.length();
            

            buffer.clear();
            stringSep = {};

            stateChange(STATE::DEFAULT);
            lexemification(token);
        }
        else {
            buffer += std::string(1, current); 
        }
    }

    void defaultState(std::string& buffer, char current, char peek, char& stringSep) {

        if (current == '\n') {
            tokenize(buffer);
            lexemification({ TOKEN_TYPE::NEW_LINE, currentLine, currentColumn });
            currentColumn = 0;
            currentLine++;
            return;
        }
        if (current == '\r') {
            if (peek == '\n') { file.get(); }
            tokenize(buffer);
            lexemification({ TOKEN_TYPE::NEW_LINE, currentLine, currentColumn });
            currentColumn = 0;
            currentLine++;
            return;
        }
        if (isspace(current)) {
            tokenize(buffer);
            return;
        }

        switch (current) {

        case '/':
            tokenize(buffer);
            if (peek == '*') { stateChange(STATE::COMMENT_BLOCK); buffer = std::string(1, current) + std::string(1, peek); file.get(); currentColumn++; }
            else if (peek == '/') { stateChange(STATE::COMMENT_LINE); buffer = std::string(1, current) + std::string(1, peek); file.get(); currentColumn++; }
            else if (peek == '=') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::DIV_ASSIGN, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::DIV, currentLine, currentColumn }); }
            break;

        case '\"':
        case '\'':
            tokenize(buffer);
            buffer += std::string(1, current);
            stateChange(STATE::STRING);
            stringSep = current;
            break;

        case '+':
            tokenize(buffer);
            if (peek == '=') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::PLUS_ASSIGN, currentLine, currentColumn - 1 }); }
            else if (peek == '+') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::INC, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::PLUS, currentLine, currentColumn }); }
            break;

        case '-':
            tokenize(buffer);
            if (peek == '=') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::MINUS_ASSIGN, currentLine, currentColumn - 1 }); }
            else if (peek == '-') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::DEC, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::MINUS, currentLine, currentColumn }); }
            break;

        case '*':
            tokenize(buffer);
            if (peek == '=') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::MULT_ASSIGN, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::MULT, currentLine, currentColumn }); }
            break;

        case '%':
            tokenize(buffer);
            if (peek == '=') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::MOD_ASSIGN, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::MOD, currentLine, currentColumn }); }
            break;

        case '=':
            tokenize(buffer);
            if (peek == '=') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::EQUAL, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::ASSIGN, currentLine, currentColumn }); }
            break;

        case '<':
            tokenize(buffer);
            if (peek == '=') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::LT_EQUAL, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::LT, currentLine, currentColumn }); }
            break;

        case '>':
            tokenize(buffer);
            if (peek == '=') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::GT_EQUAL, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::GT, currentLine, currentColumn }); }
            break;

        case '!':
            tokenize(buffer);
            if (peek == '=') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::NOT_EQUAL, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::NOT, currentLine, currentColumn }); }
            break;

        case '&':
            tokenize(buffer);
            if (peek == '&') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::AND, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::BITWISE_AND, currentLine, currentColumn }); }
            break;

        case '|':
            tokenize(buffer);
            if (peek == '|') { file.get(); currentColumn++; lexemification({ TOKEN_TYPE::OR, currentLine, currentColumn - 1 }); }
            else { lexemification({ TOKEN_TYPE::BITWISE_OR, currentLine, currentColumn }); }
            break;

        case '(': tokenize(buffer); lexemification({ TOKEN_TYPE::LPAREN, currentLine, currentColumn }); break;
        case ')': tokenize(buffer); lexemification({ TOKEN_TYPE::RPAREN, currentLine, currentColumn }); break;
        case '[': tokenize(buffer); lexemification({ TOKEN_TYPE::LBRACK, currentLine, currentColumn }); break;
        case ']': tokenize(buffer); lexemification({ TOKEN_TYPE::RBRACK, currentLine, currentColumn }); break;
        case '{': tokenize(buffer); lexemification({ TOKEN_TYPE::LBRACE, currentLine, currentColumn }); break;
        case '}': tokenize(buffer); lexemification({ TOKEN_TYPE::RBRACE, currentLine, currentColumn }); break;
        case '.': tokenize(buffer); lexemification({ TOKEN_TYPE::DOT, currentLine, currentColumn }); break;
        case ',': tokenize(buffer); lexemification({ TOKEN_TYPE::COMMA, currentLine, currentColumn }); break;
        case ':': tokenize(buffer); lexemification({ TOKEN_TYPE::COLON, currentLine, currentColumn }); break;
        case ';': tokenize(buffer); lexemification({ TOKEN_TYPE::SEMICOLON, currentLine, currentColumn }); break;

        case '\\':
            if (peek == 'n') {
                tokenize(buffer);
                file.get();
                currentColumn++;
                lexemification({ TOKEN_TYPE::NEW_LINE, currentLine, currentColumn });
            }
            break;

        default:
            buffer += std::string(1, current);
            break;
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
        if (buffer.empty()) return;

        TOKEN token = {};

        token.line = currentLine;
        token.column = currentColumn - static_cast<int>(buffer.length());
        token.value = buffer;


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

        if (debug) {
            std::cout << s << std::endl;
            std::cout << "LINE: " << token.line << std::endl;
            std::cout << "COLUMN: " << token.column << std::endl;
        }

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
        case TOKEN_TYPE::END_OF_FILE: return "END_OF_FILE";
        default: return "UNKNOWN";
        }
    }

    std::vector<std::string> getLexems() {
        return lexems;
    }
};

int main()
{
    std::string path;
    //std::getline(std::cin, s);

    path = "readFile.kx";

    LEXER lexer(path, true);
    lexer.start();

    std::vector<std::string> lexems = lexer.getLexems();

    std::cout << "___________________________ LEXEMS DUMP ___________________________\n";
     
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
/* TODO

* Comment line needing \n to be categorized as COMMENT_LINE (if not it is IDENTIFIER)
* weird COMMENT_LINE output: )OMMENT_LINE(// something

*/