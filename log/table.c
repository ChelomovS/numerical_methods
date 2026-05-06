#include "table.h"
#include "log_halley.h"
#include <stdint.h>
#include <string.h>

float R_TABLE[256];
float T_TABLE[256];

void init_lookup_table()
{
    uint32_t bias = 0x3f3504f3;

    for (int i = 0; i < 256; i++)
    {
        uint32_t ux_norm_mid = (i << 15) + (1 << 14);
        uint32_t ix_mid      = ux_norm_mid + bias;

        uint32_t ux_red = ix_mid - (ux_norm_mid & 0xff800000);
        float m_mid;
        memcpy(&m_mid, &ux_red, sizeof(float));

        double C;
        if (m_mid > 0.99f && m_mid < 1.01f)
        {
            C = 1.0;
        }
        else
        {
            C = 1.0 / (double)m_mid;
        }

        float C_float = (float)C;
        R_TABLE[i]    = C_float;
        T_TABLE[i]    = (float)(-log_halley((double)C_float));
    }
}