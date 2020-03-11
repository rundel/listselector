// [[Rcpp::plugins(cpp14)]]
// [[Rcpp::depends(BH)]]

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>

#include <Rcpp.h>

// [[Rcpp::export]]
bool test(std::string const& str) {
  namespace x3 = boost::spirit::x3;
  using x3::ascii::char_;

  auto first = str.begin();
  auto last = str.end();


  int n;
  struct _n{};

  auto init_bracket = [](auto& ctx) { x3::get<_n>(ctx).get() = 0; };

  auto open_bracket = [](auto& ctx){
    //Rcpp::Rcout << "open\n";
    //i++;
    int& i = x3::get<_n>(ctx).get();
    i++;
  };

  auto close_bracket = [](auto& ctx){
    //Rcpp::Rcout << "close\n";

    int& i = x3::get<_n>(ctx).get();
    if (i-1 < 0) {
      x3::_pass(ctx) = false;
    } else {
      i--;
    }
  };

  auto check_attr = [](auto& ctx){
    int& i = x3::get<_n>(ctx).get();
    Rcpp::Rcout << "Attr: " << x3::_attr(ctx)
                << "; i = "  << i << "\n";
  };

  auto expr = x3::with<_n>(std::ref(n)) [(
    '[' >>
      x3::eps[init_bracket] >>
      x3::raw[ x3::lexeme[
        *(
            char_('[')[open_bracket]  |
            char_(']')[close_bracket] |
            (char_ - char_("[]"))
        )
      ] ] >>
    ']'
  )];

  Rcpp::Rcout << "n = " << n << "\n";

  std::vector<std::string> res;
  bool r = phrase_parse(first, last,
                        +expr,
                        x3::ascii::space,
                        res);

  Rcpp::Rcout << "r = " << (int) r << "; first != last = " << (int) (first != last) << "\n";
  Rcpp::Rcout << "n = " << n << "\n";
  Rcpp::Rcout << "res = ";
  for(auto const a : res) {
    Rcpp::Rcout << a << "\n";
  }
  Rcpp::Rcout << "\n";

  if (!r || first != last) // fail if we did not get a full match
    return false;

  return true;
}

/*** R
test("[]")
test("[abc]")
test("[a[]bc]")
test("[x[y[1]] + x[[1]]]")
test("[x[[1]]")
*/
