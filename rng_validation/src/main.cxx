#include "rng_validation.hxx"

#include <fstream>
#include <iostream>
#include <random>
#include <vector>

int main()
{
    const size_t NumRuns    = 100;
    const size_t SampleSize = 10000;

    std::ofstream csvFile("rng_validation.csv");
    if (!csvFile.is_open())
    {
        std::cerr << "Failed to open output file\n";
        return EXIT_FAILURE;
    }

    csvFile << "Generator,Run,Chi2,KS,RunsZScore,AutoCorrDev\n";

    std::mt19937_64 mtGen(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    rng::WeylGenerator weylGen(0.123);

    std::cout << "Starting tests: " << NumRuns << " runs of " << SampleSize << " elements each\n";

    for (size_t run = 0; run < NumRuns; ++run)
    {
        std::vector<double> mtSample(SampleSize);
        std::vector<uint32_t> mtBits(SampleSize);

        std::vector<double> weylSample(SampleSize);
        std::vector<uint32_t> weylBits(SampleSize);

        for (size_t i = 0; i < SampleSize; ++i)
        {
            mtSample[i] = dist(mtGen);
            mtBits[i]   = static_cast<uint32_t>(mtGen());

            weylSample[i] = weylGen.nextDouble();
            weylBits[i]   = weylGen();
        }

        csvFile << "MT19937," << run << "," << rng::chiSquaredTest(mtSample) << "," << rng::ksTest(mtSample) << ","
                << rng::runsTest(mtSample) << "," << rng::bitwiseAutocorrelation(mtBits) << "\n";

        csvFile << "WeylSequence," << run << "," << rng::chiSquaredTest(weylSample) << "," << rng::ksTest(weylSample) << ","
                << rng::runsTest(weylSample) << "," << rng::bitwiseAutocorrelation(weylBits) << "\n";
    }

    return EXIT_SUCCESS;
}