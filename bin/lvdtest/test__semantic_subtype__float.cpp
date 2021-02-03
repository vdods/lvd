// 2021.01.28 - Copyright Victor Dods - Licensed under Apache 2.0

#include <cmath>
#include "lvd/IndexedTuple_t.hpp"
#include "lvd/req.hpp"
#include "lvd/semantic_subtype.hpp"
#include "lvd/test.hpp"
#include "lvd/type.hpp"
#include "lvd/type_string_of.hpp"
#include <limits>

namespace lvd {

// Extended real line (this includes positive and negative infinity) with not-a-number value(s).
struct NaNExtReal_s : virtual Base_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("NaNExtReal")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return true;)
};

// TODO: Create NaNOrZero_s, which would be the result of Zero_s * ExtXYZ_s

struct NaN_s : virtual NaNExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("NaN")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return std::isnan(cv);)
};

// NOTE: This sort of thing could go in semantic_subtype.hpp itself.
// https://en.wikipedia.org/wiki/Extended_real_number_line -- this includes positive and negative infinity, but not NaN.
struct ExtReal_s : virtual NaNExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("ExtReal")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return !std::isnan(cv);)
};

struct Infinite_s : virtual ExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("Infinite")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return !std::isfinite(cv);)
};

struct ExtNonZero_s : virtual ExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("ExtNonZero")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv != C(0);)
};

struct ExtNonNeg_s : virtual ExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("ExtNonNeg")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv >= C(0);)
};

struct ExtNonPos_s : virtual ExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("ExtNonPos")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv <= C(0);)
};

struct ExtPos_s : virtual ExtNonNeg_s, virtual ExtNonZero_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("ExtPos")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv > C(0);)
};

struct ExtNeg_s : virtual ExtNonPos_s, virtual ExtNonZero_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("ExtNeg")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv < C(0);)
};

struct Infinity_s : virtual ExtPos_s, virtual Infinite_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("Infinity")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv == std::numeric_limits<C>::infinity();)
};

struct NegInfinity_s : virtual ExtNeg_s, virtual Infinite_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("NegInfinity")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv == -std::numeric_limits<C>::infinity();)
};

// Represents real numbers.  Relative to ExtReal_s, this is the subset of finite values.
struct Real_s : virtual ExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("Real")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return std::isfinite(cv);)
};

struct NonZero_s : virtual Real_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("NonZero")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv != C(0) && std::isfinite(cv);)
};

struct NonPos_s : virtual Real_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("NonPos")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv <= C(0) && std::isfinite(cv);)
};

struct NonNeg_s : virtual Real_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("NonNeg")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv >= C(0) && std::isfinite(cv);)
};

struct Pos_s : virtual NonNeg_s, virtual NonZero_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("Pos")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv > C(0) && std::isfinite(cv);)
};

struct Neg_s : virtual NonPos_s, virtual NonZero_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("Neg")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv < C(0) && std::isfinite(cv);)
};

struct Zero_s : virtual NonNeg_s, virtual NonPos_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("Zero")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv == C(0);)
};

struct One_s : virtual Pos_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("One")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv == C(1);)
};

struct NegOne_s : virtual Neg_s {
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING("NegOne")
    LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(return cv == C(-1);)
};

//
// Catch-alls
//

