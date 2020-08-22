// 2020.08.22 - Victor Dods

#pragma once

#include "lvd/core.hpp"
// #include <errno.h>
// #include <fcntl.h>
// #include <sys/stat.h>
// #include <unistd.h>

namespace lvd {

enum class Fd : int {
    STDIN = 0,
    STDOUT = 1,
    STDERR = 2
};

class Pipe {
public:

    enum class End : int {
        READ = 0,
        WRITE = 1
    };

    Pipe () {
        if (::pipe(m_fds) == -1)
            throw std::runtime_error(std::string("pipe() returned with error: ") + strerror(errno));
    }
    Pipe (Pipe const &) = delete;
    Pipe (Pipe &&other) {
        m_fds[int(End::READ)] = other.m_fds[int(End::READ)];
        m_fds[int(End::WRITE)] = other.m_fds[int(End::WRITE)];
        other.clear();
    }
    ~Pipe () {
        // Ignore errors
        close(End::READ);
        close(End::WRITE);
    }

    int descriptor (End end) const & {
        return m_fds[int(end)];
    }
    // If for some reason you want to clear the file descriptors so that they aren't closed
    // when this object is destroyed.
    void clear () {
        m_fds[int(End::READ)] = -1;
        m_fds[int(End::WRITE)] = -1;
    }
    // Technically target_fd could be any int, but realistically only the standard fds will be used.
    void dup2 (End end, Fd target_fd) const {
        if (::dup2(m_fds[int(end)], int(target_fd)) == -1)
            throw std::runtime_error(std::string("dup2() returned with error: ") + strerror(errno));
    }
    void close (End end) {
        if (m_fds[int(end)] >= 0) {
            ::close(m_fds[int(end)]);
            m_fds[int(end)] = -1;
        }
    }

private:

    int m_fds[2];
};

} // end namespace lvd
