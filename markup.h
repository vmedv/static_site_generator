#pragma once

#include <vector>
#include <string>

// base class for markup element, easy to translate gemtext to another markup languages (LaTeX, Markdown, etc)
struct markup {
protected:
    std::string content;
    explicit markup(std::string const& data);
public:
    virtual std::string to_html() const = 0;
    virtual ~markup();
};

struct text : markup {
public:
    explicit text(std::string const& data);
    std::string to_html() const override;
};

struct link : markup {
private:
    std::string address;
public:
    link(std::string const& addr, std::string const& data);
    std::string to_html() const override;
};

struct heading : markup {
private:
    unsigned level;
public:
    heading(std::string const& data, unsigned lvl);
    std::string to_html() const override;
};

struct list : markup {
public:
    list();
    void add_element(std::string const& el);
    std::string to_html() const override;
private:
    struct list_element : markup {
    public:
        explicit list_element(std::string const& data);
        std::string to_html() const override;
    };
    std::vector<list_element> elements;
};

struct blockquote : markup {
public:
    explicit blockquote(std::string const& data);
    std::string to_html() const override;
};

struct preformatted : markup {
public:
    explicit preformatted(std::string const& data);
    std::string to_html() const override;
};
