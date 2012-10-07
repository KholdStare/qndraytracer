#ifndef _QND_COORDINATE_BASE_HPP_
#define _QND_COORDINATE_BASE_HPP_

#include <Eigen/Dense>

// TODO: make Expression wrapper with compile time attributes so expression
// can carry over information about the type ( e.g. whether it's a point or vector )

// TODO: instead of a bool, take an arbitrary tag class, that can be propagated
// on top of expression templates. So you could have a "vector" or "point" or "normal" tag for example

namespace qnd {

    /**
     * A "facade" for Eigen vectors, that stores as part of its type
     * whether it is a vector or a point. Delegates all operations to
     * Eigen, including propagation of expression templates
     *
     * As a sidenote, this is complete overkill- it is just for trying out
     * the concept of "hijacking" expression templates
     */
    template<typename Scalar, int Dim, bool isVector>
    class CoordinateBase {

    public:
        typedef Eigen::Matrix<Scalar, Dim, 1> EigenType;
        typedef CoordinateBase<Scalar, Dim, isVector> SelfType;

        EigenType v;

        // CONSTRUCTORS
        CoordinateBase() : v(0, 0, 0) { }
        CoordinateBase(double x, double y, double z) : v(x, y, z) { }
        CoordinateBase(EigenType const& other) : v(other) { }

        // without explicit keyword, this constructor will be used in the middle
        // of expressions, when instead we want to propagate expression templates.
        // TODO: not working yet, need to "tag" expression templates too
        template <class Expression>
        CoordinateBase(Expression const& other) : v(other) { }

        // ASSIGNMENT OPERATORS
        // expression template assignment
        template <class Expression>
        CoordinateBase& operator=(Expression const& other) {
            v = other;
            return *this;
        }

        template <bool otherVector>
        CoordinateBase& operator=(CoordinateBase<Scalar, Dim, otherVector> const& other) {
            v = other.v;
            return *this;
        }

        // CONVERSION OPERATORS
        // convert to underlying eigen type
        operator EigenType() const { return v; }

        // ACCESS OPERATORS
        Scalar& operator [](size_t i) { return v(i); }
        Scalar const& operator [](size_t i) const { return v(i); }

        // ALGEBRA OPERATORS
        auto dot(SelfType const& other) const -> decltype(v.dot(other.v)) {
            return v.dot(other.v);
        }

        auto cross(SelfType const& other) const -> decltype(v.cross(other.v)) {
            return v.cross(other.v);
        }

        Scalar norm() const { return v.norm(); }
        Scalar squaredNorm() const { return v.squaredNorm(); }
        // Normalize the vector and return the norm
        Scalar normalize() {
            Scalar l = this->norm();
            if (l != 0.0) {
                v /= l;
            }
            return l;
        }

        // OPERATORS that propagate Eigen's expression templates
        // Allow eigen's expression templates to propagate through, by allowing
        // the compiler to decide the return type

        auto operator -() const -> decltype(-EigenType() ) const {
            return -v; 
        }

        // Macro for producing operator and its corresponding
        // assignment operator for expression template delegation
        // e.g. makes operator+ and operator+=
        //
        // operators will also accept Scalar quantities
#define MEMBER_EXPRESSION_DELEGATION_OP(Op) template <class Expression> \
        auto operator Op (Expression const& other) const -> decltype(EigenType() Op other ) const { \
            return v Op other; \
        } \
          \
        template <class Expression> \
        SelfType& operator Op##= (Expression const& other) { \
            v Op##= other; \
            return *this; \
        }


        MEMBER_EXPRESSION_DELEGATION_OP(+)
        MEMBER_EXPRESSION_DELEGATION_OP(-)
        MEMBER_EXPRESSION_DELEGATION_OP(*)
        MEMBER_EXPRESSION_DELEGATION_OP(/)

#undef MEMBER_EXPRESSION_DELEGATION_OP


#define MEMBER_OP(Op) auto operator Op (SelfType const& other) const -> decltype(EigenType() Op EigenType() ) const { \
            return v Op other.v; \
        } \
          \
        SelfType& operator Op##= (SelfType const& other) { \
            v Op##= other.v; \
            return *this; \
        }

        MEMBER_OP(+)
        MEMBER_OP(-)

#undef MEMBER_OP

    };

    // NONMEMBER operators for working with expression templates
#define NONMEMBER_EXPRESSION_DELEGATION_OP(Op) template<typename Scalar, int Dim, bool isVector, class Expression> \
    auto operator Op (Expression const& eigenExpr, CoordinateBase<Scalar, Dim, isVector> const& coord) -> decltype(eigenExpr Op coord.v) { \
        return eigenExpr Op coord.v; \
    }

    // Allow eigen's expression templates for various operators
    // Allow (expression + coordinate)
    NONMEMBER_EXPRESSION_DELEGATION_OP(+)
    NONMEMBER_EXPRESSION_DELEGATION_OP(-)
    NONMEMBER_EXPRESSION_DELEGATION_OP(*)
    NONMEMBER_EXPRESSION_DELEGATION_OP(/)

#undef NONMEMBER_EXPRESSION_DELEGATION_OP

template <typename Scalar, int Dim>
using Vector = CoordinateBase<Scalar, Dim, true>;

template <typename Scalar, int Dim>
using Point = CoordinateBase<Scalar, Dim, false>;

}

#endif // _QND_COORDINATE_BASE_HPP_
