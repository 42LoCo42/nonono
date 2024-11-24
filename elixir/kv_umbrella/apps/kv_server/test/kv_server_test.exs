defmodule KVServerTest do
  use ExUnit.Case

  setup do
    Application.stop(:kv)
    :ok = Application.start(:kv)
  end

  setup do
    opts = [:binary, packet: :line, active: false]
    {:ok, socket} = :gen_tcp.connect(~c"localhost", 37812, opts)
    %{socket: socket}
  end

  test "server interaction", %{socket: socket} do
    assert send_and_recv(socket, "UNKNOWN shopping\n") == "UNKNOWN COMMAND\n"
    assert send_and_recv(socket, "GET shopping eggs\n") == "NOT FOUND\n"
    assert send_and_recv(socket, "CREATE shopping\n") == "OK\n"
    assert send_and_recv(socket, "PUT shopping eggs 3\n") == "OK\n"

    assert send_and_recv(socket, "GET shopping eggs\n") == "3\n"
    assert send_and_recv(socket, "") == "OK\n"

    assert send_and_recv(socket, "DELETE shopping eggs\n") == "OK\n"

    assert send_and_recv(socket, "GET shopping eggs\n") == "\n"
    assert send_and_recv(socket, "") == "OK\n"
  end

  defp send_and_recv(socket, command) do
    :ok = :gen_tcp.send(socket, command)
    {:ok, data} = :gen_tcp.recv(socket, 0, 1000)
    data
  end
end
