#include <gtest/gtest.h>

#include <iostream>
#include <exception>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "Running unit tests" << std::endl;
	
    int retval = 0;
	try {
		retval = RUN_ALL_TESTS();
	} catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
    return retval;
}