LVD_DEFINE_SEMANTIC_BIN_OP(+, add, NaNExtReal_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, NaNExtReal_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(*, mul, NaNExtReal_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NaNExtReal_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)

//
// It's easy enough to define the unary operators
//

LVD_DEFINE_SEMANTIC_UN_OP(+, pos, NaN_s, NaN_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_UN_OP(+, pos, ExtReal_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, Infinite_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, ExtNonZero_s, ExtNonZero_s , ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, ExtNonNeg_s, ExtNonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, ExtNonPos_s, ExtNonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, ExtPos_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, ExtNeg_s, ExtNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, Infinity_s, Infinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, NegInfinity_s, NegInfinity_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_UN_OP(+, pos, Real_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, NonZero_s, NonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, NonNeg_s, NonNeg_s , ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, NonPos_s, NonPos_s , ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, Pos_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, Neg_s, Neg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, Zero_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, One_s, One_s , ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(+, pos, NegOne_s, NegOne_s , ALLOW__NO_CHECK)


LVD_DEFINE_SEMANTIC_UN_OP(-, neg, NaN_s, NaN_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_UN_OP(-, neg, ExtReal_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, Infinite_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, ExtNonZero_s, ExtNonZero_s , ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, ExtNonNeg_s, ExtNonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, ExtNonPos_s, ExtNonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, ExtPos_s, ExtNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, ExtNeg_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, Infinity_s, NegInfinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, NegInfinity_s, Infinity_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_UN_OP(-, neg, Real_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, NonZero_s, NonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, NonNeg_s, NonPos_s , ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, NonPos_s, NonNeg_s , ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, Pos_s, Neg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, Neg_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, Zero_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, One_s, NegOne_s , ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_UN_OP(-, neg, NegOne_s, One_s , ALLOW__NO_CHECK)



//
// Operations with NaN result in NaN
//

LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, NaNExtReal_s, NaN_s, NaN_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(-, sub, NaNExtReal_s, NaN_s, NaN_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, NaNExtReal_s, NaN_s, NaN_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(/, div, NaNExtReal_s, NaN_s, NaN_s, ALLOW__NO_CHECK)

//
// Adding Zero_s doesn't change the semantic class.
//

LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, ExtReal_s, Zero_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, Infinite_s, Zero_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, ExtNonZero_s, Zero_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, ExtNonNeg_s, Zero_s, ExtNonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, ExtNonPos_s, Zero_s, ExtNonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, ExtPos_s, Zero_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, ExtNeg_s, Zero_s, ExtNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, Infinity_s, Zero_s, Infinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, NegInfinity_s, Zero_s, NegInfinity_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, Real_s, Zero_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, NonZero_s, Zero_s, NonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, NonNeg_s, Zero_s, NonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, NonPos_s, Zero_s, NonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, Pos_s, Zero_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, Neg_s, Zero_s, Neg_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(+, add, Zero_s, Zero_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, One_s, Zero_s, One_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, NegOne_s, Zero_s, NegOne_s, ALLOW__NO_CHECK)

//
// Subtracting Zero_s doesn't change the semantic class.
//

LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, ExtReal_s, Zero_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Infinite_s, Zero_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, ExtNonZero_s, Zero_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, ExtNonNeg_s, Zero_s, ExtNonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, ExtNonPos_s, Zero_s, ExtNonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, ExtPos_s, Zero_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, ExtNeg_s, Zero_s, ExtNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Infinity_s, Zero_s, Infinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, NegInfinity_s, Zero_s, NegInfinity_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Real_s, Zero_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, NonZero_s, Zero_s, NonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, NonNeg_s, Zero_s, NonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, NonPos_s, Zero_s, NonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Pos_s, Zero_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Neg_s, Zero_s, Neg_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, Zero_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, One_s, Zero_s, One_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, NegOne_s, Zero_s, NegOne_s, ALLOW__NO_CHECK)

//
// Subtracting from Zero_s negates the semantic class.
//

LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, ExtReal_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, Infinite_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, ExtNonZero_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, ExtNonNeg_s, ExtNonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, ExtNonPos_s, ExtNonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, ExtPos_s, ExtNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, ExtNeg_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, Infinity_s, NegInfinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, NegInfinity_s, Infinity_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, Real_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, NonZero_s, NonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, NonNeg_s, NonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, NonPos_s, NonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, Pos_s, Neg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, Neg_s, Pos_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, One_s, NegOne_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Zero_s, NegOne_s, One_s, ALLOW__NO_CHECK)

//
// Multiplying by Zero_s produces Zero_s in most cases.  The semantic class hierarchy can be
// used to advantage here.  Note that inf*0 = NaN, so all the ext and infinite classes can't
// have anything more specific without creating NaNOrZero_s.
//

// This one is sufficient to catch all cases except for Zero_s * Zero_s.
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Real_s, Zero_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(*, mul, Zero_s, Zero_s, Zero_s, ALLOW__NO_CHECK)

//
// Zero_s divided by anything finite and nonzero is Zero_s, although 0/0 is NaN.
//

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK) // Should be a catch-all

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, Infinite_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, ExtNonZero_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, Infinity_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, NegInfinity_s, Zero_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, NonZero_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, Pos_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, Neg_s, Zero_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, Zero_s, NaN_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, One_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Zero_s, NegOne_s, Zero_s, ALLOW__NO_CHECK)

//
// Dividing by zero is a big no-no.
//

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNonZero_s, Zero_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtPos_s, Zero_s, Infinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNeg_s, Zero_s, NegInfinity_s, ALLOW__NO_CHECK)

//
// Adding by one changes some semantic classes.
//

LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, ExtReal_s, One_s, ExtReal_s, ALLOW__NO_CHECK) // Catch-all
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, Infinite_s, One_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, ExtNonNeg_s, One_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, ExtPos_s, One_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, Infinity_s, One_s, Infinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, NegInfinity_s, One_s, NegInfinity_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, Real_s, One_s, Real_s, ALLOW__NO_CHECK) // Catch-all
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, NonNeg_s, One_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, NegOne_s, One_s, Zero_s, ALLOW__NO_CHECK)

