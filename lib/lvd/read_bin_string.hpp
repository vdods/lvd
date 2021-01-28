// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/read.hpp"
#include "lvd/read_bin_type.hpp"
#include <string>

namespace lvd {

template <typename... Types_, auto... Params_>
struct ReadInPlace_t<std::basic_string<Types_...>,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, std::basic_string<Types_...> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        auto size = inner_enc.template read<size_t>(in);
        // TODO: Probably put a limit on the size to avoid bugs in which a huge value is read
        // causing memory to fill up.
        dest_val.resize(size);
        in.read(reinterpret_cast<char *>(&dest_val[0]), size*sizeof(dest_val[0]));
        // If called for, endian-swap the contents.
        if (machine_endianness() != enc.endianness()) {
            for (auto &c : dest_val)
                swap_byte_order_of(c);
        }
        return in;
    }
};

} // end namespace lvd
