#include <gtest/gtest.h>
#include <sys/resource.h>
#include <cstdlib>

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);

        // Disable core dumps
        struct rlimit rl;
        rl.rlim_cur = 0;
        rl.rlim_max = 0;
        if (setrlimit(RLIMIT_CORE, &rl) != 0) {
                std::cerr << "Failed to disable core dumps\n";
                return 1;
        }

        return RUN_ALL_TESTS();
}
