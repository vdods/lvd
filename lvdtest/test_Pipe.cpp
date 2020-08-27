// 2020.08.25 - Victor Dods

#include "lvd/Pipe.hpp"
#include "lvd/req.hpp"
#include "lvd/Test.hpp"

LVD_REGISTER_TEST(310__Pipe, ([](){
    lvd::Pipe p;

    // Write to one end, then read from the other.

    char const input_buffer[12] = "A MAD HIPPO";
    ::write(p.descriptor(lvd::Pipe::End::WRITE), input_buffer, 12);

    char result_buffer[12];
    auto read_byte_count = ::read(p.descriptor(lvd::Pipe::End::READ), result_buffer, 12);

    LVD_REQ_EQ(errno, 0);
    LVD_REQ_EQ(read_byte_count, 12);
    LVD_REQ_EQ(input_buffer[11], '\0');
    LVD_REQ_EQ(result_buffer[11], '\0');
    LVD_REQ_EQ(std::string(input_buffer), std::string(result_buffer));
}));