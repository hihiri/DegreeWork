#pragma once

#include "common.h"
#include <string>

namespace ConfigHandler {
    Config read(const std::string &path);
    void write(const std::string &path, const Config &cfg, const CFDConfig &cfdCfg);
}
