#include <iostream>
#include <string_view>

namespace edgerelay {

// Keep startup output intentionally simple in Phase 0 so interviewers can verify
// the toolchain quickly before we add MQTT/SQLite/OPC UA dependencies.
void print_boot_banner(std::string_view mode) {
    std::cout << "[EdgeRelay] boot mode=" << mode << '\n';
}

} // namespace edgerelay

int main() {
    edgerelay::print_boot_banner("phase0-skeleton");
    return 0;
}
