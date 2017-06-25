#pragma once
#include <iostream>
#include <math.h>

//
// ### Rational Math ###########################################################
//

template<int numerator, int denominator>
class Rational
{
public:
    Rational() = default;

    constexpr operator double() const
    {
        return numerator / double(denominator);
    }
    constexpr double operator()(const double * x) const
    {
        return double(*this);
    }

};

template<int num1, int num2, int denom1, int denom2>
constexpr Rational<num1 * denom2 + num2 * denom1, denom1 * denom2> operator+(const Rational<num1, denom1> &r1, const Rational<num2, denom2> &r2) { return {}; }
template<int num1, int num2, int denom1, int denom2>
constexpr Rational<num1 * denom2 - num2 * denom1, denom1 * denom2> operator-(const Rational<num1, denom1> &r1, const Rational<num2, denom2> &r2) { return {}; }
template<int num1, int num2, int denom1, int denom2>
constexpr Rational<num1 * num2, denom1 * denom2> operator*(const Rational<num1, denom1> &r1, const Rational<num2, denom2> &r2) { return {}; }
template<int num1, int num2, int denom1, int denom2>
constexpr Rational<num1 * denom2, num2 * denom1> operator/(const Rational<num1, denom1> &r1, const Rational<num2, denom2> &r2) { return {}; }

//
// ### Symbolic Variable Math ##################################################
// Based on: https://arxiv.org/pdf/1705.01729.pdf
//

template <size_t index>
class Variable
{
//
// General variable type, used to build up expressions
//
public:
    constexpr double operator()(const double *x) const
    {
        return x[index];
    }
};

////////////////////////////////////////////////////////////////////////////////

template <int value>
class Integer
{
//
// Integer type
//
public:
    constexpr double operator()(const double *x) const
    {
        return value;
    }
};

////////////////////////////////////////////////////////////////////////////////

// template <typename T>
// class Constant
// {
// public:
//     Constant() : expression(T()), value(expression(0)) { }
//
//     inline constexpr double operator()(const double* x) { return value; }
//
// private:
//     T expression;
//     double value;
// };

////////////////////////////////////////////////////////////////////////////////

template <typename lhs, typename rhs, typename op>
class BinaryOp
{
//
// General binary operator between two general types
//
public:
    constexpr double operator()(const double *x) const
    {
        return op::apply(l(x), r(x));
    }
private:
    lhs l;
    rhs r;
};

////////////////////////////////////////////////////////////////////////////////

#define DEFINE_BINARY_OP(NAME, OPERATOR)                                         \
class NAME                                                                       \
{                                                                                \
public:                                                                          \
    static constexpr double apply(double a, double b)                            \
    {                                                                            \
        return a OPERATOR b;                                                     \
    }                                                                            \
};                                                                               \
                                                                                 \
template<typename lhs, typename rhs>                                             \
constexpr BinaryOp<lhs, rhs, NAME> operator OPERATOR(const lhs &l, const rhs &r) \
{                                                                                \
    return {l, r};                                                               \
}

////////////////////////////////////////////////////////////////////////////////

#define DEFINE_FUNCTION_OP(MATH_FUNCTION)                                          \
template <typename T>                                                              \
class Symbolic_##MATH_FUNCTION                                                     \
{                                                                                  \
public:                                                                            \
    inline constexpr double operator()(const double* x) const                      \
    {                                                                              \
        return MATH_FUNCTION(expression(x));                                       \
    }                                                                              \
                                                                                   \
private:                                                                           \
    T expression;                                                                  \
};                                                                                 \
                                                                                   \
template <typename T>                                                              \
inline Symbolic_##MATH_FUNCTION<T> MATH_FUNCTION(const T& expression)              \
{                                                                                  \
    return {expression};                                                           \
}

DEFINE_BINARY_OP(Add, +)
DEFINE_BINARY_OP(Sub, -)
DEFINE_BINARY_OP(Div, /)
DEFINE_BINARY_OP(Mult, *)

DEFINE_FUNCTION_OP(sin)
DEFINE_FUNCTION_OP(cos)
DEFINE_FUNCTION_OP(exp)

//
// Power is *almost* a FUNCTION_OP but not quite
//
template <typename T, int power>
class Symbolic_pow
{
public:
    inline constexpr double operator()(const double* x) const
    {
        return pow(expression(x), power);
    }

private:
    T expression;
};

