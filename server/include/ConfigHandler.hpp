#pragma once

#include "common.h"
#include <string>

namespace ConfigHandler {
    Config read();
    void write(const Config &cfg, const CFDConfig &cfdCfg);
}
