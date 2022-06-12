#include <fstream>
#include <memory>

#include "gemtext.h"
#include "markup.h"

base_parser::base_parser(std::istream &in) : input(in) {}

bool base_parser::test(char c) {
    int n = input.peek();
    return n == c;
}

int base_parser::show() {
    return input.peek();
}

bool base_parser::take(char c) {
    if (test(c)) {
        input.ignore(1);
        return true;
    }
    return false;
}

void base_parser::expect(char c) {
    int n = input.get();
    if (n != c) {
        throw std::runtime_error(std::string("Unexpected parsing error"));
    }
}

bool base_parser::eof() {
    return input.eof() || input.peek() == EOF;
}

void base_parser::skip_ws() {
    input >> std::ws;
}

std::string base_parser::take_word() {
    std::string res;
    input >> res;
    return res;
}

std::string base_parser::take_line() {
    std::string res;
    std::getline(input, res);
    return res;
}

std::string base_parser::take_rest() {
    std::string res;
    while (!input.eof()) {
        res += take_line() += '\n';
    }
    if (!res.empty()) res.pop_back();
    return res;
}

gemtext_parser::gemtext_parser(std::istream &in) : base_parser(in) {}

std::unique_ptr<markup> gemtext_parser::parse_element() {
    if (take('#')) {
        unsigned level = 1;
        while (take('#')) {
            level++;
        }
        expect(' ');
        return std::make_unique<heading>(take_rest(), level);
    }
    if (take('>')) {
        return std::make_unique<blockquote>(take_rest());
    }
    if (take('*')) {
        expect(' ');
        std::unique_ptr<list> l(new list());
        l->add_element(take_line());
        while (!eof()) {
            expect('*');
            expect(' ');
            l->add_element(take_line());
        }
        return l;
    }
    if (take('`')) {
        if (take('`')) {
            if (take('`')) {
                take_line(); // meta-data, hidden to user, don't know what to do with it
                return std::make_unique<preformatted>('\n' + take_rest());
            }
            return std::make_unique<text>("``" + take_rest());
        }
        return std::make_unique<text>("`" + take_rest());
    }
    if (take('=')) {
        if (take('>')) {
            skip_ws();
            std::string addr = take_word();
            skip_ws();
            return std::make_unique<link>(addr,take_rest());
        }
        return std::make_unique<text>('=' + take_rest());
    }
    return std::make_unique<text>(take_rest());
}
