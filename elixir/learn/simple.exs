IO.puts("Hello world from Elixir!")

# add = fn a, b -> a + b end
add = &(&1 + &2)
IO.puts("1 + 2 = #{add.(1, 2)}")

double = fn a -> add.(a, a) end
IO.puts("2 * 5 = #{double.(5)}")

list = [42, 43, 44]
IO.puts(hd(list))
IO.inspect(tl(list))

IO.puts(length([1, 2, 3]))
IO.puts(tuple_size({1, 2, 3}))

# foo = {1, 2, 3}
# bar = put_elem(foo, 2, 9)

case File.read("flake.nix") do
  {:ok, data} -> IO.puts("read #{byte_size(data)} bytes of data!")
  {:error, what} -> raise("#{what}")
end

cond do
  2 + 2 == 5 -> IO.puts("math broke")
  2 + 2 == 4 -> IO.puts("all good")
end

IO.puts(String.split("1  2 3", " ", trim: true))

map = %{:a => 1, 2 => :b}
IO.puts(map[:a])
IO.puts(map[2])

map = %{name: "John", age: 23}
IO.puts(map.name)

users = %{
  john: %{name: "John", age: 27, languages: ["Erlang", "Ruby", "Elixir"]},
  mary: %{name: "Mary", age: 29, languages: ["Elixir", "F#", "Clojure"]}
}

users = put_in(users.mary.age, 31)
IO.puts(users.mary.age)

users = update_in(users.mary.languages, fn languages -> List.delete(languages, "Clojure") end)
IO.inspect(users.mary.languages)

defmodule Math do
  @moduledoc """
  Provides math-related functions.

  ## Examples
  iex> Math.sum(1, 2)
  3

  iex> Math.zero?(0)
  true
  """

  @doc """
  Calculates the sum of two numbers
  """
  def sum(a, b), do: do_sum(a, b)

  defp do_sum(a, b), do: a + b

  @doc """
  Checks if a given number is zero
  """
  def zero?(0), do: true

  def zero?(x) when is_integer(x), do: false
end

IO.puts(Math.sum(1, 2))
# IO.puts(Math.do_sum(1, 2)) # UndefinedFunctionError

IO.puts(Math.zero?(0))
IO.puts(Math.zero?(1))
# IO.puts(Math.zero?([1, 2, 3])) # FunctionClauseError

defmodule Concat do
  def join(a, b \\ nil, sep \\ " ")
  def join(a, b, _sep) when is_nil(b), do: a
  def join(a, b, sep), do: a <> sep <> b
end

IO.puts(Concat.join("hello"))
IO.puts(Concat.join("hello", "world"))
IO.puts(Concat.join("hello", "world", "_"))

defmodule DefaultTest do
  def dowork(x \\ "hello"), do: x
end

IO.puts(DefaultTest.dowork())
IO.puts(DefaultTest.dowork(123))

defmodule Recursion do
  def print_multiple_times(msg, n) when n > 0 do
    IO.puts(msg)
    print_multiple_times(msg, n - 1)
  end

  def print_multiple_times(_, _), do: :ok

  def sum_list([h | t], acc) do
    sum_list(t, h + acc)
  end

  def sum_list([], acc), do: acc

  def double_each([h | t]) do
    [h * 2 | double_each(t)]
  end

  def double_each([]), do: []
end

Recursion.print_multiple_times("recursion is fun", 3)
IO.puts(Recursion.sum_list([1, 2, 3], 4))
IO.puts(Enum.reduce([1, 2, 3], 4, &+/2))

odd? = &(rem(&1, 2) != 0)

IO.puts(
  1..100_000
  |> Enum.map(&(&1 * 3))
  |> Enum.filter(odd?)
  |> Enum.sum()
)

IO.puts(
  1..100_000
  |> Stream.map(&(&1 * 3))
  |> Stream.filter(odd?)
  |> Enum.sum()
)

parent = self()

spawn(fn ->
  # IO.puts("child: sleeping for 500 ms")
  # :timer.sleep(500)
  send(parent, {:hello, self()})
end)

