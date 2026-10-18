#include "libs/doctest.h"

#include "tests_main.cpp"
#include "ans_dsp_common.hpp"

TEST_SUITE("DSP Common tests" * doctest::description("DSP common tests")) {
	TEST_CASE("Conversions: db to linear") {
		SUBCASE("Known anchors. (db -> lin)")
		{
			CHECK(ans_dsp::db_to_linear(-120) == approx_relative(1e-6));
			CHECK(ans_dsp::db_to_linear(-20) == approx_relative(0.1));
			CHECK(ans_dsp::db_to_linear(-3.0103) == approx_relative(1 / std::sqrt(2)));
			CHECK(ans_dsp::db_to_linear(-6.0206) == approx_relative(0.5));
			CHECK(ans_dsp::db_to_linear(-60) == approx_relative(0.001));
			CHECK(ans_dsp::db_to_linear(0) == approx_relative(1));
			CHECK(ans_dsp::db_to_linear(20) == approx_relative(10));
			CHECK(ans_dsp::db_to_linear(3.0103) == approx_relative(std::sqrt(2)));
			CHECK(ans_dsp::db_to_linear(40) == approx_relative(100));
			CHECK(ans_dsp::db_to_linear(6.0206) == approx_relative(2));
		}
		SUBCASE("Roundtrip (db -> lin == lin -> db) - Sweep between -120 db to +24 db in steps of 0.5 dB")
		{
			float current = -120;
			float max = 24;
			while (current <= max)
			{
				CHECK(ans_dsp::linear_to_db(ans_dsp::db_to_linear(current)) == approx_relative(current));
				current += 0.5f;
			}
		}
		SUBCASE("Monotonicity Ascending (db -> lin). Sweep between -120 db to +24 db in steps of 0.5 dB."
		  "The next step must produce a value higher than the previous.")
		{
			float current = -120;
			float max = 24;
			while (current < max)
			{
				constexpr float increment = 0.5f;
				CHECK(ans_dsp::db_to_linear(current + increment) > ans_dsp::db_to_linear(current));
				current += increment;
			}
		}
		SUBCASE("Monotonicity Descending (db -> lin). Sweep between -120 db to +24 db in steps of 0.5 dB."
		  "The next step must produce a value lower than the previous.")
		{
			float current = 24;
			float min = -120;
			while (current > min)
			{
				constexpr float decrement = 0.5f;
				CHECK(ans_dsp::db_to_linear(current - decrement) < ans_dsp::db_to_linear(current));
				current -= decrement;
			}
		}
	}
}