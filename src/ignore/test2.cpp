#include <Rcpp.h>

// [[Rcpp::plugins(cpp14)]]
// [[Rcpp::depends(BH)]]

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <iostream>
#include <string>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

namespace client { namespace ast {
  namespace x3 = boost::spirit::x3;

  struct attribute{
    std::string name, op, value;
  };

  struct simple_selector {
    std::string combinator, name;
    std::vector<attribute> attrs;
  };

  using boost::fusion::operator<<;
} }

BOOST_FUSION_ADAPT_STRUCT(
  client::ast::attribute,
  name, op, value
)

BOOST_FUSION_ADAPT_STRUCT(
  client::ast::simple_selector,
  combinator, name, attrs
)

namespace parser {
  using x3::char_;

  struct combinator_ : x3::symbols<std::string> {
    combinator_() {
      add
      ("+", "+")
      (">", ">")
      ("~", "~");
    }
  } combinator;

  struct attr_op_ : x3::symbols<std::string> {
    attr_op_() {
      add
      //("=",  "==")
      ("==", "==")
      ("!=", "!=")
      (">",  ">")
      ("<",  "<")
      (">=", ">=")
      ("<=", "<=");
    }
  } attr_op;

  x3::rule<class name, std::string> const name = "name";
  x3::rule<class attribute, client::ast::attribute> const attribute = "attribute";
  x3::rule<class simple_selector, client::ast::simple_selector> const simple_selector = "simple_selector";

  auto const name_def = x3::raw[ x3::lexeme[ (
    ( char_("A-Za-z") > *char_("._A-Za-z0-9") ) |
    ( char_(".") > !char_("0-9") > *char_("._A-Za-z0-9") )
  ) ] ];

  auto const attribute_def = (
    '[' >
      name >
      attr_op >
      +(char_ - ']') >
    ']'
  );

  auto const simple_selector_def = (
    -combinator >>
    name >>
    *attribute
  );

  auto const selector = +simple_selector;

  BOOST_SPIRIT_DEFINE(name, attribute, simple_selector);
}



// [[Rcpp::export]]
bool test(std::string str) {

  auto first = str.begin();
  auto last = str.end();

  std::vector<client::ast::simple_selector> ast;

  bool r = x3::phrase_parse(
    first, last,
    parser::selector,
    ascii::space,
    ast
  );

  if (first != last) // fail if we did not get a full match
    return false;

  for(auto const& s : ast) {
    if (!s.combinator.empty())
      Rcpp::Rcout << "combinator: " << s.combinator << "\n";

    Rcpp::Rcout << "name: " << s.name << "\n";

    for(auto const& x : s.attrs) {
      Rcpp::Rcout << "Attributes:\n"
                  << "  name: " << x.name
                  << "  op: " << x.op
                  << "  value: " << x.value << "\n";
    }
  }

  Rcpp::Rcout << "\n";

  return r;
}
