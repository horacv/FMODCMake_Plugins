#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "libs/doctest.h"

static doctest::Approx approx_relative(const double expected, const double scale = 1, const double epsilon = 1e-6)
{
	return doctest::Approx(expected).scale(scale).epsilon(epsilon);
}