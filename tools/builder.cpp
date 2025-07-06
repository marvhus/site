#include <string_view>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

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

    std::string_view sv(data, length);
    return sv;
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

int main(void) {
    const char* path = "pages/index.md";
    std::string_view path_sv(path);

    std::cout << "Content: " << read_entire_file(path) << std::endl;
    std::cout << "Path: " << strip_pages_path(path_sv) << std::endl;
}
