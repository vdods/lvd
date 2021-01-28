// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/read.hpp"
#include "lvd/Type_t.hpp"

namespace lvd {

template <typename T_, auto... Params_>
struct ReadInPlace_t<Type_t<T_>,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, Type_t<T_> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED) {
            auto s = enc.with_demoted_type_encoding().template read<std::string>(in);
            if (s != "type") // This is somewhat informally defined and may need to change later.
                throw std::runtime_error("type mismatch; expected \"type\" but got " + literal_of(s));
        }

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        // This is the string that encodes T_
        auto s = inner_enc.template read<std::string>(in);
        // This just asserts that the type matches.
        if (s != TypeString_t<T_>::get())
            throw std::runtime_error("type mismatch; expected " + literal_of(TypeString_t<T_>::get()) + " but got " + literal_of(s));
        return in;
    }
};

} // end namespace lvd
