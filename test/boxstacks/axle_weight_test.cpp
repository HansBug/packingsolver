#include "packingsolver/boxstacks/instance_builder.hpp"
#include "packingsolver/boxstacks/optimize.hpp"

#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

using namespace packingsolver::boxstacks;
namespace fs = boost::filesystem;

struct BoxStacksAxleWeightTestParams
{
    fs::path items_path;
    fs::path bins_path;
    fs::path parameters_path;

    /** Minimum number of items the returned solution must pack. */
    packingsolver::ItemPos minimum_number_of_items;
};

inline std::ostream& operator<<(std::ostream& os, const BoxStacksAxleWeightTestParams& test_params)
{
    os << test_params.items_path;
    return os;
}

class BoxStacksAxleWeightTest: public testing::TestWithParam<BoxStacksAxleWeightTestParams> { };

TEST_P(BoxStacksAxleWeightTest, BoxStacksAxleWeight)
{
    BoxStacksAxleWeightTestParams test_params = GetParam();
    InstanceBuilder instance_builder;
    instance_builder.read_item_types(test_params.items_path.string());
    instance_builder.read_bin_types(test_params.bins_path.string());
    instance_builder.read_parameters(test_params.parameters_path.string());
    Instance instance = instance_builder.build();

    OptimizeParameters optimize_parameters;
    optimize_parameters.optimization_mode = packingsolver::OptimizationMode::NotAnytimeSequential;
    Output output = optimize(instance, optimize_parameters);

    // These instances enter the axle weight repair loop of
    // 'sequential_onedimensional_rectangle', which used to read
    // 'fixed_items_solutions' one element past the end and throw.
    // The bound is a minimum rather than an exact count, so that the test
    // does not have to be updated when the repair packs more items.
    EXPECT_GE(
            output.solution_pool.best().number_of_items(),
            test_params.minimum_number_of_items);
}

INSTANTIATE_TEST_SUITE_P(
        BoxStacksAxleWeight,
        BoxStacksAxleWeightTest,
        testing::ValuesIn(std::vector<BoxStacksAxleWeightTestParams>{
            {
                // The repair returns an empty solution for this instance; the
                // point of the case is that 'optimize' returns at all.
                fs::path("data") / "boxstacks" / "tests" / "semi_trailer_truck_middle_axle_bin_packing" / "items.csv",
                fs::path("data") / "boxstacks" / "tests" / "semi_trailer_truck_middle_axle_bin_packing" / "bins.csv",
                fs::path("data") / "boxstacks" / "tests" / "semi_trailer_truck_middle_axle_bin_packing" / "parameters.csv",
                0,
            }, {
                fs::path("data") / "boxstacks" / "tests" / "semi_trailer_truck_middle_axle_knapsack" / "items.csv",
                fs::path("data") / "boxstacks" / "tests" / "semi_trailer_truck_middle_axle_knapsack" / "bins.csv",
                fs::path("data") / "boxstacks" / "tests" / "semi_trailer_truck_middle_axle_knapsack" / "parameters.csv",
                1,
            }}));
