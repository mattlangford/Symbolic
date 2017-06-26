#include "types.hh"

enum Activation : uint8_t
{
    IDENTITY,
    LOGISTIC
};

namespace ActivationFunctions
{
    template<Activation type, typename T>
    struct Function
    {
        static auto impl() {}
    };

    ///////////////////////////////////////////////////////////////////////////

    template<typename T>
    struct Function<IDENTITY, T>
    {
        static auto impl() { return T(); }
    };

    ///////////////////////////////////////////////////////////////////////////

    template<typename T>
    struct Function<LOGISTIC, T>
    {
        static auto impl() { return Integer<1>() / (Integer<1>() + Symbolic_exp<BinaryOp<Integer<0>, T, Sub> >()); }
    };
};

//
// The following functions support the `perceptron` function.
// A perceptron type can be created like this: perceptron<ACTIVATION_TYPE, x0,w0, x1,w1 .... xn,wn, b>(),
// where x0,w0 ... xn,wn are the inputs and weights that will be dotted together and passed through the
// activation function based on the enum type
//

template <int b>
inline constexpr Variable<b> dot()
{
    return Variable<b>();
}

///////////////////////////////////////////////////////////////////////////////

template <int x, int w, int...Args>
inline constexpr auto dot()
{
    return Variable<x>() * Variable<w>() + dot<Args...>();
}

///////////////////////////////////////////////////////////////////////////////

template <Activation type, int...Args>
inline constexpr auto perceptron()
{
    return ActivationFunctions::Function<type, decltype(dot<Args...>())>::impl();
}

//
//
//

int main()
{
    Variable<0> x0;
    Variable<1> w0;
    Variable<2> x1;
    Variable<3> w1;
    Variable<4> b;

    using T = decltype(perceptron<LOGISTIC, x0.id,w0.id, x1.id,w1.id, b.id>());

    double vars[] = {1, 1, 1, 1, 1};
    std::cout << T()(vars) << std::endl;
}