receive do
  {:hello, pid} -> IO.puts("Got hello from #{inspect(pid)}")
end

defmodule KV do
  def start_link do
    Task.start_link(fn -> loop(%{}) end)
  end

  defp loop(map) do
    receive do
      {:get, key, caller} ->
        IO.puts("get #{key} for #{inspect(caller)}")
        send(caller, Map.get(map, key))
        loop(map)

      {:put, key, val} ->
        IO.puts("put #{key} = #{val}")
        loop(Map.put(map, key, val))
    end
  end
end

{:ok, pid} = KV.start_link()
Process.register(pid, :kv)

send(:kv, {:put, :hello, :world})
send(:kv, {:get, :hello, self()})

receive do: (val -> IO.puts("got back #{inspect(val)}"))

# reply = String.trim(IO.gets("yes or no? "))
# IO.puts(:stderr, "you said: #{reply}")

{:ok, file} = File.open("data.bin", [:write])
IO.binwrite(file, "héllø wörłd")
File.close(file)

IO.puts(File.read!("data.bin"))

defmodule MyServer do
  @initial_state %{host: "127.0.0.1", port: 37812}
  IO.inspect(@initial_state)

  @my_data 14
  def first_data, do: @my_data

  @my_data 13
  def second_data, do: @my_data
end

IO.puts(MyServer.first_data())
IO.puts(MyServer.second_data())

defmodule Foo do
  Module.register_attribute(__MODULE__, :param, accumulate: true)
  @param :foo
  @param :bar
  IO.inspect(@param)
end

defmodule User do
  defstruct [:email, name: "John", age: 27]
end

defmodule UserTest do
  john = %User{}
  IO.inspect(john)

  jane = %{john | name: "Jane"}
  IO.inspect(jane)
end

defprotocol Utility do
  @spec type(t) :: String.t()
  def type(value)
end

defimpl Utility, for: BitString do
  def type(_), do: "string"
end

defimpl Utility, for: Integer do
  def type(_), do: "integer"
end

IO.puts(Utility.type("foo"))
IO.puts(Utility.type(42))

defprotocol Size do
  @doc "Calculates the size of a data structure (constant time)"
  def size(data)
end

defimpl Size, for: BitString do
  def size(d), do: byte_size(d)
end

defimpl Size, for: Map do
  def size(d), do: map_size(d)
end

defimpl Size, for: Tuple do
  def size(d), do: tuple_size(d)
end

defimpl Size, for: MapSet do
  def size(d), do: MapSet.size(d)
end

IO.puts(Size.size("foo"))
IO.puts(Size.size(%{label: "something"}))
IO.puts(Size.size({:ok, "hello"}))
IO.puts(Size.size(MapSet.new(foo: "bar")))

defimpl Size, for: Any do
  def size(_), do: 0
end

defmodule OtherUser do
  @derive [Size]
  defstruct [:name, :age]
end

defmodule OtherUserTest do
  IO.puts(Size.size(%OtherUser{}))
end

IO.inspect(for n <- 1..4, do: n * n)

values = [good: 1, good: 2, bad: 3, good: 4]
IO.inspect(for {:good, n} <- values, do: n * n)

IO.inspect(for n <- 0..5, rem(n, 3) == 0, do: n * n)

for dir <- ["~/docs", "~/HOST"],
    dir = Path.expand(dir),
    file <- File.ls!(dir),
    path = Path.join(dir, file),
    File.regular?(path),
    size = File.stat!(path).size,
    do: IO.puts("#{path}: #{size}")

# stream = IO.stream(:stdio, :line)
# for line <- stream, into: stream do
#   String.upcase(line)
# end

regex = ~r/foo|bar/i
IO.puts("foo" =~ regex)
IO.puts("BaR" =~ regex)
IO.puts("baz" =~ regex)

