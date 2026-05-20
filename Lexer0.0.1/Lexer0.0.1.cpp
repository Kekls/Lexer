#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <unordered_set>

enum class STATE {
    DEFAULT,
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
    IDENTIFIER,
    COMMA,
    EQUAL
};

std::ostream& operator<<(std::ostream& os, STATE state) {
    switch (state) {
    case STATE::DEFAULT: return os << "DEFAULT\n";
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
    std::string t;
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
        {"=", TOKEN_TYPE::EQUAL}
    };

    std::unordered_set<std::string> stringCut = { "+", "-", "*", "/", "(", ")", "%", ",", ".", "=" };


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
        default: std::cout << "FATAL ERROR LEXER.stateChange\n";
        }
    }

    void textDivision() {
        char stringSep;

        for (int i = 0; i < s.length(); i++) {
            if (state == STATE::STRING) {

                if (s[i] == stringSep) {
                    stateChange(STATE::DEFAULT);
                    t += std::string(1, s[i]);
                    tokenization(t);
                    t.clear();
                    stringSep = '\0';
                }
                else {
                    t += std::string(1, s[i]);
                }
            }
            else if (state == STATE::DEFAULT) {
                if (stringCut.contains(std::string(1, s[i]))) {
                    if (!t.empty()) tokenization(t);
                    t.clear();
                    t += std::string(1, s[i]);
                    tokenization(t);
                    t.clear();
                }
                else {
                    if (s[i] == '\'' || s[i] == '\"') {
                        t += std::string(1, s[i]);
                        stateChange(STATE::STRING);
                        stringSep = s[i];
                    }
                    else if (isspace(s[i])) {

                        if (t.empty()) continue;

                        tokenization(t);
                        t.clear();
                    }
                    else {
                        t += std::string(1, s[i]);
                    }
                }
            }
            else {
                std::cout << "FATAL ERROR textDivision\n";
            }

            if (i + 1 == s.length()) {
                stateChange(STATE::DEFAULT);
                if (!t.empty()) tokenization(t);
                t.clear();
                stringSep = '\0';
            }
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
        case TOKEN_TYPE::EQUAL: return "EQUAL";
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

    // /*-473=3===-(0 "dziala jej+ su'per'"'napewno?' + "tak'!'-0(" )(
*/