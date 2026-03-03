#include "pipeline.h"

#include "console_sink.h"
#include "stub_source.h"

#include <chrono>
#include <thread>

namespace edgerelay {

void run_pipeline(const std::size_t count) {
    StubSource source;
    ConsoleSink sink;

    for (std::size_t i = 0; i < count; ++i) {
        const auto message = source.next();
        sink.publish(message);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

} // namespace edgerelay
