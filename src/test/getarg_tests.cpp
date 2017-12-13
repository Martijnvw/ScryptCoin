#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-sbp");
    BOOST_CHECK(GetBoolArg("-sbp"));
    BOOST_CHECK(GetBoolArg("-sbp", false));
    BOOST_CHECK(GetBoolArg("-sbp", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-sbpo"));
    BOOST_CHECK(!GetBoolArg("-sbpo", false));
    BOOST_CHECK(GetBoolArg("-sbpo", true));

    ResetArgs("-sbp=0");
    BOOST_CHECK(!GetBoolArg("-sbp"));
    BOOST_CHECK(!GetBoolArg("-sbp", false));
    BOOST_CHECK(!GetBoolArg("-sbp", true));

    ResetArgs("-sbp=1");
    BOOST_CHECK(GetBoolArg("-sbp"));
    BOOST_CHECK(GetBoolArg("-sbp", false));
    BOOST_CHECK(GetBoolArg("-sbp", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nosbp");
    BOOST_CHECK(!GetBoolArg("-sbp"));
    BOOST_CHECK(!GetBoolArg("-sbp", false));
    BOOST_CHECK(!GetBoolArg("-sbp", true));

    ResetArgs("-nosbp=1");
    BOOST_CHECK(!GetBoolArg("-sbp"));
    BOOST_CHECK(!GetBoolArg("-sbp", false));
    BOOST_CHECK(!GetBoolArg("-sbp", true));

    ResetArgs("-sbp -nosbp");  // -sbp should win
    BOOST_CHECK(GetBoolArg("-sbp"));
    BOOST_CHECK(GetBoolArg("-sbp", false));
    BOOST_CHECK(GetBoolArg("-sbp", true));

    ResetArgs("-sbp=1 -nosbp=1");  // -sbp should win
    BOOST_CHECK(GetBoolArg("-sbp"));
    BOOST_CHECK(GetBoolArg("-sbp", false));
    BOOST_CHECK(GetBoolArg("-sbp", true));

    ResetArgs("-sbp=0 -nosbp=0");  // -sbp should win
    BOOST_CHECK(!GetBoolArg("-sbp"));
    BOOST_CHECK(!GetBoolArg("-sbp", false));
    BOOST_CHECK(!GetBoolArg("-sbp", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--sbp=1");
    BOOST_CHECK(GetBoolArg("-sbp"));
    BOOST_CHECK(GetBoolArg("-sbp", false));
    BOOST_CHECK(GetBoolArg("-sbp", true));

    ResetArgs("--nosbp=1");
    BOOST_CHECK(!GetBoolArg("-sbp"));
    BOOST_CHECK(!GetBoolArg("-sbp", false));
    BOOST_CHECK(!GetBoolArg("-sbp", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-sbp", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-sbp", "eleven"), "eleven");

    ResetArgs("-sbp -bar");
    BOOST_CHECK_EQUAL(GetArg("-sbp", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-sbp", "eleven"), "");

    ResetArgs("-sbp=");
    BOOST_CHECK_EQUAL(GetArg("-sbp", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-sbp", "eleven"), "");

    ResetArgs("-sbp=11");
    BOOST_CHECK_EQUAL(GetArg("-sbp", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-sbp", "eleven"), "11");

    ResetArgs("-sbp=eleven");
    BOOST_CHECK_EQUAL(GetArg("-sbp", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-sbp", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-sbp", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-sbp", 0), 0);

    ResetArgs("-sbp -bar");
    BOOST_CHECK_EQUAL(GetArg("-sbp", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-sbp=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-sbp", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-sbp=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-sbp", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--sbp");
    BOOST_CHECK_EQUAL(GetBoolArg("-sbp"), true);

    ResetArgs("--sbp=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-sbp", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nosbp");
    BOOST_CHECK(!GetBoolArg("-sbp"));
    BOOST_CHECK(!GetBoolArg("-sbp", true));
    BOOST_CHECK(!GetBoolArg("-sbp", false));

    ResetArgs("-nosbp=1");
    BOOST_CHECK(!GetBoolArg("-sbp"));
    BOOST_CHECK(!GetBoolArg("-sbp", true));
    BOOST_CHECK(!GetBoolArg("-sbp", false));

    ResetArgs("-nosbp=0");
    BOOST_CHECK(GetBoolArg("-sbp"));
    BOOST_CHECK(GetBoolArg("-sbp", true));
    BOOST_CHECK(GetBoolArg("-sbp", false));

    ResetArgs("-sbp --nosbp");
    BOOST_CHECK(GetBoolArg("-sbp"));

    ResetArgs("-nosbp -sbp"); // sbp always wins:
    BOOST_CHECK(GetBoolArg("-sbp"));
}

BOOST_AUTO_TEST_SUITE_END()
