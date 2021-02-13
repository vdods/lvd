// 2021.02.09 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/literal.hpp"
#include "lvd/read_bin_sst.hpp"
#include "lvd/read_bin_string.hpp"
#include "lvd/read_bin_tuple.hpp"
#include "lvd/req.hpp"
#include "lvd/sst/SV_t.hpp"
#include "lvd/test.hpp"
#include "lvd/type.hpp"
#include "lvd/type_string_of.hpp"
#include "lvd/write_bin_sst.hpp"
#include "lvd/write_bin_string.hpp"
#include "lvd/write_bin_tuple.hpp"
#include "lvd/write_text_sst.hpp"
#include "lvd/write_text_string.hpp"
#include "lvd/write_text_tuple.hpp"
#include "NonEmpty.hpp"
#include "print.hpp"
#include <sstream>
#include <tuple>

namespace lvd {

struct Name_s : virtual NonEmpty_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("Name")
};
struct PhoneNumber_s : virtual sst::Base_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("PhoneNumber")
    // Should have the format 1AAABBBNNNN in decimal.
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(
        return cv/C(10000000000ull) == C(1) && cv/C(100000000000ull) == C(0);
    )
};
struct BankBalance_s : virtual sst::Base_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("BankBalance")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv >= C(0);)
};

template <typename C_> using Name_t = sst::SV_t<Name_s,C_>;
template <typename C_> using PhoneNumber_t = sst::SV_t<PhoneNumber_s,C_>;
template <typename C_> using BankBalance_t = sst::SV_t<BankBalance_s,C_>;

using Contact_CType = std::tuple<Name_t<std::string>,PhoneNumber_t<uint64_t>,BankBalance_t<double>>;

struct Contact : public Contact_CType {
//     using Contact_CType::Contact_CType;
    Contact (Contact_CType const &c) : Contact_CType(c) { }
    Contact (Contact_CType &&c) : Contact_CType(std::move(c)) { }
    template <typename... Args_>
    Contact (Args_&&... args) : Contact_CType(std::forward<Args_>(args)...) { }

    decltype(auto) name () const { return std::get<0>(*this); }
    decltype(auto) name () { return std::get<0>(*this); }
    decltype(auto) phone () const { return std::get<1>(*this); }
    decltype(auto) phone () { return std::get<1>(*this); }
    decltype(auto) bank () const { return std::get<2>(*this); }
    decltype(auto) bank () { return std::get<2>(*this); }

    Contact_CType const &as_tuple () const { return static_cast<Contact_CType const &>(*this); }
    Contact_CType &as_tuple () { return static_cast<Contact_CType &>(*this); }
};

inline std::ostream &operator<< (std::ostream &out, Contact const &c) {
    return out << "Contact(" << c.as_tuple() << ')';
}

} // end namespace lvd

LVD_DEFINE_TYPE_STRING_EXPLICIT(lvd::Contact, "Contact")

