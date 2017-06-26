#include <array>
#include <iostream>
#include <random>

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

///////////////////////////////////////////////////////////////////////////////

template <Activation type, typename T>
inline constexpr auto perceptron()
{
    return ActivationFunctions::Function<type, T>::impl();
}

///////////////////////////////////////////////////////////////////////////////

double mystery_function(double a, double b)
{
    return 2 * a + 3 * b;
}

template <typename T>
void print_type();

int main()
{
    //
    // Ignore this, random number stuff for generating test cases
    //
    std::random_device r;
    std::mt19937 rd(r());
    std::normal_distribution<double> norm(0.0, 10.0);

    //
    // Variable declaration, they don't have to be in order but should correspond to an
    // entry in the vars[] array
    //
    Variable<0> x00;
    Variable<1> w00;
    Variable<2> x01;
    Variable<3> w01;
    Variable<4> b00;
    Variable<8> w02;
    Variable<9> w03;
    Variable<10> b01;

    Variable<5> w10;
    Variable<6> w11;
    Variable<7> b10;

    Variable<11> target;

    constexpr size_t MAX_VAR = 12;
    double vars[MAX_VAR] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2};

    //
    // The activation function type
    //
    constexpr Activation type = IDENTITY;

    //
    // This computes the first layer
    //
    using dot_0 = decltype(x00 * w00 + x01 * w01 + b00);
    auto out_0 = perceptron<type, dot_0>();
    using dot_1 = decltype(x00 * w02 + x01 * w03 + b01);
    auto out_1 = perceptron<type, dot_1>();

    //
    // This computes the second layer
    //
    using dot_2 = decltype(out_0 * w10 + out_1 * w11 + b10);
    auto predict = perceptron<type, dot_2>();

    //
    // Squared error function, along with all the partial derivatives for each weight/bias
    //
    using error_function = decltype((predict - target) * (predict - target));  // TODO: This should use pow(...)
#define COMPUTE_PARTIAL(VAR) auto d##VAR = Der<decltype(VAR), error_function>::DerType();
    COMPUTE_PARTIAL(w00); COMPUTE_PARTIAL(w01); COMPUTE_PARTIAL(b00);
    COMPUTE_PARTIAL(w02); COMPUTE_PARTIAL(w03); COMPUTE_PARTIAL(b01);
    COMPUTE_PARTIAL(w10); COMPUTE_PARTIAL(w11); COMPUTE_PARTIAL(b10);

    //
    // Learning here
    //
    constexpr double learning_rate = 0.0000005;
    constexpr size_t EPOCS = 200;
    for (size_t i = 0; i < EPOCS; ++i)
    {
        //
        // Get the original x arguments and training expected solution
        //
        vars[x00.id] = norm(r);
        vars[x01.id] = norm(r);
        vars[target.id] = mystery_function(vars[x00.id], vars[x01.id]);

        //
        // Local copy of the variables so the weight updates don't effect things
        //
        double temp_vars[MAX_VAR];
        std::copy(std::begin(vars), std::end(vars), std::begin(temp_vars));

        //
        // One by one update each weight, using a macro to avoid some typing
        //
#define UPDATE(VAR) vars[VAR.id] -= learning_rate * d##VAR(temp_vars);
        UPDATE(w00); UPDATE(w01); UPDATE(b00);
        UPDATE(w02); UPDATE(w03); UPDATE(b01);
        UPDATE(w10); UPDATE(w11); UPDATE(b10);

        std::cout << dw00(temp_vars) << std::endl;
        std::cout << temp_vars[0] << " " << temp_vars[1] << " " << temp_vars[2] << std::endl;

        //std::cout << error_function()(vars.data()) << std::endl;
    }

}
