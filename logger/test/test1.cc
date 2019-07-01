#include "../logstream.h"
#include <iostream>

int main() {
    yxalp::LogStream ls;
    ls << 3.1415926;
    std::cout << ls.get_buffer().current_length() << std::endl;
    std::cout << ls.get_buffer().get_data() << std::endl;
    ls << 10 << 20 << "a good day! " << 3.1415926;
    std::cout << ls.get_buffer().get_data() << std::endl;
    std::cout << ls.get_buffer().available() << std::endl;
    std::cout << ls.get_buffer().current_length() << std::endl;
    return 0;
}