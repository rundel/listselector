#ifndef PARSER_HPP
#define PARSER_HPP

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

namespace client { namespace ast {
  namespace x3 = boost::spirit::x3;

  struct attribute : x3::position_tagged {
    std::string name, expr;
  };

  struct simple_selector : x3::position_tagged {
    std::string combinator, name;
    std::vector<attribute> attrs;
  };
} }

BOOST_FUSION_ADAPT_STRUCT(
  client::ast::attribute,
  name, expr
)

BOOST_FUSION_ADAPT_STRUCT(
  client::ast::simple_selector,
  combinator, name, attrs
)


#include <RcppCommon.h>

namespace Rcpp {
  template <> SEXP wrap(client::ast::attribute const& attr);
  template <> SEXP wrap(std::vector<client::ast::attribute> const& attrs);
  template <> SEXP wrap(client::ast::simple_selector const& sel);
}

#include <Rcpp.h>

namespace Rcpp {
  template <> SEXP wrap(client::ast::attribute const& attr){
    return Rcpp::List::create(
      Rcpp::Named("name") = attr.name,
      Rcpp::Named("expr") = attr.expr
    );
  };

  template <> SEXP wrap(std::vector<client::ast::attribute> const& attrs){
    return Rcpp::List(attrs.begin(), attrs.end());
  };

  template <> SEXP wrap(client::ast::simple_selector const& sel){
    return Rcpp::List::create(
      Rcpp::Named("name") = sel.name,
      Rcpp::Named("combinator") = sel.combinator,
      Rcpp::Named("attrs") = Rcpp::wrap(sel.attrs)
    );
  };
}



namespace client { namespace parser {

  namespace x3 = boost::spirit::x3;
  using x3::ascii::char_;

  //struct error_handler {
  //  template <typename Iterator, typename Exception, typename Context>
  //  x3::error_handler_result on_error(
  //      Iterator& first, Iterator const& last,
  //      Exception const& x, Context const& context)
  //  {
  //    auto& error_handler = x3::get<x3::error_handler_tag>(context).get();
  //    std::string message = "Error! Expecting: " + x.which() + " here:";
  //    error_handler(x.where(), message);
  //    return x3::error_handler_result::fail;
  //  }
  //};

  struct combinator_ : x3::symbols<std::string> {
    combinator_() {
      add
      ("+", "+")
      (">", ">")
      ("~", "~");
    }
  } combinator;


  struct name_class;
  struct expr_class;
  struct attribute_class;
  struct simple_selector_class;

  x3::rule<name_class, std::string> const name = "name";
  x3::rule<expr_class, std::string> const expr = "expr";
  x3::rule<attribute_class, ast::attribute> const attribute = "attribute";
  x3::rule<simple_selector_class, ast::simple_selector> const simple_selector = "employee";


  int n_br_open;
  struct _n_br_open{};

  auto init_bracket = [](auto& ctx) {
    //Rcpp::Rcout << "init\n";
    x3::get<_n_br_open>(ctx).get() = 0;
  };
  auto open_bracket = [](auto& ctx) {
    //Rcpp::Rcout << "open\n";
    x3::get<_n_br_open>(ctx).get()++;
  };
  auto close_bracket = [](auto& ctx) {
    //Rcpp::Rcout << "close\n";
    int& i = x3::get<_n_br_open>(ctx).get();
    if (i-1 < 0) {
      x3::_pass(ctx) = false;
    } else {
      i--;
    }
  };
  auto end_bracket = [](auto& ctx) {
    //Rcpp::Rcout << "end\n";
    if (x3::get<_n_br_open>(ctx).get() != 0) {
      x3::_pass(ctx) = false;
    }
  };

  // This needs to be complicated here because we want to allow our
  // expressions to contain brackets.
  //
  // Solution based on https://stackoverflow.com/questions/33624149/boost-spirit-x3-cannot-compile-repeat-directive-with-variable-factor/33627991#33627991
  // %= weirdness is from https://stackoverflow.com/questions/33929849/boost-spirit-x3-ast-not-working-with-semantic-actions-when-using-separate-rule-d/33937139#33937139
  //
  auto expr_def = x3::rule<struct expr_def, std::string> {}
               %= x3::with<_n_br_open>(std::ref(n_br_open)) [(
                  x3::eps[init_bracket]  >>
                  x3::raw[ x3::lexeme[
                    *(
                      char_('[')[open_bracket]  |
                      char_(']')[close_bracket] |
                      (char_ - char_("[]"))
                    )
                  ] ] >>
                  x3::eps[end_bracket]
                  )];

  // A syntactically valid name consists of letters, numbers and the dot or underline
  // characters and starts with a letter or the dot not followed by a number. Names such as
  // ".2way" are not valid, and neither are the reserved words.
  auto const name_def = x3::raw[ x3::lexeme[ (
    ( char_("A-Za-z") >> *char_("._A-Za-z0-9") ) |
      ( char_(".") >> !char_("0-9") >> *char_("._A-Za-z0-9") ) |
      ( '`' >> +(char_ - '`') >> '`' )
  ) ] ];

  auto const attribute_def = (
    '[' >>
      name >>
      expr >>
    ']'
  );

  auto const simple_selector_def = (
    -combinator >>
      name >>
      *(attribute)
  );

  //auto const selectors = +simple_selector;

  BOOST_SPIRIT_DEFINE(name, expr, attribute, simple_selector);

  struct name_class {};
  struct expr_class {};
  struct attribute_class {};//: x3::annotate_on_success {};
  struct simple_selector_class {};// : x3::annotate_on_success {};
} }

#endif
