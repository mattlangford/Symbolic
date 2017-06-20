#include "symbolicc++.h"
#include <stdlib.h>
#include <time.h>

//
// ## Types ###################################################################
//

enum ACTIVATION : uint8_t
{
    IDENTITY,
    LOGISTIC,
    TEST,
};

//
// ### Forward Propagation ####################################################
//

template <ACTIVATION act>
Symbolic activation_function(const Symbolic &x) { std::cout << "NONE FOUND" << std::endl; return 0; }

template <>
inline Symbolic activation_function<LOGISTIC>(const Symbolic &x)
{
    return 1 / (exp(-x) + 1);
}

template <>
inline Symbolic activation_function<IDENTITY>(const Symbolic &x)
{
    return x;
}
template <>
inline Symbolic activation_function<TEST>(const Symbolic &x)
{
    return exp(x);
}

//
// ### Functions used to train network ########################################
//

double generate_output(const double x0, const double x1)
{
    // Unknown to the network
    Symbolic e("e");
    return activation_function<LOGISTIC>(x0 + 2 * x1 - 6)[e==std::exp(1)];
}

std::pair<double, double> generate_input()
{
    return {rand() % 100 / 100.0, rand() % 100 / 100.0};
}

int main()
{
    srand(time(NULL));
    // const std::pair<double, double> input = generate_input();
    // std::cout << generate_output(input.first, input.second) << std::endl;

    double learning_rate = 10;
    Symbolic e("e");

    Symbolic _w00("_w00");
    double w00 = 0;
    Symbolic _w01("_w01");
    double w01 = 0;
    Symbolic _b00("_b00");
    double b00 = 1;

    constexpr size_t TRAINS = 100;
    for (size_t i = 0; i < TRAINS; ++i)
    {
        const std::pair<double, double> input = generate_input();
        Symbolic x0 = input.first;
        Symbolic x1 = input.second;
        Symbolic out = Symbolic(generate_output(input.first, input.second));

        Symbolic predict = activation_function<LOGISTIC>(_w00 * x0 + _w01 * x1 + _b00);
        Symbolic loss = (predict - out) ^ 2;

        double df_loss_w00 = df(loss, _w00)[_w00==w00, _w01==w01, _b00==b00, e==std::exp(1)];
        double df_loss_w01 = df(loss, _w01)[_w00==w00, _w01==w01, _b00==b00, e==std::exp(1)];
        double df_loss_b00 = df(loss, _b00)[_w00==w00, _w01==w01, _b00==b00, e==std::exp(1)];

        std::cout << "LOSS: " << loss[_w00==w00, _w01==w01, _b00==b00, e==std::exp(1)] << std::endl;

        w00 -= learning_rate * df_loss_w00 / (i + 1);
        w01 -= learning_rate * df_loss_w01 / (i + 1);
        b00 -= learning_rate * df_loss_b00 / (i + 1);
    }

    std::cout << "w00: " << w00 << " w01: " << w01 << " b00: " << b00 << std::endl;

    double error = 0;
    constexpr size_t TESTS = 200;
    for (size_t i = 0; i < TESTS; ++i)
    {
        const std::pair<double, double> input = generate_input();
        double x0 = input.first;
        double x1 = input.second;
        double out = generate_output(input.first, input.second);

        double predict = activation_function<LOGISTIC>(w00 * x0 + w01 * x1 + b00)[e==std::exp(1)];
        double loss = (predict - out) * (predict - out);

        error += loss;
    }

    std::cout << error / TESTS << std::endl;
}