template <typename T, int power>
inline Symbolic_pow<T, power> pow(const T& expression, const Integer<power> power_)
{
    return {expression};
}
//
// ############################################################################
//

template <class n, class m>
class Der
//
// "Base" template type, everything else is specialised from here
//
{
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index, int value>
class Der<Variable<index>, Integer<value> >
{
//
// Partial derivatives of any integer is 0
//
public:
    typedef Integer<0> DerType;
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index>
class Der<Variable<index>, Variable<index> >
//
// Partial derivatives of any variable and itself is 1
//
{
public:
    typedef Integer<1> DerType;
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index, size_t index_1>
class Der<Variable<index>, Variable<index_1> >
{
//
// Partial derivatives of any variable with any other variable is 0
//
public:
    typedef Integer<0> DerType;
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index, typename lhs, typename rhs>
class Der<Variable<index>, BinaryOp<lhs, rhs, Add> >
{
//
// Sum rule: d(F + G) = dF + dG
//
private:
    typedef typename Der<Variable<index>, lhs>::DerType d_l;
    typedef typename Der<Variable<index>, rhs>::DerType d_r;

public:
    typedef BinaryOp<d_l, d_r, Add> DerType;
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index, typename lhs, typename rhs>
class Der<Variable<index>, BinaryOp<lhs, rhs, Sub> >
{
//
// Subtraction rule: d(F - G) = dF - dG
//
private:
    typedef typename Der<Variable<index>, lhs>::DerType d_l;
    typedef typename Der<Variable<index>, rhs>::DerType d_r;

public:
    typedef BinaryOp<d_l, d_r, Sub> DerType;
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index, typename lhs, typename rhs>
class Der<Variable<index>, BinaryOp<lhs, rhs, Mult> >
{
//
// Chain rule: d(L * R) = (dL * R) + (L * dR)
//
private:
    typedef typename Der<Variable<index>, lhs>::DerType d_l;
    typedef typename Der<Variable<index>, rhs>::DerType d_r;

public:
    typedef BinaryOp<BinaryOp<d_l, rhs, Mult>,
                     BinaryOp<lhs, d_r, Mult>,
                     Add> DerType;
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index, typename F>
class Der<Variable<index>, Symbolic_exp<F> >
{
//
// Exp derivative: d(e ^ f) = df * e ^ f
//
private:
    typedef typename Der<Variable<index>, F>::DerType d_f;

public:
    typedef BinaryOp<d_f, Symbolic_exp<F>, Mult> DerType;
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index, typename u, typename v>
class Der<Variable<index>, BinaryOp<u, v, Div> >
{
//
// Quotient rule: d(u / v) = (du * v - u * dv) / (v * v)
//
private:
    typedef typename Der<Variable<index>, u>::DerType d_u;
    typedef typename Der<Variable<index>, v>::DerType d_v;

    typedef BinaryOp<BinaryOp<d_u, v, Mult>,
                     BinaryOp<u, d_v, Mult>,
                     Sub> Numerator;
    typedef BinaryOp<v, v, Mult> Denominator;

public:
    typedef BinaryOp<Numerator, Denominator, Div> DerType;
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index, typename F>
class Der<Variable<index>, Symbolic_sin<F> >
{
//
// d(sin(F)) = dF * cos(F)
//
private:
    typedef typename Der<Variable<index>, F>::DerType d_F;

public:
    typedef BinaryOp<Symbolic_cos<F>, d_F, Mult> DerType;
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index, typename F>
class Der<Variable<index>, Symbolic_cos<F> >
{
//
// d(cos(F)) = -dF * sin(F)
//
private:
    typedef typename Der<Variable<index>, F>::DerType d_F;

public:
    typedef BinaryOp<Integer<-1>, BinaryOp<Symbolic_cos<F>, d_F, Mult>, Mult> DerType;
};

///////////////////////////////////////////////////////////////////////////////

template <size_t index, typename F, int power>
class Der<Variable<index>, Symbolic_pow<F, power> >
{
//
// Power Rule: d(F ^ n) = n * F ^ (n - 1)
//
public:
    typedef BinaryOp<Integer<power>, Symbolic_pow<F, power - 1>, Mult> DerType;
};
