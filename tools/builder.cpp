#include <fstream>
#include <functional>
#include <string_view>
#include <iostream>
#include <sstream>
#include <filesystem>

#include <md4c/md4c.h>
#include <md4c/md4c-html.h>

std::string read_entire_file(std::string path) {
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

void build_html(std::filesystem::path source, std::filesystem::path output) {

    std::stringstream html_builder;
    html_builder << "<!DOCTYPE html>\n<html>\n"
                 << read_entire_file("templates/head.html") // @TODO: read from config file
                 << "<body>\n"
                 << read_entire_file("templates/navbar.html") // @TODO: read from config file
                 << "<main>\n";

    { // Read, Parse, and Render MD file to HTML
        std::string content = read_entire_file(source);
        if (int result = md_html(content.data(), content.length(), handle_chunk, &html_builder,
                                 MD_FLAG_COLLAPSEWHITESPACE | MD_FLAG_STRIKETHROUGH,
                                 MD_HTML_FLAG_DEBUG); result != 0) {
            std::cout << "Failed to parse and render markdown." << std::endl;
            exit(1);
        }
    }

    html_builder << "</main>\n"
                 << read_entire_file("templates/footer.html") // @TODO: read from config file
                 << "</body>\n</html>";

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
    if (!std::filesystem::exists(output_path))
        std::filesystem::copy_file(file_path, output_path);

    std::cout << file_path << " -> " << output_path << std::endl;
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
