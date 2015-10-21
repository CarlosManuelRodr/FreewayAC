#define BOOST_TEST_MODULE FreewayAC
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../FreewayAC/CellularAutomata.h"

BOOST_AUTO_TEST_SUITE(Auxiliar)

BOOST_AUTO_TEST_CASE(AuxiliarTest)
{
    BOOST_CHECK_EQUAL(aux_string_to_bool("true"), true);
    BOOST_CHECK_EQUAL(aux_string_to_bool("false"), false);
    BOOST_CHECK_EQUAL(aux_get_extension("path/myfile.id.csv"), "csv");
    BOOST_CHECK_EQUAL(aux_replace_all("this is my test string", "test", "result"), "this is my result string");
    BOOST_CHECK_EQUAL(aux_replace_extension("path/myfile.id.csv", "bmp"), "path/myfile.id.bmp");
    BOOST_CHECK_EQUAL(aux_is_in(std::vector<int>({ 1, 6, 8, 10, 89 }), 10), true);
    BOOST_CHECK_EQUAL(aux_is_in(std::vector<int>({ 1, 6, 8, 10, 89 }), 23), false);
    BOOST_CHECK_EQUAL(aux_is_in_const(std::vector<int>({ 1, 6, 8, 10, 89 }), 10), true);
    BOOST_CHECK_EQUAL(aux_is_in_const(std::vector<int>({ 1, 6, 8, 10, 89 }), 23), false);
    BOOST_CHECK_EQUAL(aux_is_in({ 1, 6, 8, 10, 89 }, 10), true);
    BOOST_CHECK_EQUAL(aux_is_in({ 1, 6, 8, 10, 89 }, 23), false);
    BOOST_CHECK_EQUAL(aux_is_in_const({ 1, 6, 8, 10, 89 }, 10), true);
    BOOST_CHECK_EQUAL(aux_is_in_const({ 1, 6, 8, 10, 89 }, 23), false);
    BOOST_CHECK_EQUAL(aux_find_pos(std::vector<int>({ 1, 6, 8, 10, 89 }), 8), 2);
    BOOST_CHECK_EQUAL(aux_find_pos_const(std::vector<int>({ 1, 6, 8, 10, 89 }), 8), 2);
    BOOST_CHECK_EQUAL(aux_string_to_num<int>("42"), 42);
    BOOST_CHECK_EQUAL(aux_string_to_num<double>("42.98"), 42.98);
    BOOST_CHECK_EQUAL(aux_mean(std::vector<int>({ 1, 6, 8, 10, 90 })), 23);
    //BOOST_CHECK_NO_THROW(aux_parallel_function<std::vector<int>, int, int>([](int a, int b) { return std::vector<int>({ a }); }, 0, 0, 0, 0));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(AC)

BOOST_AUTO_TEST_CASE(AcTest)
{
    // Circular.
    std::vector<int> init({ 1, -1, -1, 1, -1, 1, 1, -1, -1, 1, -1, -1, -1, 1, -1, -1, 1, -1, -1, -1 });
    std::vector<bool> rand_val_circ({ false, false, false, false, true, false, true,
        true, false, false, true, false, false, false,
        false, true, false, false, true, false, false });
    CircularCA circ_ca(init, rand_val_circ, 5);
    circ_ca.Evolve(3);
    BOOST_CHECK(circ_ca.GetCa() == std::vector<CaVelocity>({ -1, 2, -1, 1, 0, -1, -1, 1, -1, 1, -1,-1, -1, -1, 2, -1, -1, -1, 2, -1 }));
    
    // Abierto.
    std::vector<bool> rand_val_open_vec({ false, false, false, false, true, false, true, false,
        true, false, false, true, false, false, false, true,
        false, true, false, false, true, false, false, false });
    OpenCA open_ca(init, rand_val_open_vec, 5, 1);
    open_ca.Evolve(3);
    BOOST_CHECK(open_ca.GetCa() == std::vector<CaVelocity>({ -1, 1, 0, -1, -1, 1, -1, 1, 0, -1, -1,-1, -1, -1, -1, 3, -1, -1, 2, -1 }));

    // Autonomo.
    std::vector<int> aut_cars_vec({ 0, 1, 3, 5 });
    std::vector<bool> rand_val_aut_vec({ false, false, false, true, true,
        true, false, true, false, false,
        false, true, false, true, false });
    AutonomousCA autonomous_ca(init, aut_cars_vec, rand_val_aut_vec, 5);
    autonomous_ca.Evolve(3);
    BOOST_CHECK(autonomous_ca.GetCa() == std::vector<CaVelocity>({ -1, -1, -1, -1, 1, 0, -1, 1, -1, -1, -1, -1, 2, -1, -1, 2, -1, 1, -1, 1 }));
}

BOOST_AUTO_TEST_SUITE_END()