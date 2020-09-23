// 2020.08.25 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/Pipe.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

LVD_TEST_BEGIN(310__Pipe)
    lvd::Pipe p;

    // Write to one end, then read from the other.

    char const input_buffer[12] = "A MAD HIPPO";
    ::write(p.descriptor(lvd::Pipe::End::WRITE), input_buffer, 12);

    char result_buffer[12];
    auto read_byte_count = ::read(p.descriptor(lvd::Pipe::End::READ), result_buffer, 12);

    LVD_TEST_REQ_EQ(errno, 0, "call to ::read failed");
    LVD_TEST_REQ_EQ(read_byte_count, 12, "expected to read 12 bytes");
    LVD_TEST_REQ_EQ(input_buffer[11], '\0');
    LVD_TEST_REQ_EQ(result_buffer[11], '\0');
    LVD_TEST_REQ_EQ(std::string(input_buffer), std::string(result_buffer));
LVD_TEST_END
