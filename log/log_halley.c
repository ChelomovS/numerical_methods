#include "log_halley.h"

#include <math.h>

double log_halley(double x)
{
    if (x <= 0)
        return -INFINITY;

    double y = 0.0;
    for (int i = 0; i < 5; i++)
    {
        double ey = exp(y);
        y         = y + 2.0 * (x - ey) / (x + ey);
    }
    return y;
}
