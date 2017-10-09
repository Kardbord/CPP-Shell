#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

// Parses @s into chunks based on @delimiter and inserts those chunks in @tokens
// Inspiration taken from a few notable stackoverflow answers cited below
//  - https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
//  - https://stackoverflow.com/questions/236129/most-elegant-way-to-split-a-string
template<typename ContainerT = std::vector<std::string>>
void parse_string(std::string const &s, std::string const &delimiter, ContainerT &tokens) {
    size_t start_pos = 0;
    size_t found_pos = 0;
    while ((found_pos = s.find(delimiter, start_pos)) != std::string::npos) {
        tokens.push_back(s.substr(start_pos, found_pos - start_pos));
        start_pos = found_pos + delimiter.length();
    }
    tokens.push_back(s.substr(start_pos, s.length() - start_pos));
}

#endif
