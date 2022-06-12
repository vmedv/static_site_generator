#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include "gemtext.h"

static void gmi_to_html(std::filesystem::path const& from, std::filesystem::path const& to) {
    std::ifstream in(from.string());
    std::ofstream out(to.string());
    out << "<!DOCTYPE html>\n" << "<html>\n" << "<body>\n";
    bool read = false;
    std::string line_;
    while (!in.eof()) {
        if (!read) {
            std::getline(in, line_);
        }
        read = false;
        std::stringstream line(line_);
        if (line_.substr(0, 3) == "```") {
            read = true;
            do {
                line << line_ << '\n';
                std::getline(in, line_);
            } while(line_.substr(0, 3) != "```");
        } else if (line_.substr(0, 2) == "* ") {
            read = true;
            while(line_.substr(0, 2) == "* ") {
                line << line_ << '\n';
                std::getline(in, line_);
            }
        }
        gemtext_parser parser(line);
        std::unique_ptr res = parser.parse_element();
        out << res->to_html();
    }
    out << "</body>\n" << "</html>\n";
}

int safe_remove(std::filesystem::path const& p) {
    try {
        std::filesystem::remove(p);
    } catch (std::filesystem::filesystem_error const& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (std::bad_alloc const& e) {
        std::cerr << "Memory allocation error\n";
        return 1;
    }
    return 0;
}

bool is_subdirectory(std::filesystem::path const& a, std::filesystem::path const& b) {
    std::filesystem::path r = std::filesystem::relative(a, b);
    return !r.empty() && r.string().substr(0, 2) != "..";
}

int main(int argc, char** argv) {
    if (argc == 2) {
        std::string arg(argv[1]);
        if (arg == "--help") {
            std::cout << "Usage: \n"
                      << "./static_site_generator <input> <output>,\n"
                      << "where both arguments are paths to directories and output is not a subdirectory of input.\n";
            return 0;
        }
        std::cerr << "Unknown argument, see ./static_site_generator --help for help\n";
        return 1;
    }
    if (argc != 3) {
        std::cerr << "Unknown arguments, see ./static_site_generator --help for help\n";
        return 1;
    }

    std::filesystem::path input(argv[1]);
    std::filesystem::path output(argv[2]);
    std::filesystem::path cur = std::filesystem::current_path();
    if (!std::filesystem::is_directory(input)) {
        std::cerr << "Input directory doesn't exist.\n";
        return 1;
    }
    if (input.is_absolute()) {
        input = std::filesystem::relative(input, cur);
    }
    if (output.is_absolute()) {
        output = std::filesystem::relative(output, cur);
    }
    input.lexically_normal();
    output.lexically_normal();
    if (is_subdirectory(input, output)) {
        std::cerr << "Output directory is subdirectory of input.\n";
        return 1;
    }

    try {
        std::filesystem::create_directories(output);
    } catch (std::filesystem::filesystem_error const& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (std::bad_alloc const& e) {
        std::cerr << "Memory allocation error\n";
    }

    for (const auto& cur_entry : std::filesystem::recursive_directory_iterator(input)) {
        auto rel_path = std::filesystem::relative(cur_entry.path(), input);
        if (cur_entry.is_directory()) {
            try {
                std::filesystem::create_directory(output / rel_path);
            } catch (std::filesystem::filesystem_error const& e) {
                std::cerr << e.what() << std::endl;
                return 1;
            } catch (std::bad_alloc const& e) {
                std::cerr << "Memory allocation error\n";
                return 1;
            }
        } else {
            std::filesystem::path cur_out = output / rel_path;
            if (rel_path.extension() == ".gmi") {
                std::filesystem::path cur_out_html = cur_out.parent_path() / cur_out.stem() += ".html";
                if (std::filesystem::exists(cur_out_html)) {
                    int i = safe_remove(cur_out_html);
                    if (i) return 1;
                }
                gmi_to_html(cur_entry.path(), cur_out_html);
            } else {
                if (std::filesystem::exists(cur_out)) {
                    int i = safe_remove(cur_out);
                    if (i) return 1;
                }
                try {
                    std::filesystem::copy_file(cur_entry.path(), cur_out);
                } catch (std::filesystem::filesystem_error const& e) {
                    std::cerr << e.what() << std::endl;
                    return 1;
                } catch (std::bad_alloc const& e) {
                    std::cerr << "Memory allocation error\n";
                    return 1;
                }
            }
        }
    }
    return 0;
}
