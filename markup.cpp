#include <string>

#include "markup.h"

static std::string simple_wrapper(std::string const& tag, std::string const& content) {
    return "<" + tag + ">" + content + "</" + tag + ">\n";
}

markup::markup(std::string const& data) : content(data) {}

markup::~markup() = default;

text::text(std::string const& data) : markup(data) {}
std::string text::to_html() const {
    return simple_wrapper("p", content);
}

link::link(std::string const& addr, std::string const& data) : markup(data), address(addr) {}
std::string link::to_html() const {
    return "<a href=\"" + address + "\">" + content + "</a>\n";
}

heading::heading(std::string const& data, unsigned lvl) : markup(data), level(lvl) {}
std::string heading::to_html() const {
    return "<h" + std::to_string(level) + ">" + content + "</h" + std::to_string(level) + ">\n";
}

list::list_element::list_element(const std::string &data) : markup(data) {}
std::string list::list_element::to_html() const {
    return simple_wrapper("li", content);
}

list::list() : markup(""), elements() {};
std::string list::to_html() const {
    std::string res("<ul>\n");
    for (auto const& entry : elements) {
        res += entry.to_html();
    }
    res += "</ul>\n";
    return res;
}
void list::add_element(const std::string &el) {
    elements.emplace_back(el);
}

blockquote::blockquote(const std::string &data) : markup(data) {}
std::string blockquote::to_html() const {
    return simple_wrapper("blockquote", content);
}

preformatted::preformatted(const std::string &data) : markup(data) {}
std::string preformatted::to_html() const {
    return simple_wrapper("pre", content);
}