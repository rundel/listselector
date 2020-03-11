#include <Rcpp.h>

// [[Rcpp::plugins(cpp14)]]
// [[Rcpp::depends(BH)]]

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <iostream>
#include <string>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;

// [[Rcpp::export]]
bool test(std::string str) {

  auto first = str.begin();
  auto last = str.end();

  std::string res;

  bool r = x3::phrase_parse(
    first, last,
    x3::raw[ x3::lexeme[ (
        ( char_("A-Za-z") > *char_("._A-Za-z0-9") ) |
        ( char_(".") > !char_("0-9") > *char_("._A-Za-z0-9") )
    ) ] ],
    ascii::space,
    res
  );

  if (first != last) // fail if we did not get a full match
    return false;

  Rcpp::Rcout << "Result: " << res << "\n";

  return r;
}