namespace lvd {

// TODO: Could make some sort of type override
template <typename Encoding_>
struct ReadInPlace_t<Contact,Encoding_> : public ReadInPlace_t<Contact_CType,Encoding_> { };

template <typename Encoding_>
struct ReadValue_t<Contact,Encoding_> : public ReadValue_t<Contact_CType,Encoding_> { };

template <typename Encoding_>
struct WriteValue_t<Contact,Encoding_> : public WriteValue_t<Contact_CType,Encoding_> { };

template <typename T_, typename Encoding_>
void test_serialize_SV_t_roundtrip (req::Context &req_context, T_ const &expected_value, Encoding_ const &enc) {
    std::ostringstream out;
    out << enc.out(expected_value);

    req_context.log() << Log::trc() << LVD_REFLECT(literal_of(out.str())) << '\n';

    std::istringstream in(out.str());
    auto actual_value = enc.template read<T_>(in);

    LVD_TEST_REQ_EQ(actual_value, expected_value);
}

template <typename T_>
void test_serialize_SV_t_roundtrip_all_encodings (req::Context &req_context, T_ const &expected_value) {
    test_serialize_SV_t_roundtrip(req_context, expected_value, lvd::bin_big_e);
    test_serialize_SV_t_roundtrip(req_context, expected_value, lvd::bin_lil_e);
    test_serialize_SV_t_roundtrip(req_context, expected_value, lvd::bin_machine_e);
    test_serialize_SV_t_roundtrip(req_context, expected_value, lvd::tbin_big_e);
    test_serialize_SV_t_roundtrip(req_context, expected_value, lvd::tbin_lil_e);
    test_serialize_SV_t_roundtrip(req_context, expected_value, lvd::tbin_machine_e);
}

LVD_TEST_BEGIN(007__sst__03__read_write__00__bin__00)
    // Just verify that expected constructors work for Contact
    auto const c1 = Contact{Name_t<std::string>{"Blobby Blue"}, PhoneNumber_t<uint64_t>{19001112222}, BankBalance_t<double>{123.0}};
    Contact c2{c1};
    Contact c3{std::move(c2)};
    auto c4 = Contact{Name_t<std::string>{"Xyz"}, PhoneNumber_t<uint64_t>{18005558888}, BankBalance_t<double>{1000000.0}};
    c3 = c1;
    c3 = std::move(c4);

    // Uncommenting this should cause compile error.
//     test_serialize_SV_t_roundtrip_all_encodings(req_context, Name_t<std::string>{});

    test_serialize_SV_t_roundtrip_all_encodings(req_context, Name_t<std::string>{"Jabby Joe"});

    test_serialize_SV_t_roundtrip_all_encodings(req_context, PhoneNumber_t<uint64_t>{18005554040ull});
    test_serialize_SV_t_roundtrip_all_encodings(req_context, PhoneNumber_t<uint64_t>{18005554040});

    test_serialize_SV_t_roundtrip_all_encodings(req_context, BankBalance_t<double>{400.0});
    test_serialize_SV_t_roundtrip_all_encodings(req_context, BankBalance_t<double>{0.0});

    test_serialize_SV_t_roundtrip_all_encodings(req_context, BankBalance_t<uint32_t>{400.0});
    test_serialize_SV_t_roundtrip_all_encodings(req_context, BankBalance_t<uint32_t>{0.0});

    auto contact = Contact{
        Name_t<std::string>{"Blobby Blue"},
        PhoneNumber_t<uint64_t>{19001112222},
        BankBalance_t<double>{123.0}
    };
    test_serialize_SV_t_roundtrip_all_encodings(req_context, contact);
LVD_TEST_END

LVD_TEST_BEGIN(007__sst__03__read_write__01__text__00)
    auto name = Name_t<std::string>{"Jabby Joe"};
    auto phone_number = PhoneNumber_t<uint64_t>{18005554040ull};
    auto bank_balance = BankBalance_t<double>{400.0};

    test_log << Log::trc() << LVD_REFLECT(LVD_FMT(lvd::txt.out(name))) << '\n';
    test_log << Log::trc() << LVD_REFLECT(LVD_FMT(lvd::lit.out(name))) << '\n';

    test_log << Log::trc() << LVD_REFLECT(LVD_FMT(lvd::txt.out(phone_number))) << '\n';
    test_log << Log::trc() << LVD_REFLECT(LVD_FMT(lvd::lit.out(phone_number))) << '\n';

    test_log << Log::trc() << LVD_REFLECT(LVD_FMT(lvd::txt.out(bank_balance))) << '\n';
    test_log << Log::trc() << LVD_REFLECT(LVD_FMT(lvd::lit.out(bank_balance))) << '\n';

    auto contact = Contact{
        Name_t<std::string>{"Blobby Blue"},
        PhoneNumber_t<uint64_t>{19001112222},
        BankBalance_t<double>{123.0}
    };

    test_log << Log::trc() << LVD_REFLECT(LVD_FMT(lvd::txt.out(contact))) << '\n';
    test_log << Log::trc() << LVD_REFLECT(LVD_FMT(lvd::lit.out(contact))) << '\n';

LVD_TEST_END

} // end namespace lvd
