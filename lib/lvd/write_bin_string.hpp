// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of.hpp"
#include "lvd/write.hpp"
#include <string>

namespace lvd {

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::basic_string<Types_...>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, std::basic_string<Types_...> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();

        out << inner_enc.out(src_val.size()); // TODO: Limit to uint32_t
        if constexpr (sizeof(src_val[0]) == 1) {
            // No need to byte-order-swap in this case.
            out.write(reinterpret_cast<char const *>(&src_val[0]), src_val.size()*sizeof(src_val[0]));
        } else if (machine_endianness() == enc.endianness()) {
            // No need to byte-order-swap in this case.
            out.write(reinterpret_cast<char const *>(&src_val[0]), src_val.size()*sizeof(src_val[0]));
        } else {
            static_assert(sizeof(src_val[0]) > 1);
            // Have to write each element individually, and this will take care of the byte-swap.
            for (auto const &c : src_val)
                out << inner_enc.out(c);
        }
        return out;
    }
};

} // end namespace lvd
