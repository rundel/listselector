test_that("name parsing", {
  # Valid object names
  expect_equal(check_name_parser('a'),           'a')
  expect_equal(check_name_parser('xyz'),         'xyz')
  expect_equal(check_name_parser('hello_world'), 'hello_world')
  expect_equal(check_name_parser('hello.world'), 'hello.world')
  expect_equal(check_name_parser('.'),           '.')
  expect_equal(check_name_parser('..1'),         '..1')

  # Invalid object names
  expect_error(check_name_parser('.1'))
  expect_error(check_name_parser('1'))
  expect_error(check_name_parser('123'))

  # Quoted object names
  expect_equal(check_name_parser('`abc`'),            '`abc`')
  expect_equal(check_name_parser('`hello world`'),    '`hello world`')
  expect_equal(check_name_parser('`hello \n world`'), '`hello \n world`')
  expect_equal(check_name_parser('`!@#$%^&*`'),       '`!@#$%^&*`')
  expect_equal(check_name_parser('`.1`'),             '`.1`')
  expect_equal(check_name_parser('`..1`'),            '`..1`')

  # Empty quoted name
  expect_error(check_name_parser('``'))
})

test_that("expression parsing", {
  # Typical
  expect_equal(check_expr_parser("x"), "x")
  expect_equal(check_expr_parser("== x"), "== x")
  expect_equal(check_expr_parser("!= x"), "!= x")
  expect_equal(check_expr_parser("%in% c(1,2,3)"), "%in% c(1,2,3)")
  expect_equal(check_expr_parser("< c(1,2,3)"), "< c(1,2,3)")

  # Good Brackets
  expect_equal(check_expr_parser("x[1]"), "x[1]")
  expect_equal(check_expr_parser("x[[1]]"), "x[[1]]")
  expect_equal(check_expr_parser("x[y[1]]"), "x[y[1]]")
  expect_equal(check_expr_parser("x[1] + y[1]"), "x[1] + y[1]")

  # Non-matching Brackets
  expect_error(check_expr_parser("x[1"))
  expect_error(check_expr_parser("x1]"))
  expect_error(check_expr_parser("x[1]]"))
  expect_error(check_expr_parser("x[[1]"))
  expect_error(check_expr_parser("x[1 + y[1]"))
  expect_error(check_expr_parser("x1] + y[1]"))
  expect_error(check_expr_parser("x[1] + y[1"))
  expect_error(check_expr_parser("x[1] + y1]"))
})



test_that("attribute parsing", {
  # No expression
  expect_equal(check_attribute_parser("[x]"),     list(name = "x", expr = ""))
  expect_equal(check_attribute_parser("[.x]"),    list(name = ".x", expr = ""))
  expect_equal(check_attribute_parser("[hello]"), list(name = "hello", expr = ""))

  # Bad name
  expect_error(check_attribute_parser("[]"))
  expect_error(check_attribute_parser("[.1]"))

  # Expression
  expect_equal(check_attribute_parser("[x==1]"),             list(name = "x", expr = "==1"))
  expect_equal(check_attribute_parser("[x == 1]"),           list(name = "x", expr = "== 1"))
  expect_equal(check_attribute_parser("[x != 1]"),           list(name = "x", expr = "!= 1"))
  expect_equal(check_attribute_parser("[x %in% c(1,2,3)]"),  list(name = "x", expr = "%in% c(1,2,3)"))

  # Expressions with brackets
  expect_equal(check_attribute_parser("[x == y[1]]"),        list(name = "x", expr = "== y[1]"))
  expect_equal(check_attribute_parser("[x != y[[1]]]"),      list(name = "x", expr = "!= y[[1]]"))
  expect_equal(check_attribute_parser("[x >  y[1] + z[1]]"), list(name = "x", expr = ">  y[1] + z[1]"))
  expect_equal(check_attribute_parser("[x >= y + z[1]]"),    list(name = "x", expr = ">= y + z[1]"))

  # Expressions with non-matching brackets
  expect_error(check_attribute_parser("[ x != y[[1] ]"))
  expect_error(check_attribute_parser("[ x >  y[1 + z[1] ]"))
  expect_error(check_attribute_parser("[ x >  y[1] + z] ]"))
})
