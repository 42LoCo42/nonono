defmodule KV.Bucket do
  use Agent, restart: :temporary

  @doc """
  Starts a new bucket.
  """
  def start_link(_opts), do: Agent.start_link(fn -> %{} end)

  @doc """
  Gets the value for a given `key` from the `bucket`.
  """
  def get(bucket, key), do: Agent.get(bucket, &Map.get(&1, key))

  @doc """
  Puts a `value` for the given `key` into the `bucket`.
  """
  def put(bucket, key, value), do: Agent.update(bucket, &Map.put(&1, key, value))

  @doc """
  Deletes `key` from `bucket`.
  """
  def delete(bucket, key), do: Agent.get_and_update(bucket, &Map.pop(&1, key))
end