// TODO: sub

//
// Multiplying by One_s leaves all classes the same.  TODO: Could just make a templatized function that does
// this, since this sort of enumeration won't work in general.
//

LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtReal_s, One_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Infinite_s, One_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtNonZero_s, One_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtNonNeg_s, One_s, ExtNonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtNonPos_s, One_s, ExtNonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtPos_s, One_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtNeg_s, One_s, ExtNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Infinity_s, One_s, Infinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, NegInfinity_s, One_s, NegInfinity_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Real_s, One_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, NonZero_s, One_s, NonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, NonNeg_s, One_s, NonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, NonPos_s, One_s, NonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Pos_s, One_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Neg_s, One_s, Neg_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(*, mul, One_s, One_s, One_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, NegOne_s, One_s, NegOne_s, ALLOW__NO_CHECK)

//
// Multiplying by NegOne_s negates classes' signs.  TODO: Could just make a templatized function that does
// this, since this sort of enumeration won't work in general.
//

LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtReal_s, NegOne_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Infinite_s, NegOne_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtNonZero_s, NegOne_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtNonNeg_s, NegOne_s, ExtNonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtNonPos_s, NegOne_s, ExtNonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtPos_s, NegOne_s, ExtNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtNeg_s, NegOne_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Infinity_s, NegOne_s, NegInfinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, NegInfinity_s, NegOne_s, Infinity_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Real_s, NegOne_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, NonZero_s, NegOne_s, NonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, NonNeg_s, NegOne_s, NonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, NonPos_s, NegOne_s, NonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Pos_s, NegOne_s, Neg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Neg_s, NegOne_s, Pos_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(*, mul, NegOne_s, NegOne_s, One_s, ALLOW__NO_CHECK)

//
// Dividing by One_s leaves all classes the same.  TODO: Could just make a templatized function that does
// this, since this sort of enumeration won't work in general.
//

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtReal_s, One_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Infinite_s, One_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNonZero_s, One_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNonNeg_s, One_s, ExtNonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNonPos_s, One_s, ExtNonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtPos_s, One_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNeg_s, One_s, ExtNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Infinity_s, One_s, Infinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NegInfinity_s, One_s, NegInfinity_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Real_s, One_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NonZero_s, One_s, NonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NonNeg_s, One_s, NonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NonPos_s, One_s, NonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Pos_s, One_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Neg_s, One_s, Neg_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, One_s, One_s, One_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NegOne_s, One_s, NegOne_s, ALLOW__NO_CHECK)

//
// Dividing by NegOne_s negates some classes.  TODO: Could just make a templatized function that does
// this, since this sort of enumeration won't work in general.
//

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtReal_s, NegOne_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Infinite_s, NegOne_s, Infinite_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNonZero_s, NegOne_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNonNeg_s, NegOne_s, ExtNonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNonPos_s, NegOne_s, ExtNonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtPos_s, NegOne_s, ExtNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNeg_s, NegOne_s, ExtPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Infinity_s, NegOne_s, NegInfinity_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NegInfinity_s, NegOne_s, Infinity_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Real_s, NegOne_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NonZero_s, NegOne_s, NonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NonNeg_s, NegOne_s, NonPos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NonPos_s, NegOne_s, NonNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Pos_s, NegOne_s, Neg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Neg_s, NegOne_s, Pos_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, NegOne_s, NegOne_s, One_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, One_s, NegOne_s, NegOne_s, ALLOW__NO_CHECK)

// TODO: One_s divided by stuff
// TODO: NegOne_s divided by stuff

//
// Various signed operations
//

// TODO: ExtNonNeg_s, ExtNonPos_s, NonNeg_s, NonPos_s

LVD_DEFINE_SEMANTIC_BIN_OP(+, add, ExtNeg_s, ExtNeg_s, ExtNeg_s, ALLOW__NO_CHECK) // Overflow goes infinite
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, ExtNeg_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK) // Sum of inf and -inf is NaN
LVD_DEFINE_SEMANTIC_BIN_OP(+, add, ExtPos_s, ExtPos_s, ExtPos_s, ALLOW__NO_CHECK) // Overflow goes infinite

LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, ExtNeg_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK) // Difference of infinities is NaN
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, ExtNeg_s, ExtPos_s, ExtNeg_s, ALLOW__NO_CHECK) // Overflow goes infinite
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, ExtPos_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK) // Difference of infinities is NaN
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, ExtPos_s, ExtNeg_s, ExtPos_s, ALLOW__NO_CHECK) // Overflow goes infinite

LVD_DEFINE_SEMANTIC_BIN_OP(*, mul, ExtNeg_s, ExtNeg_s, ExtNonNeg_s, ALLOW__NO_CHECK) // Product of near-zero values can be 0.
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, ExtNeg_s, ExtPos_s, ExtNonPos_s, ALLOW__NO_CHECK) // Product of near-zero values can be 0.
LVD_DEFINE_SEMANTIC_BIN_OP(*, mul, ExtPos_s, ExtPos_s, ExtNonNeg_s, ALLOW__NO_CHECK) // Product of near-zero values can be 0.

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNeg_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK) // Dividing infinities gives NaN
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtNeg_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK) // Dividing infinities gives NaN
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtPos_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK) // Dividing infinities gives NaN
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, ExtPos_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK) // Dividing infinities gives NaN


LVD_DEFINE_SEMANTIC_BIN_OP(+, add, Neg_s, Neg_s, ExtNeg_s, ALLOW__NO_CHECK) // -big+-big can be -inf
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(+, add, Neg_s, Pos_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(+, add, Pos_s, Pos_s, ExtPos_s, ALLOW__NO_CHECK) // big+big can be inf

LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Neg_s, Neg_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Neg_s, Pos_s, ExtNeg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Pos_s, Pos_s, Real_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(-, sub, Pos_s, Neg_s, ExtPos_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_BIN_OP(*, mul, Neg_s, Neg_s, ExtNonNeg_s, ALLOW__NO_CHECK) // Subnormal*subnormal can be 0, -big*-big can be inf.
LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(*, mul, Neg_s, Pos_s, ExtNonPos_s, ALLOW__NO_CHECK) // Subnormal*subnormal can be 0, -big*big can be -inf.
LVD_DEFINE_SEMANTIC_BIN_OP(*, mul, Pos_s, Pos_s, ExtNonNeg_s, ALLOW__NO_CHECK) // Subnormal*subnormal can be 0, big*big can be inf.

LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Neg_s, Neg_s, ExtNonNeg_s, ALLOW__NO_CHECK) // -1/-subnormal can be inf, subnormal/big can be 0.
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Neg_s, Pos_s, ExtNonPos_s, ALLOW__NO_CHECK) // -1/subnormal can be -inf, subnormal/big can be 0.
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Pos_s, Pos_s, ExtNonNeg_s, ALLOW__NO_CHECK) // 1/subnormal can be inf, subnormal/big can be 0.
LVD_DEFINE_SEMANTIC_BIN_OP(/, div, Pos_s, Neg_s, ExtNonPos_s, ALLOW__NO_CHECK) // 1/-subnormal can be -inf, subnormal/big can be 0.

// Semantic value type factories.

#define DEFINE_SEMANTIC_VALUE_TYPE(BaseName) template <typename C_> using BaseName##_t = SV_t<BaseName##_s,C_>; using BaseName = BaseName##_t<double>;

DEFINE_SEMANTIC_VALUE_TYPE(NaNExtReal)
DEFINE_SEMANTIC_VALUE_TYPE(NaN)
DEFINE_SEMANTIC_VALUE_TYPE(ExtReal)
DEFINE_SEMANTIC_VALUE_TYPE(Infinite)
DEFINE_SEMANTIC_VALUE_TYPE(ExtNonZero)
DEFINE_SEMANTIC_VALUE_TYPE(ExtNonNeg)
DEFINE_SEMANTIC_VALUE_TYPE(ExtNonPos)
DEFINE_SEMANTIC_VALUE_TYPE(ExtPos)
DEFINE_SEMANTIC_VALUE_TYPE(ExtNeg)
DEFINE_SEMANTIC_VALUE_TYPE(Infinity)
DEFINE_SEMANTIC_VALUE_TYPE(NegInfinity)
DEFINE_SEMANTIC_VALUE_TYPE(Real)
DEFINE_SEMANTIC_VALUE_TYPE(NonZero)
DEFINE_SEMANTIC_VALUE_TYPE(NonNeg)
DEFINE_SEMANTIC_VALUE_TYPE(NonPos)
DEFINE_SEMANTIC_VALUE_TYPE(Pos)
DEFINE_SEMANTIC_VALUE_TYPE(Neg)
DEFINE_SEMANTIC_VALUE_TYPE(Zero)
DEFINE_SEMANTIC_VALUE_TYPE(One)
DEFINE_SEMANTIC_VALUE_TYPE(NegOne)

