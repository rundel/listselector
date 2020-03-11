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

test_that("attribute parsing", {
  # No expression
  expect_equal(check_attribute_parser("[x]"),     list(name = "x", expr = ""))
  expect_equal(check_attribute_parser("[.x]"),    list(name = ".x", expr = ""))
  expect_equal(check_attribute_parser("[hello]"), list(name = "hello", expr = ""))

  # Bad name
  expect_error(check_attribute_parser("[]"))
  expect_error(check_attribute_parser("[.1]"))

  # Expression
  expect_equal(check_attribute_parser("[x==1]"),  list(name = "x", expr = "==1"))
  expect_equal(check_attribute_parser("[x == 1]"),  list(name = "x", expr = "== 1"))
  expect_equal(check_attribute_parser("[x != 1]"),  list(name = "x", expr = "!= 1"))
  expect_equal(check_attribute_parser("[x %in% c(1,2,3)]"),  list(name = "x", expr = "%in% c(1,2,3)"))

  check_attribute_parser("[x == y[1]]")
})
