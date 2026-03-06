#include "stub_source.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace edgerelay {
namespace {
std::string now_utc_iso8601() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_utc{};
#ifdef _WIN32
    gmtime_s(&tm_utc, &t);
#else
    gmtime_r(&t, &tm_utc);
#endif
    std::ostringstream os;
    os << std::put_time(&tm_utc, "%Y-%m-%dT%H:%M:%SZ");
    return os.str();
}
} // namespace

SensorMessage StubSource::next() {
    ++sequence_;
    SensorMessage message{};
    message.sequence = sequence_;
    message.source_name = "stub.temperature";
    message.timestamp_utc = now_utc_iso8601();
    message.value = 20.0 + static_cast<double>(sequence_) * 0.5;
    return message;
}

} // namespace edgerelay