IO.puts(~s(This is a string with "double" and 'single' quotes))
IO.inspect(~w(And this is a list of word atoms)a)
IO.puts(~D[2002-11-04])
IO.puts(~T[13:37:42.5])
IO.puts(~U[2002-11-04 13:37:42.5Z])

defmodule MySigils do
  def sigil_i(string, []), do: String.to_integer(string)
  def sigil_i(string, [?n]), do: -sigil_i(string, [])
end

defmodule MySigilsTest do
  import MySigils
  IO.puts(~i(42))
  IO.puts(~i{42}n)
end

defmodule MyError do
  defexception message: "default message"
end

try do
  raise MyError
rescue
  e ->
    require Logger
    Logger.error(Exception.format(:error, e, __STACKTRACE__))
    # reraise e, __STACKTRACE__
end

try do
  Enum.each(-50..50, &if(rem(&1, 13) == 0, do: throw(&1)))
catch
  x -> IO.puts("Got #{x}")
end

IO.inspect(Enum.find(-50..50, &(rem(&1, 13) == 0)))

# spawn_link(fn -> exit(1) end)

try do
  exit(1)
catch
  :exit, e -> IO.puts("fake exit(#{e})")
end

{:ok, file} = File.open("sample", [:utf8, :write])

spawn(fn ->
  try do
    IO.write(file, "olá")
    raise "oops, something went wrong"
  after
    File.close(file)
  end
end)

defmodule RunAfter do
  def without_even_trying do
    raise "oh no"
  after
    IO.puts("cleaning up")
  end
end

spawn(fn -> RunAfter.without_even_trying() end)

x = 2

try do
  1 / x
rescue
  ArithmeticError -> IO.puts("division by zero is impossible!")
else
  x -> IO.puts("result: #{x}")
end

:io.format("Pi is approximately given by:~10.3f~n", [:math.pi()])
IO.puts(Base.encode64(:crypto.hash(:sha256, "Elixir")))

table = :ets.new(:ets_test, [])
:ets.insert(table, {"China", 1_374_000_000})
:ets.insert(table, {"India", 1_284_000_000})
:ets.insert(table, {"USA", 322_000_000})

q = :queue.new()
q = :queue.in("A", q)
q = :queue.in("B", q)
{val, q} = :queue.out(q)
IO.inspect(val)
{val, q} = :queue.out(q)
IO.inspect(val)
_ = q

IO.puts(:rand.uniform())

1..10
|> IO.inspect(label: "input  ")
|> Enum.map(&(&1 * 2))
|> IO.inspect(label: "times 2")
|> Enum.sum()
|> IO.inspect(label: "result ")

IO.inspect(binding())

feature = %{name: :dbg, inspiration: "Rust"}
dbg(feature)
dbg(Map.put(feature, :in_version, "1.14.0"))

__ENV__.file
|> String.split("/", trim: true)
|> List.last()
|> File.exists?()
|> dbg()

defmodule Parser do
  @doc """
  Parses a string.
  """
  @callback parse(String.t()) :: {:ok, term} | {:error, atom}

  @doc """
  Lists all supported file extensions.
  """
  @callback extensions() :: [String.t()]

  @spec parse_path(Path.t(), [module()]) :: {:ok, term} | {:error, atom}
  def parse_path(filename, parsers) do
    with {:ok, ext} <- parse_extension(filename),
         {:ok, parser} <- find_parser(ext, parsers),
         {:ok, contents} <- File.read(filename),
         do: parser.parse(contents)
  end

  defp parse_extension(filename) do
    if ext = Path.extname(filename) do
      {:ok, ext}
    else
      {:error, :no_extension}
    end
  end

  defp find_parser(ext, parsers) do
    if parser = Enum.find(parsers, &(ext in &1.extensions())) do
      {:ok, parser}
    else
      {:error, :no_matching_parser}
    end
  end
end

defmodule JSONParser do
  @behaviour Parser

  @impl Parser
  def parse(str), do: {:ok, "some json" <> str}

  @impl Parser
  def extensions(), do: [".json"]
end

IO.inspect(Parser.parse_path("foo.json", [JSONParser]))
