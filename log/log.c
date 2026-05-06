#include "logf_coeffs.h"
#include "table.h"
#include <math.h>
#include <stdint.h>
#include <string.h>

float logf(float x)
{
    if (isnan(x) || x < 0.0f)
        return NAN;
    if (x == 1.0f)
        return 0.0f;
    if (x == 0.0f)
        return -INFINITY;
    if (isinf(x))
        return INFINITY;
    uint32_t ix;
    memcpy(&ix, &x, sizeof(float));

    int e_offset = 0;
    if (ix < 0x00800000)
    {
        x *= 8388608.0f;
        memcpy(&ix, &x, sizeof(float));
        e_offset = -23;
    }

    uint32_t bias    = 0x3f3504f3;
    uint32_t ux_norm = ix - bias;
    int e            = (int32_t)ux_norm >> 23;

    uint32_t ux_red = ix - (ux_norm & 0xff800000);
    float m;
    memcpy(&m, &ux_red, sizeof(float));

    int i = (ux_norm & 0x007FFFFF) >> 15;

    float rcp = R_TABLE[i];
    float t   = T_TABLE[i];
    float r   = fmaf(m, rcp, -1.0f);

    float p    = LOGF_POLY_COEFF[4];
    p          = fmaf(r, p, LOGF_POLY_COEFF[3]);
    p          = fmaf(r, p, LOGF_POLY_COEFF[2]);
    p          = fmaf(r, p, LOGF_POLY_COEFF[1]);
    p          = fmaf(r, p, LOGF_POLY_COEFF[0]);
    float poly = fmaf(r, p, 0.0);

    float result = fmaf((float)(e + e_offset), LOGF_LN2, t + poly);

    return result;
}