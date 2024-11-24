defmodule KVServer.Command do
  @doc ~S"""
  Parses the given `line` into a command.

  ## Examples

  iex> KVServer.Command.parse("CREATE shopping\n")
  {:ok, {:create, "shopping"}}

  iex> KVServer.Command.parse("CREATE    shopping   \n")
  {:ok, {:create, "shopping"}}

  iex> KVServer.Command.parse("PUT shopping milk 1\n")
  {:ok, {:put, "shopping", "milk", "1"}}

  iex> KVServer.Command.parse("GET shopping milk\n")
  {:ok, {:get, "shopping", "milk"}}

  iex> KVServer.Command.parse("DELETE shopping milk\n")
  {:ok, {:delete, "shopping", "milk"}}

  Unknown commands or commands with the wrong number of arguments return an error:

  iex> KVServer.Command.parse("UNKNOWN shopping milk\n")
  {:error, :unknown_command}

  iex> KVServer.Command.parse("GET shopping\n")
  {:error, :unknown_command}

  """
  def parse(line) do
    case String.split(line) do
      ["CREATE", bucket] -> {:ok, {:create, bucket}}
      ["PUT", bucket, key, val] -> {:ok, {:put, bucket, key, val}}
      ["GET", bucket, key] -> {:ok, {:get, bucket, key}}
      ["DELETE", bucket, key] -> {:ok, {:delete, bucket, key}}
      _ -> {:error, :unknown_command}
    end
  end

  @doc """
  Runs the given command.
  """
  def run(command)

  def run({:create, bucket}) do
    KV.Registry.create(KV.Registry, bucket)
    {:ok, "OK\n"}
  end

  def run({:get, bucket, key}) do
    lookup(bucket, fn pid ->
      val = KV.Bucket.get(pid, key)
      {:ok, "#{val}\nOK\n"}
    end)
  end

  def run({:put, bucket, key, val}) do
    lookup(bucket, fn pid ->
      KV.Bucket.put(pid, key, val)
      {:ok, "OK\n"}
    end)
  end

  def run({:delete, bucket, key}) do
    lookup(bucket, fn pid ->
      KV.Bucket.delete(pid, key)
      {:ok, "OK\n"}
    end)
  end

  defp lookup(bucket, callback) do
    case KV.Registry.lookup(KV.Registry, bucket) do
      {:ok, pid} -> callback.(pid)
      :error -> {:error, :not_found}
    end
  end
end