LVD_TEST_BEGIN(007__semantic_subtype__01__float__00)
    auto z1 = Zero{0};

    test_log << Log::dbg() << LVD_REFLECT(z1) << '\n';

    auto nn1 = NonNeg{0};
    auto nn2 = NonNeg{nn1};
    auto nn3 = NonNeg{1};
    auto nn4 = NonNeg{z1}; // Can construct from a sub-semantic-type.

    test_log << Log::dbg() << LVD_REFLECT(nn1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(nn2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(nn3) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(nn4) << '\n';
LVD_TEST_END

} // end namespace lvd

LVD_DEFINE_TYPE_STRING(lvd::NaNExtReal_s);
LVD_DEFINE_TYPE_STRING(lvd::NaN_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtReal_s);
LVD_DEFINE_TYPE_STRING(lvd::Infinite_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtNonZero_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtNonNeg_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtNonPos_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtPos_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtNeg_s);
LVD_DEFINE_TYPE_STRING(lvd::Infinity_s);
LVD_DEFINE_TYPE_STRING(lvd::NegInfinity_s);
LVD_DEFINE_TYPE_STRING(lvd::Real_s);
LVD_DEFINE_TYPE_STRING(lvd::NonZero_s);
LVD_DEFINE_TYPE_STRING(lvd::NonNeg_s);
LVD_DEFINE_TYPE_STRING(lvd::NonPos_s);
LVD_DEFINE_TYPE_STRING(lvd::Pos_s);
LVD_DEFINE_TYPE_STRING(lvd::Neg_s);
LVD_DEFINE_TYPE_STRING(lvd::Zero_s);
LVD_DEFINE_TYPE_STRING(lvd::One_s);
LVD_DEFINE_TYPE_STRING(lvd::NegOne_s);

namespace lvd {

LVD_TEST_BEGIN(007__semantic_subtype__01__float__01)
    auto p1 = Pos{0.5};
    auto p2 = Pos{0.75};

    test_log << Log::dbg() << LVD_REFLECT(p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 + p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 - p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 * p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 / p2) << '\n';

    auto n1 = Neg{-0.6};
    auto n2 = Neg{-0.55};

    test_log << Log::dbg() << LVD_REFLECT(n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 + n2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 - n2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 * n2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 / n2) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p1 + n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 - n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 * n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 / n1) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(n1 + p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 - p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 * p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 / p1) << '\n';

    auto z = Zero{0.0};

    test_log << Log::dbg() << LVD_REFLECT(z) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(z * p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z * p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z * n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z * p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z * z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z / p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z / p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z / n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z / p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z / z) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p1 * z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p2 * z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 * z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n2 * z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 / z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p2 / z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 / z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n2 / z) << '\n';

    auto mx = std::numeric_limits<double>::max();
    test_log << Log::dbg() << LVD_REFLECT(mx) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(mx+mx) << '\n';

    auto mn = std::numeric_limits<double>::min();
    auto subnormal = std::numeric_limits<double>::denorm_min();
    test_log << Log::dbg() << "mn = " << std::scientific << mn << '\n';
    test_log << Log::dbg() << LVD_REFLECT(std::isnormal(mn)) << '\n';
    test_log << Log::dbg() << "mn*mn = " << std::scientific << mn*mn << '\n';
    test_log << Log::dbg() << "`mn*mn == 0` = " << std::boolalpha << (mn*mn == 0) << '\n';
    test_log << Log::dbg() << "subnormal = " << std::scientific << subnormal << '\n';
    test_log << Log::dbg() << LVD_REFLECT(std::isnormal(subnormal)) << '\n';
    test_log << Log::dbg() << "subnormal*subnormal = " << std::scientific << subnormal*subnormal << '\n';
    test_log << Log::dbg() << "subnormal/subnormal = " << std::scientific << subnormal/subnormal << '\n';
    test_log << Log::dbg() << "1.0/subnormal = " << std::scientific << 1.0/subnormal << '\n';

    test_log << Log::dbg() << "mx/subnormal = " << std::scientific << mx/subnormal << '\n';
