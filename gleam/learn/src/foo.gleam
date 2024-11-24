import gleam/io
import gleam/string

pub fn main() {
  io.println("Hello from foo!")

  let x = "Joe"
  let x: String = "Hello, " <> x <> "!"
  io.println(x)

  let value: Bool = {
    io.println("Hello")
    io.debug(42 + 12)
    False
  }

  io.debug(value)

  let x = [2, 3]
  let y = [1, ..x]
  io.debug(y)

  let tup = #(1, 2, 3)
  let foo = tup.2
  io.debug(foo)

  let pk = <<42, 13, 37>>
  io.debug(pk)

  case Error("oh no") {
    Ok(foo) -> io.debug(foo)
    Error(msg) -> io.println_error(msg)
  }

  io.debug(twice)

  io.println(replace(in: "A,B,C", what: ",", with: ""))
}

fn twice(f: fn(t) -> t, x: t) -> t {
  f(f(x))
}

fn replace(
  // this is cringe tbh, why not use the param names directly?
  in string: String,
  what pattern: String,
  with replacement: String,
) -> String {
  string.replace(string, pattern, replacement)
}
