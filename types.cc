#include <iostream>

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

class Add
{
//
// Addition between two doubles
//
public:
    static constexpr double apply(double a, double b)
    {
        return a + b;
    }
};

template<typename lhs, typename rhs>
constexpr BinaryOp<lhs, rhs, Add> operator+(const lhs &l, const rhs &r)
{
    return {l, r};
}

////////////////////////////////////////////////////////////////////////////////

class Subtract
{
//
// Subtraction between two doubles
//
public:
    static constexpr double apply(double a, double b)
    {
        return a - b;
    }
};

template<typename lhs, typename rhs>
constexpr BinaryOp<lhs, rhs, Subtract> operator-(const lhs &l, const rhs &r)
{
    return {l, r};
}

////////////////////////////////////////////////////////////////////////////////

class Multiply
{
//
// Multiplication between two doubles
//
public:
    static constexpr double apply(double a, double b)
    {
        return a * b;
    }
};

template<typename lhs, typename rhs>
constexpr BinaryOp<lhs, rhs, Multiply> operator*(const lhs &l, const rhs &r)
{
    return {l, r};
}

////////////////////////////////////////////////////////////////////////////////

class Divide
{
//
// Division between two doubles
// TODO: This could return a Rational type
//
public:
    static constexpr double apply(double a, double b)
    {
        return a / b;
    }
};

template<typename lhs, typename rhs>
constexpr BinaryOp<lhs, rhs, Divide> operator/(const lhs &l, const rhs &r)
{
    return {l, r};
}

////////////////////////////////////////////////////////////////////////////////

int main()
{
    Rational<1, 2> half;
    Rational<1, 3> third;

    Variable<0> x;
    Variable<1> y;

    double vars[] = {1, 2};
    auto z = x + (y / x) + x * x * x + (half + third);
    std::cout << z(vars) << std::endl;
}