LVD_TEST_END

template <typename Lhs_, typename Rhs_>
void test_ops (req::Context &req_context, Lhs_ const &lhs, Rhs_ const &rhs) {
    auto &test_log = req_context.log();
    test_log << Log::dbg() << "testing " << lhs << " and " << rhs << '\n';
    IndentGuard ig(test_log);
    test_log << Log::trc() << lhs << " + " << rhs << " = " << (lhs + rhs) << '\n';
    test_log << Log::trc() << lhs << " - " << rhs << " = " << (lhs - rhs) << '\n';
    test_log << Log::trc() << lhs << " * " << rhs << " = " << (lhs * rhs) << '\n';
    test_log << Log::trc() << lhs << " / " << rhs << " = " << (lhs / rhs) << '\n';
    test_log << Log::trc() << "+" << lhs << " = " << (+lhs) << '\n';
    test_log << Log::trc() << "-" << lhs << " = " << (-lhs) << '\n';

    // Do explicit checks on each.
    (lhs + rhs).template check<ALLOW__VERIFY_OR_THROW>();
    (lhs - rhs).template check<ALLOW__VERIFY_OR_THROW>();
    (lhs * rhs).template check<ALLOW__VERIFY_OR_THROW>();
    (lhs / rhs).template check<ALLOW__VERIFY_OR_THROW>();
    (+lhs).template check<ALLOW__VERIFY_OR_THROW>();
    (-lhs).template check<ALLOW__VERIFY_OR_THROW>();
}

template <size_t LHS_INDEX_, typename... LhsArgs_, typename Rhs_>
void test_ops_lhs (req::Context &req_context, IndexedTuple_t<LHS_INDEX_,LhsArgs_...> const &lhs, Rhs_ const &rhs) {
    if constexpr (lhs.has_ended()) {
        // Base case; nothing to do.
    } else {
        // Inductive case.
        test_ops(req_context, lhs.current(), rhs);
        test_ops_lhs(req_context, lhs.incremented(), rhs);
    }
}

template <size_t LHS_INDEX_, typename... LhsArgs_, size_t RHS_INDEX_, typename... RhsArgs_>
void test_ops_lhs_rhs (req::Context &req_context, IndexedTuple_t<LHS_INDEX_,LhsArgs_...> const &lhs, IndexedTuple_t<RHS_INDEX_,RhsArgs_...> const &rhs) {
    if constexpr (rhs.has_ended()) {
        // Base case; nothing to do.
    } else {
        // Inductive case.
        test_ops_lhs(req_context, lhs, rhs.current());
        test_ops_lhs_rhs(req_context, lhs, rhs.incremented());
    }
}

