#pragma once

#include "runtime_config.h"

#include <string>

namespace edgerelay {

void load_config_file(const std::string& path, RuntimeConfig& config);

} // namespace edgerelay
