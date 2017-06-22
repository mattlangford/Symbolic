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

//
// *I THINK* these will be preferred over the normie math below when using Rational types
//
// template<int num1, int num2, int denom1, int denom2>
// constexpr Rational<num1 * denom2 + num2 * denom1, denom1 * denom2> operator+(const Rational<num1, denom1> &r1, const Rational<num2, denom2> &r2) { return {}; }
// template<int num1, int num2, int denom1, int denom2>
// constexpr Rational<num1 * denom2 - num2 * denom1, denom1 * denom2> operator-(const Rational<num1, denom1> &r1, const Rational<num2, denom2> &r2) { return {}; }
// template<int num1, int num2, int denom1, int denom2>
// constexpr Rational<num1 * num2, denom1 * denom2> operator*(const Rational<num1, denom1> &r1, const Rational<num2, denom2> &r2) { return {}; }
// template<int num1, int num2, int denom1, int denom2>
// constexpr Rational<num1 * denom2, num2 * denom1> operator/(const Rational<num1, denom1> &r1, const Rational<num2, denom2> &r2) { return {}; }
// 
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
    BinaryOp(const lhs &l_, const rhs &r_) : l(l_), r(r_) {};

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
}                                                                                \

////////////////////////////////////////////////////////////////////////////////

#define DEFINE_FUNCTION_OP(MATH_FUNCTION)                                          \
template <typename T>                                                              \
class _Symbolic_##MATH_FUNCTION                                                    \
{                                                                                  \
public:                                                                            \
    _Symbolic_##MATH_FUNCTION(const T& expression_) : expression(expression_) { }; \
                                                                                   \
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
inline _Symbolic_##MATH_FUNCTION<T> MATH_FUNCTION(const T& expression)             \
{                                                                                  \
    return {expression};                                                           \
}                                                                                  \

DEFINE_BINARY_OP(Add, +)
DEFINE_BINARY_OP(Sub, -)
DEFINE_BINARY_OP(Div, /)
DEFINE_BINARY_OP(Mult, *)

DEFINE_FUNCTION_OP(sin)
DEFINE_FUNCTION_OP(cos)
DEFINE_FUNCTION_OP(exp)

int main()
{
    Variable<0> x;
    Variable<1> y;

    double vars[] = {1, 2};
    auto z = sin(x + (y / x) + x * x * x);
    std::cout << z(vars) << std::endl;
}