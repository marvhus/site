#include <string_view>
#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

#include <md4c/md4c.h>
#include <md4c/md4c-html.h>

std::string_view read_entire_file(const char* path) {
    FILE* fp = fopen(path, "r");
    if (fp == nullptr) {
        std::cout << "Unable to open file: " << path << std::endl;
        exit(1);
    }

    if (int status = fseek(fp, 0, SEEK_END); status != 0) {
        std::cout << "Unable to find end of file for: " << path << std::endl;
        exit(1);
    }

    long length = ftell(fp);

    rewind(fp);

    char* data = (char*)malloc(length);
    if (size_t bytes_read = fread(data, 1, length, fp); bytes_read != (size_t)length) {
        std::cout << "Unable to read '" << path << "'.  Read " << bytes_read << " but expected " << length << std::endl;
        exit(1);
    }

    if (int result = fclose(fp); result != 0) {
        std::cout << "Unable to close file: " << path << std::endl;
        exit(1);
    }

    std::string_view sv(data, length);
    return sv;
}

void write_entire_file(const char* path, std::string_view content) {
    FILE* fp = fopen(path, "w");
    if (fp == nullptr) {
        std::cout << "Unable to open file: " << path << std::endl;
        exit(1);
    }

    size_t content_length = content.length();
    if (size_t bytes_written = fwrite(content.data(), 1, content_length, fp); bytes_written != content_length) {
        std::cout << "Unable to write to file: " << path << std::endl;
        exit(1);
    }

    if (int result = fclose(fp); result != 0) {
        std::cout << "Unable to close file: " << path << std::endl;
        exit(1);
    }
}

std::string_view strip_pages_path(std::string_view path) {
    constexpr std::string_view prefix_path("pages/");
    constexpr size_t prefix_length = prefix_path.length();

    size_t path_length = path.length();
    {
        bool has_prefix = path.starts_with(prefix_path);
        bool path_after_prefix = path_length > prefix_length;
        if (!has_prefix || !path_after_prefix) {
            std::cout << "'" << path << "' is not a valid pages path" << std::endl;
            exit(1);
        }
    }

    return path.substr(prefix_length, path_length - prefix_length);
}

void handle_chunk(const MD_CHAR* data, MD_SIZE length, void* userdata) {
    std::string_view view(data, length);
    std::stringstream* html_builder = (std::stringstream*) userdata;
    *html_builder << view;
}

void build_html(const char* path) {
    std::string_view content = read_entire_file(path);

    std::stringstream html_builder;
    html_builder << "<!DOCTYPE html>\n<html>\n"
                 << read_entire_file("page-head.html")
                 << "<body>\n"
                 << read_entire_file("page-navbar.html")
                 << "<main>\n";

    if (int result = md_html(content.data(), content.length(), handle_chunk, &html_builder,
                             MD_FLAG_COLLAPSEWHITESPACE | MD_FLAG_STRIKETHROUGH,
                             MD_HTML_FLAG_DEBUG); result != 0) {
        std::cout << "Failed to parse and render markdown." << std::endl;
        exit(1);
    }

    html_builder << "</main>\n"
                 << read_entire_file("page-footer.html")
                 << "</body>\n</html>";

    write_entire_file("web/index.html", html_builder.str());
    if (void* data = (void*)content.data(); data) free(data);
}

int main(void) {
    build_html("pages/index.md");
}
