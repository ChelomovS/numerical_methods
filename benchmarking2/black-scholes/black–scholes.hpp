#pragma once

#include <cmath>
#include <numbers>

namespace NumMethods
{

class BS
{
public:
    BS(double begin_stock_price, double strike_price, double volatility, double risk_free_rate, double maturity_time)
        : begin_stock_price{begin_stock_price}, strike_price{strike_price}, volatility{volatility}, risk_free_rate{risk_free_rate},
          maturity_time{maturity_time}
    {
    }
    double get_price() const
    {
        double v_sqrt_time = volatility * std::sqrt(maturity_time);
        double d1 =
            (std::log(begin_stock_price / strike_price) + (risk_free_rate + 0.5 * volatility * volatility) * maturity_time) / v_sqrt_time;

        double d2 = d1 - v_sqrt_time;

        // функция распределения стандартного нормального распределения
        auto N = [](double x) { return 0.5 * (1.0 + std::erf(x / std::numbers::sqrt2)); };

        return begin_stock_price * N(d1) - strike_price * std::exp(-risk_free_rate * maturity_time) * N(d2);
    }

private:
    double begin_stock_price;
    double strike_price;
    double volatility;
    double risk_free_rate;
    double maturity_time;
};

} // namespace NumMethods
