#pragma once

#include <ios>
#include <cstdlib>
#include <string>

#include "markup.h"

// base parser class, abstract layer between input stream and real parser
struct base_parser {
private:
    std::istream& input;
public:
    explicit base_parser(std::istream& in);
    bool test(char c);
    int show();
    bool take(char c);
    void expect(char c);
    bool eof();
    std::string take_word();
    std::string take_line();
    std::string take_rest();
    void skip_ws();
};

struct gemtext_parser : base_parser {
public:
    explicit gemtext_parser(std::istream& in);

    // returns unique_ptr to a heap object which was parsed
    std::unique_ptr<markup> parse_element();
};