LVD_TEST_BEGIN(007__semantic_subtype__01__float__02)
    auto ner0a = NaNExtReal{0};
    auto ner0b = NaNExtReal{-0};
    auto ner1a = NaNExtReal{std::numeric_limits<double>::denorm_min()};
    auto ner1b = NaNExtReal{-std::numeric_limits<double>::denorm_min()};
    auto ner2a = NaNExtReal{std::numeric_limits<double>::min()};
    auto ner2b = NaNExtReal{-std::numeric_limits<double>::min()};
    auto ner3a = NaNExtReal{1};
    auto ner3b = NaNExtReal{-1};
    auto ner4a = NaNExtReal{0.5};
    auto ner4b = NaNExtReal{-0.5};
    auto ner5a = NaNExtReal{2};
    auto ner5b = NaNExtReal{-2};
    auto ner6a = NaNExtReal{std::numeric_limits<double>::max()};
    auto ner6b = NaNExtReal{-std::numeric_limits<double>::max()};
    auto ner7a = NaNExtReal{std::numeric_limits<double>::infinity()};
    auto ner7b = NaNExtReal{-std::numeric_limits<double>::infinity()};
    auto ner8a = NaNExtReal{std::numeric_limits<double>::quiet_NaN()};
    auto ner8b = NaNExtReal{std::numeric_limits<double>::signaling_NaN()};

    auto nan8a = NaNExtReal{std::numeric_limits<double>::quiet_NaN()};
    auto nan8b = NaNExtReal{std::numeric_limits<double>::signaling_NaN()};

    auto er0a = ExtReal{0};
    auto er0b = ExtReal{-0};
    auto er1a = ExtReal{std::numeric_limits<double>::denorm_min()};
    auto er1b = ExtReal{-std::numeric_limits<double>::denorm_min()};
    auto er2a = ExtReal{std::numeric_limits<double>::min()};
    auto er2b = ExtReal{-std::numeric_limits<double>::min()};
    auto er3a = ExtReal{1};
    auto er3b = ExtReal{-1};
    auto er4a = ExtReal{0.5};
    auto er4b = ExtReal{-0.5};
    auto er5a = ExtReal{2};
    auto er5b = ExtReal{-2};
    auto er6a = ExtReal{std::numeric_limits<double>::max()};
    auto er6b = ExtReal{-std::numeric_limits<double>::max()};
    auto er7a = ExtReal{std::numeric_limits<double>::infinity()};
    auto er7b = ExtReal{-std::numeric_limits<double>::infinity()};

    auto infinite7a = NaNExtReal{std::numeric_limits<double>::infinity()};
    auto infinite7b = NaNExtReal{-std::numeric_limits<double>::infinity()};

    auto enz1a = ExtNonZero{std::numeric_limits<double>::denorm_min()};
    auto enz1b = ExtNonZero{-std::numeric_limits<double>::denorm_min()};
    auto enz2a = ExtNonZero{std::numeric_limits<double>::min()};
    auto enz2b = ExtNonZero{-std::numeric_limits<double>::min()};
    auto enz3a = ExtNonZero{1};
    auto enz3b = ExtNonZero{-1};
    auto enz4a = ExtNonZero{0.5};
    auto enz4b = ExtNonZero{-0.5};
    auto enz5a = ExtNonZero{2};
    auto enz5b = ExtNonZero{-2};
    auto enz6a = ExtNonZero{std::numeric_limits<double>::max()};
    auto enz6b = ExtNonZero{-std::numeric_limits<double>::max()};
    auto enz7a = ExtNonZero{std::numeric_limits<double>::infinity()};
    auto enz7b = ExtNonZero{-std::numeric_limits<double>::infinity()};

    auto enn0a = ExtNonNeg{0};
    auto enn1a = ExtNonNeg{std::numeric_limits<double>::denorm_min()};
    auto enn2a = ExtNonNeg{std::numeric_limits<double>::min()};
    auto enn3a = ExtNonNeg{1};
    auto enn4a = ExtNonNeg{0.5};
    auto enn5a = ExtNonNeg{2};
    auto enn6a = ExtNonNeg{std::numeric_limits<double>::max()};
    auto enn7a = ExtNonNeg{std::numeric_limits<double>::infinity()};

    auto enp0b = ExtNonPos{-0};
    auto enp1b = ExtNonPos{-std::numeric_limits<double>::denorm_min()};
    auto enp2b = ExtNonPos{-std::numeric_limits<double>::min()};
    auto enp3b = ExtNonPos{-1};
    auto enp4b = ExtNonPos{-0.5};
    auto enp5b = ExtNonPos{-2};
    auto enp6b = ExtNonPos{-std::numeric_limits<double>::max()};
    auto enp7b = ExtNonPos{-std::numeric_limits<double>::infinity()};

    auto ep1a = ExtPos{std::numeric_limits<double>::denorm_min()};
    auto ep2a = ExtPos{std::numeric_limits<double>::min()};
    auto ep3a = ExtPos{1};
    auto ep4a = ExtPos{0.5};
    auto ep5a = ExtPos{2};
    auto ep6a = ExtPos{std::numeric_limits<double>::max()};
    auto ep7a = ExtPos{std::numeric_limits<double>::infinity()};

    auto en1b = ExtNeg{-std::numeric_limits<double>::denorm_min()};
    auto en2b = ExtNeg{-std::numeric_limits<double>::min()};
    auto en3b = ExtNeg{-1};
    auto en4b = ExtNeg{-0.5};
    auto en5b = ExtNeg{-2};
    auto en6b = ExtNeg{-std::numeric_limits<double>::max()};
    auto en7b = ExtNeg{-std::numeric_limits<double>::infinity()};

    auto i7a = Infinity{std::numeric_limits<double>::infinity()};

    auto ni7b = NegInfinity{-std::numeric_limits<double>::infinity()};

    auto r0a = Real{0};
    auto r0b = Real{-0};
    auto r1a = Real{std::numeric_limits<double>::denorm_min()};
    auto r1b = Real{-std::numeric_limits<double>::denorm_min()};
    auto r2a = Real{std::numeric_limits<double>::min()};
    auto r2b = Real{-std::numeric_limits<double>::min()};
    auto r3a = Real{1};
    auto r3b = Real{-1};
    auto r4a = Real{0.5};
    auto r4b = Real{-0.5};
    auto r5a = Real{2};
    auto r5b = Real{-2};
    auto r6a = Real{std::numeric_limits<double>::max()};
    auto r6b = Real{-std::numeric_limits<double>::max()};

    auto nz1a = NonZero{std::numeric_limits<double>::denorm_min()};
    auto nz1b = NonZero{-std::numeric_limits<double>::denorm_min()};
    auto nz2a = NonZero{std::numeric_limits<double>::min()};
    auto nz2b = NonZero{-std::numeric_limits<double>::min()};
    auto nz3a = NonZero{1};
    auto nz3b = NonZero{-1};
    auto nz4a = NonZero{0.5};
    auto nz4b = NonZero{-0.5};
    auto nz5a = NonZero{2};
    auto nz5b = NonZero{-2};
    auto nz6a = NonZero{std::numeric_limits<double>::max()};
    auto nz6b = NonZero{-std::numeric_limits<double>::max()};

    auto nn0a = NonNeg{0};
    auto nn1a = NonNeg{std::numeric_limits<double>::denorm_min()};
    auto nn2a = NonNeg{std::numeric_limits<double>::min()};
    auto nn3a = NonNeg{1};
    auto nn4a = NonNeg{0.5};
    auto nn5a = NonNeg{2};
    auto nn6a = NonNeg{std::numeric_limits<double>::max()};

    auto np0b = NonPos{-0};
    auto np1b = NonPos{-std::numeric_limits<double>::denorm_min()};
    auto np2b = NonPos{-std::numeric_limits<double>::min()};
    auto np3b = NonPos{-1};
    auto np4b = NonPos{-0.5};
    auto np5b = NonPos{-2};
    auto np6b = NonPos{-std::numeric_limits<double>::max()};

    auto p1a = Pos{std::numeric_limits<double>::denorm_min()};
    auto p2a = Pos{std::numeric_limits<double>::min()};
    auto p3a = Pos{1};
    auto p4a = Pos{0.5};
    auto p5a = Pos{2};
    auto p6a = Pos{std::numeric_limits<double>::max()};

    auto n1b = Neg{-std::numeric_limits<double>::denorm_min()};
    auto n2b = Neg{-std::numeric_limits<double>::min()};
    auto n3b = Neg{-1};
    auto n4b = Neg{-0.5};
    auto n5b = Neg{-2};
    auto n6b = Neg{-std::numeric_limits<double>::max()};

    auto z0a = Zero{0};
    auto z0b = Zero{-0};

    auto all = std::tuple(
        ner0a, ner0b, ner1a, ner1b, ner2a, ner2b, ner3a, ner3b, ner4a, ner4b, ner5a, ner5b, ner6a, ner6b, ner7a, ner7b, ner8a, ner8b,
        nan8a, nan8b,

        er0a, er0b, er1a, er1b, er2a, er2b, er3a, er3b, er4a, er4b, er5a, er5b, er6a, er6b, er7a, er7b,
        infinite7a, infinite7b,
        enz1a, enz1b, enz2a, enz2b, enz3a, enz3b, enz4a, enz4b, enz5a, enz5b, enz6a, enz6b, enz7a, enz7b,
        enn0a, enn1a, enn2a, enn3a, enn4a, enn5a, enn6a, enn7a,
        enp0b, enp1b, enp2b, enp3b, enp4b, enp5b, enp6b, enp7b,
        ep1a, ep2a, ep3a, ep4a, ep5a, ep6a, ep7a,
        en1b, en2b, en3b, en4b, en5b, en6b, en7b,
        i7a,
        ni7b,

        r0a, r0b, r1a, r1b, r2a, r2b, r3a, r3b, r4a, r4b, r5a, r5b, r6a, r6b,
        nz1a, nz1b, nz2a, nz2b, nz3a, nz3b, nz4a, nz4b, nz5a, nz5b, nz6a, nz6b,
        nn0a, nn1a, nn2a, nn3a, nn4a, nn5a, nn6a,
        np0b, np1b, np2b, np3b, np4b, np5b, np6b,
        p1a, p2a, p3a, p4a, p5a, p6a,
        n1b, n2b, n3b, n4b, n5b, n6b,

        z0a, z0b
    );

    std::ignore = all;

    test_ops_lhs_rhs(req_context, begin_indexed_tuple(all), begin_indexed_tuple(all));
LVD_TEST_END

} // end namespace lvd
