test = list(
  list(
    name = list(
      first = "Bob",
      last = "Smith",
      age = list("blah","ugh")
    ),
    address = "123 Main St",
    age = 39
  ),
  list(
    name = list(
      first = "Shirley",
      last = "Doe"
    ),
    address = "456 1st St",
    age = 46
  )
)

# Examples:
# name => function(node) node[["name]]
# name>first => function(node) node[["name"]][["first"]]
# name+phone => function(node) node[c("name", "phone")]
# name[age=46] => function(node) node[["name"]][["age"]] == 46


filter = local({
  res = list()

  filter_step = function(l, selector, recurse, pos) {
    val = selector(l)

    if (!is.null(val)) {
      res[[paste(pos, collapse="::")]] <<- val
      if (!recurse)
        return()
    }

    l = purrr::keep(l, is.list)

    if (length(names(l)) == length(l))
      new_step = names(l)
    else
      new_step = seq_along(l)

    purrr::map2(
      l, new_step,
      ~ filter_step(.x, selector, recurse, pos = c(pos, .y))
    )
  }

  function(l, selector, recurse = TRUE) {
    res <<- list()
    filter_step(l, selector, recurse, pos = c())
    res
  }
})


#
#
# str(filter(test, function(node) node[["name"]]))
# str(filter(test, function(node) node[["last"]]))
# str(filter(test, function(node) node[["age"]]))
#
# res <- jsonlite::fromJSON('http://ergast.com/api/f1/2019/results.json?limit=500', simplifyVector = FALSE)
#
# system.time(
#   str(filter(res, function(node) node[["raceName"]]))
# )
#
# system.time(
#   str(filter(res, function(node) node[["raceName"]], recurse = FALSE))
# )
#
#
#
# system.time(
#   str(filter(res, function(node) node[["familyName"]]))
# )
#
# system.time(
#   str(filter(res, function(node) node[["familyName"]], recurse = FALSE))
# )
#
