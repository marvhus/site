#include <cassert>
#include <cstdio>
#include <fstream>
#include <functional>
#include <string_view>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>

#include <md4c/md4c.h>
#include <md4c/md4c-html.h>

std::string read_entire_file(std::string path) {
    if (!std::filesystem::exists(path)) {
        std::cout << "Attempted to read file that does not exist: " << path << std::endl;
        exit(1);
    }

    std::ifstream file(path);
    std::stringstream content;
    content << file.rdbuf();
    return content.str();
}

void handle_chunk(const MD_CHAR* data, MD_SIZE length, void* userdata) {
    std::string_view view(data, length);
    std::stringstream* html_builder = (std::stringstream*) userdata;
    *html_builder << view;
}

std::string render_html(std::filesystem::path file_path) {
    std::stringstream builder;
    std::string content = read_entire_file(file_path);
    if (int result = md_html(content.data(), content.length(), handle_chunk, &builder,
                             MD_FLAG_COLLAPSEWHITESPACE | MD_FLAG_STRIKETHROUGH,
                             MD_HTML_FLAG_DEBUG); result != 0) {
        std::cout << "Failed to parse and render markdown." << std::endl;
        exit(1);
    }
    return builder.str();
}

std::string template_extract_value(std::ifstream& page_template) {
    assert(page_template.get() == '{');

    std::stringstream value_builder;
    bool backslash = false;
    bool done = false;
    int counter = 0;

    while (!done) {
        char current = page_template.get();
        if (current == EOF) {
            std::cout << "End of file while parsing template value" << std::endl;
            exit(1);
        }

        switch (current) {
            case '\\': {
                if (backslash) {
                    backslash = false;
                    break;
                }
                backslash = true;
            } continue;
            case '{': {
                if (backslash)
                    break;

                counter++;
            } break;
            case '}': {
                if (backslash)
                    break;

                if (counter > 0) {
                    counter--;
                    continue;
                }

                done = true;
            } continue;
        }

        value_builder << current;
        backslash = false;
    }

    return value_builder.str();
}

std::string trim_whitespace(std::string str) {
    const std::string whitespace = " \t\r\n";

    const auto begin = str.find_first_not_of(whitespace);
    const auto end   = str.find_last_not_of(whitespace);
    const auto range = end - begin + 1;

    return str.substr(begin, range);
}

void build_html(std::filesystem::path source, std::filesystem::path output) {
    std::stringstream html_builder;
    std::ifstream page_template("page.template");

    bool backslash = false;
    while (page_template.peek() != EOF) {
        char current = page_template.get();
        switch (current) {
            case '\\': {
                if (backslash) {
                    backslash = false;
                    break;
                }

                backslash = true;
            } continue;
            case '{': {
                if (backslash)
                    break;

                page_template.unget();
                std::string value = template_extract_value(page_template);
                if (value.length() < 1) {
                    std::cout << "Template has empty value" << std::endl;
                    exit(1);
                }

                if (value.at(0) == '#') {
                    // @TODO: Check if the text after # is `page`  For now this works, but may be a bit weird,
                    // and will need to be updated later on if we need more internal things to be used as a value.
                    html_builder << trim_whitespace(render_html(source));
                    backslash = false;
                    continue;
                }

                std::stringstream template_value_file_path;
                template_value_file_path << "templates/" << value << ".html";
                html_builder << trim_whitespace(read_entire_file(template_value_file_path.str()));
                backslash = false;
            } continue;
        }

        html_builder << current;
        backslash = false;
    }

    std::filesystem::create_directories(output.parent_path());

    { // Write entire file
        std::ofstream file(output);
        file << html_builder.str();
        file.close();
    }
}

void build_pages_file(std::filesystem::path file_path) {
    std::stringstream result_path;
    result_path << "public/";

    std::string tmp_path = file_path.string();
    int start = 0, end = tmp_path.length();

    constexpr std::string_view PREFIX = "pages/", SUFFIX = ".md";
    if (tmp_path.starts_with(PREFIX)) {
        int length = PREFIX.length();
        start += length;
        end -= length;
    }
    if (tmp_path.ends_with(SUFFIX)) {
        int length = SUFFIX.length();
        end -= length;
    }
    std::string tmp_path2 = tmp_path.substr(start, end);

    result_path << tmp_path2
                << ".html";

    std::filesystem::path output_path(result_path.str());
    build_html(file_path, output_path);

    std::cout << file_path << " -> " << output_path << std::endl;
}

void copy_web_file(std::filesystem::path file_path) {
    std::stringstream result_path;
    result_path << "public/";

    std::string tmp_path = file_path.string();

    int prefix = 0;
    constexpr std::string_view PREFIX = "web/";
    if (tmp_path.starts_with(PREFIX)) prefix = PREFIX.length();

    std::string tmp_path2 = tmp_path.substr(prefix, tmp_path.length() - prefix);
    result_path << tmp_path2;

    std::filesystem::path output_path(result_path.str());
    std::filesystem::create_directories(output_path.parent_path());
    if (!std::filesystem::exists(output_path)) {
        std::filesystem::copy_file(file_path, output_path);
        std::cout << file_path << " -> " << output_path << std::endl;
    }
}

void walk_directory(std::filesystem::path directory, std::function<void (std::filesystem::path)> function) {
    for (const std::filesystem::directory_entry& entry: std::filesystem::directory_iterator(directory)) {
        if (entry.is_directory()) {
            walk_directory(entry.path(), function);
            continue;
        }
        function(entry.path());
    }
}

int main(void) {
    std::cout << "=== Building pages/ ===" << std::endl;
    walk_directory("pages/", build_pages_file); // @TODO: read from config file

    std::cout << "=== Copying web/ ======" << std::endl;
    walk_directory("web/", copy_web_file);
}
