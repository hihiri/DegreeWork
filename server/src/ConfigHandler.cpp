#include "ConfigHandler.hpp"

#include <cctype>
#include <fstream>
#include <iterator>

namespace {
int parseIntAfter(const std::string &s, size_t pos)
{
    size_t colon = s.find(':', pos);
    if(colon == std::string::npos)
        return 0;

    size_t i = colon + 1;
    while(i < s.size() && (s[i] == ' ' || s[i] == '"'))
        ++i;

    std::string num;
    while(i < s.size() && (s[i] == '-' || std::isdigit((unsigned char)s[i]))) {
        num.push_back(s[i]);
        ++i;
    }

    if(num.empty())
        return 0;

    return std::stoi(num);
}
}

Config ConfigHandler::read(const std::string &path)
{
    Config c;
    std::ifstream f(path);
    if(!f)
        return c;

    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    auto parseIntField = [&](const std::string &key, int &out){
        size_t pos = s.find(key);
        if(pos != std::string::npos)
            out = parseIntAfter(s, pos);
    };

    auto parseBoolField = [&](const std::string &key, bool &out){
        size_t pos = s.find(key);
        if(pos != std::string::npos)
           out = (parseIntAfter(s, pos) != 0);
    };

    parseIntField("\"port\"", c.port);
    parseBoolField("\"log\"", c.log);
    return c;
}

void ConfigHandler::write(const std::string &path, const Config &cfg, const CFDConfig &cfdCfg)
{
    std::ofstream f(path, std::ios::trunc);
    f << "{\n";

    auto writeField = [&](const char *name, const std::string &value, bool comma, int indent = 2){
        f << std::string(indent, ' ') << "\"" << name << "\": " << value;
        if(comma)
            f << ",";
        f << "\n";
    };

    writeField("log", (cfg.log ? "true" : "false"), true);
    writeField("port", std::to_string(cfg.port), true);

    f << "  \"cfdConfig\": {\n";
    writeField("width", std::to_string(cfdCfg.width), true, 4);
    writeField("height", std::to_string(cfdCfg.height), false, 4);
    f << "  }\n";
    f << "}\n";
}
