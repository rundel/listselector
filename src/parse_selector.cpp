// [[Rcpp::plugins(cpp14)]]
// [[Rcpp::depends(BH)]]

#include "parser.hpp"

// [[Rcpp::export]]
std::string check_name_parser(std::string const& str) {
  namespace x3 = boost::spirit::x3;

  auto first = str.begin();
  auto last = str.end();

  std::string name;

  bool r = x3::phrase_parse(
    first, last,
    client::parser::name,
    x3::ascii::space,
    name
  );

  if (first != last || !r) // fail if we did not get a full match
    Rcpp::stop("Failed to parse.");

  return name;
}

// [[Rcpp::export]]
Rcpp::List check_attribute_parser(std::string const& str) {
  namespace x3 = boost::spirit::x3;

  auto first = str.begin();
  auto last = str.end();

  client::ast::attribute attr;

  bool r = x3::phrase_parse(
    first, last,
    client::parser::attribute,
    x3::ascii::space,
    attr
  );

  if (first != last || !r) // fail if we did not get a full match
    Rcpp::stop("Failed to parse.");

  return Rcpp::wrap(attr);
}



// [[Rcpp::export]]
Rcpp::List check_simple_selector_parser(std::string const& str) {
  namespace x3 = boost::spirit::x3;

  auto first = str.begin();
  auto last = str.end();

  client::ast::simple_selector s;

  bool r = x3::phrase_parse(
    first, last,
    client::parser::simple_selector,
    x3::ascii::space,
    s
  );

  if (first != last || !r) // fail if we did not get a full match
    Rcpp::stop("Failed to parse.");

  return Rcpp::wrap(s);
}





//// [[Rcpp::export]]
//void parse(std::string const& input) {
//  namespace x3 = boost::spirit::x3;
//
//
//  std::vector<client::ast::simple_selector> ast;
//
//  auto iter = input.begin();
//  auto const end = input.end();
//
//  using boost::spirit::x3::with;
//  using boost::spirit::x3::error_handler_tag;
//
//  // Our error handler
//  using error_handler_type = x3::error_handler<std::string::const_iterator>;
//  error_handler_type error_handler(iter, end, std::cerr);
//
//  // Our parser
//  using client::parser::selectors;
//  auto const parser =
//    // we pass our error handler to the parser so we can access
//    // it later in our on_error and on_sucess handlers
//    x3::with<x3::error_handler_tag>(std::ref(error_handler))[selectors];
//
//  bool r = phrase_parse(iter, end, parser, x3::ascii::space, ast);
//
//  if (r && iter == end)
//  {
//    Rcpp::Rcout << boost::fusion::tuple_open('[');
//    Rcpp::Rcout << boost::fusion::tuple_close(']');
//    Rcpp::Rcout << boost::fusion::tuple_delimiter(", ");
//
//    Rcpp::Rcout << "-------------------------\n";
//    Rcpp::Rcout << "Parsing succeeded\n";
//
//    for (auto const& emp : ast)
//    {
//      Rcpp::Rcout << "got: " << emp << std::endl;
//    }
//    Rcpp::Rcout << "\n-------------------------\n";
//
//  }
//  else
//  {
//    Rcpp::Rcout << "-------------------------\n";
//    Rcpp::Rcout << "Parsing failed\n";
//    Rcpp::Rcout << "-------------------------\n";
//    ast.clear();
//  }
//}
