#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>

enum class STATE {
    NORMAL,
    EXPRESSION,
    STRING
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
    IDENTIFIER
};

std::ostream& operator<<(std::ostream& os, STATE state) {
    switch (state) {
        case STATE::NORMAL:return os << "NORMAL\n";
        case STATE::EXPRESSION: return os << "EXPRESSION\n";
        case STATE::STRING: return os << "STRING\n";
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
    std::vector<std::string> storage;
    std::vector<std::string> str;
    std::vector<TOKEN> tokens;
    std::vector<std::string> lexems;
    STATE state = STATE::NORMAL;

    std::unordered_map<std::string, TOKEN_TYPE> KEYWORDS = {
        {"+", TOKEN_TYPE::PLUS},
        {"-", TOKEN_TYPE::MINUS},
        {"*", TOKEN_TYPE::MULT},
        {"/", TOKEN_TYPE::DIV},
        {"(", TOKEN_TYPE::LPAREN},
        {")", TOKEN_TYPE::RPAREN},
        {"%", TOKEN_TYPE::MOD}
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
        textDivision();
        tokenization();
        lexemification();
    }

    void stateChange(STATE state) {
        switch (state) {
        case STATE::NORMAL: 
            std::cout << "NORMAL\n";
            this->state = STATE::NORMAL;
            break;
        case STATE::EXPRESSION: 
            std::cout << "EXPRESSION\n";
            this->state = STATE::EXPRESSION;
            break;
        case STATE::STRING: 
            std::cout << "STRING\n";
            this->state = STATE::STRING;
            break;
        default: std::cout << "FATAL ERROR LEXER.stateChange\n";
        }
    }

    void textDivision() {
        char stringSep;

        for (int i = 0; i < s.length(); i++) {
            if (state == STATE::NORMAL) {
                if (s[i] == '\'' || s[i] == '\"') {
                    str.push_back(std::string(1, s[i]));
                    stateChange(STATE::STRING);
                    stringSep = s[i];
                }
                else if (isspace(s[i])) {

                    if (str.empty()) continue;

                    storage.push_back(stringify(str));
                    str.clear();
                }
                else {
                    stateChange(STATE::EXPRESSION);
                    str.push_back(std::string(1, s[i]));
                }
            }
            else if (state == STATE::EXPRESSION) {
                if (isspace(s[i])) {
                    storage.push_back(stringify(str));
                    str.clear();
                    stateChange(STATE::NORMAL);
                }
                else {
                    str.push_back(std::string(1, s[i]));
                }
            }
            else {
                if (s[i] == stringSep) {
                    stateChange(STATE::NORMAL);
                    str.push_back(std::string(1, s[i]));
                    storage.push_back(stringify(str));
                    str.clear();
                    stringSep = '\0';
                }
                else {
                    str.push_back(std::string(1, s[i]));
                }
            }

            if (i + 1 == s.length()) {
                stateChange(STATE::NORMAL);
                if(!str.empty()) storage.push_back(stringify(str));
                str.clear();
                stringSep = '\0';
            }
        }
    }

    std::string stringify(std::vector<std::string> str) {
        if (str.empty()) std::cout << "FATAL ERROR stringify\n";

        std::string returnString = "";
        for (const auto& s : str) {
            returnString += s;
        }

        return returnString;
    }

    std::vector<std::string> getStorage() {
        return storage;
    }

    void tokenization() {
        for (const auto& s : storage) {
            TOKEN token;

            if (KEYWORDS.count(s) > 0) {
                token.type = KEYWORDS[s];
            }
            else if (isdigit(s[0])) {
                token.type = TOKEN_TYPE::NUMBER;
                token.value = s;
            }
            else if (s[0] == '\'' || s[0] == '\"') {
                token.type = TOKEN_TYPE::STRING;
                token.value = s;
            }
            else {
                token.type = TOKEN_TYPE::IDENTIFIER;
                token.value = s;
            }

            tokens.push_back(token);
        }
    }

    void lexemification() {
        for (const auto& t : tokens) {
            std::string s;

            s = tokenToString(t.type);
            if (!t.value.empty()) {
                s += "(" + t.value + ")";
            }

            lexems.push_back(s);
        }
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
    std::vector<std::string> storage = lexer.getStorage();
    for (const auto& a : storage) {
        std::cout << a;
        std::cout << std::endl;
    }

    std::vector<std::string> lexems = lexer.getLexems();
    for (const auto& l : lexems) {
        std::cout << l;
        std::cout << std::endl;
    }

}
/*
    "ala ma kota o imieniu ' jarek ' ma on " 5 + 3 * ( 2 + 1 - 2 ) "lat"   
*/