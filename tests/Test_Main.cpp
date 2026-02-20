#include <gtest/gtest.h>
#include "Log.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // 初始化日志和其异步线程池
    Nexus::Log::init();
    Nexus::Log::info("NexusEngine Test Suite Starting...");
    
    int result = RUN_ALL_TESTS();
    
    return result;
}
