#include "log.h"
#include "table.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float get_ulp_error(double ref, float test)
{
    if (ref == (double)test)
        return 0.0f;
    if (isnan(ref) || isnan(test))
        return (isnan(ref) && isnan(test)) ? 0.0f : 1e30f;
    if (isinf(ref) || isinf(test))
        return (ref == (double)test) ? 0.0f : 1e30f;

    uint64_t ref_bits;
    memcpy(&ref_bits, &ref, sizeof(uint64_t));
    const uint64_t EXP_MASK = 0x7FF0000000000000;
    uint64_t exp_bits       = ref_bits & EXP_MASK;

    double ulp_value;
    if (exp_bits < (23ULL << 52))
    {
        ulp_value = 1.401298464324817e-45;
    }
    else
    {
        uint64_t ulp_bits = exp_bits - (23ULL << 52);
        memcpy(&ulp_value, &ulp_bits, sizeof(double));
    }

    double diff = fabs(ref - (double)test);
    return (float)(diff / ulp_value);
}

#ifdef TESTING
int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <start_hex> <end_hex>\n", argv[0]);
        return EXIT_FAILURE;
    }

    init_lookup_table();

    uint32_t start_bits = (uint32_t)strtoul(argv[1], NULL, 16);
    uint32_t end_bits   = (uint32_t)strtoul(argv[2], NULL, 16);

    float max_err         = 0.0f;
    uint32_t max_err_bits = 0;
    uint64_t count        = 0;
    uint64_t errors       = 0;

    for (uint32_t bits = start_bits; bits <= end_bits; bits++)
    {
        float input;
        memcpy(&input, &bits, sizeof(float));

        float my_val   = logf(input);
        double ref_val = log((double)input);

        float err = get_ulp_error(ref_val, my_val);

        if (err > max_err)
        {
            max_err      = err;
            max_err_bits = bits;
        }

        if (err > 3.5f)
        {
            errors++;
        }

        count++;
        if (bits == end_bits)
            break;
    }

    float max_err_val;
    memcpy(&max_err_val, &max_err_bits, sizeof(float));

    printf("Tested: %llu\n", count);
    printf("Errors (>3.5 ULP): %llu\n", errors);
    printf("Max ULP: %.6f (x = %.9g, hex = 0x%08X)\n", max_err, max_err_val, max_err_bits);

    return (errors == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
#endif

#ifndef TESTING
int main()
{
    init_lookup_table();

    float input = 0;
    if (scanf("%f", &input) != 1)
        return EXIT_FAILURE;

    float my_val   = logf(input);
    double ref_val = log((double)input);

    float err = get_ulp_error(ref_val, my_val);

    printf("Input x:           %f\n", input);
    printf("My logf result:    %.9f; hex: %a\n", my_val, my_val);
    printf("GNU libm result:   %.9f; hex: %a\n", (float)ref_val, (float)ref_val);
    printf("ULP Error:         %.4f\n", err);

    if (err > 3.5)
    {
        printf("Bad results (err > 3.5 ULP)\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
#endif