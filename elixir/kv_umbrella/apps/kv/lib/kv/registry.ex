defmodule KV.Registry do
  use GenServer

  ## Server API

  @impl true
  def init(table) do
    names = :ets.new(table, [:named_table, read_concurrency: true])
    refs = %{}
    {:ok, {names, refs}}
  end

  @impl true
  def handle_call({:create, name}, _from, state) do
    {names, refs} = state

    case lookup(names, name) do
      {:ok, bucket} ->
        {:reply, bucket, state}

      :error ->
        {:ok, bucket} = DynamicSupervisor.start_child(KV.BucketSupervisor, KV.Bucket)
        ref = Process.monitor(bucket)
        refs = Map.put(refs, ref, name)
        :ets.insert(names, {name, bucket})
        {:reply, bucket, {names, refs}}
    end
  end

  @impl true
  def handle_info({:DOWN, ref, :process, _pid, _reason}, {names, refs}) do
    {name, refs} = Map.pop(refs, ref)
    :ets.delete(names, name)
    {:noreply, {names, refs}}
  end

  @impl true
  def handle_info(msg, state) do
    require Logger
    Logger.debug("Unexpected message in #{__MODULE__}: #{inspect(msg)}")
    {:noreply, state}
  end

  ## Client API

  @doc """
  Starts the registry with the given options.

  `:name` is always required.
  """
  def start_link(opts) do
    server = Keyword.fetch!(opts, :name)
    GenServer.start_link(__MODULE__, server, opts)
  end

  @doc """
  Looks up the bucket PID for `names` stored in `registry`.

  Returns `{:ok, pid}` if the bucket exists, `:error` otherwise.
  """
  def lookup(registry, name) do
    case :ets.lookup(registry, name) do
      [{^name, bucket}] -> {:ok, bucket}
      [] -> :error
    end
  end

  @doc """
  Ensures there is a bucket associated with the given `name` in `registry`.
  """
  def create(registry, name) do
    GenServer.call(registry, {:create, name})
  end
end
