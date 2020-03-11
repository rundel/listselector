#include <Rcpp.h>

// [[Rcpp::plugins(cpp14)]]
// [[Rcpp::depends(BH)]]

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <iostream>
#include <string>

namespace client
{
  namespace x3 = boost::spirit::x3;
  namespace ascii = boost::spirit::x3::ascii;

  using x3::double_;
  using ascii::space;
  using x3::_attr;

  ///////////////////////////////////////////////////////////////////////////
  //  Our adder
  ///////////////////////////////////////////////////////////////////////////

  template <typename Iterator>
  bool adder(Iterator first, Iterator last, double& n)
  {
    auto assign = [&](auto& ctx){ n = _attr(ctx); };
    auto add = [&](auto& ctx){ n += _attr(ctx); };

    bool r = x3::phrase_parse(first, last,

                              //  Begin grammar
                              (
                                  double_[assign] >> *(',' >> double_[add])
                              )
                                ,
                                //  End grammar

                                space);

    if (first != last) // fail if we did not get a full match
      return false;
    return r;
  }
}

// [[Rcpp::export]]
double add(std::string str) {

  double n;
  if (client::adder(str.begin(), str.end(), n)) {
    Rcpp::Rcout << "-------------------------\n";
    Rcpp::Rcout << "Parsing succeeded\n";
    Rcpp::Rcout << str << " Parses OK: " << "\n";

    Rcpp::Rcout << "sum = " << n;
    Rcpp::Rcout << "\n-------------------------\n";
    return n;
  } else {
    Rcpp::Rcout << "-------------------------\n";
    Rcpp::Rcout << "Parsing failed\n";
    Rcpp::Rcout << "-------------------------\n";
    return NA_REAL;
  }
}
