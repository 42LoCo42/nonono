# pwdhack
Remotely change the current working directory of any process!

## Usage
1. Start anything with pwdhack.so as demonstrated in [the test file](test)
2. Send a line with the new PWD to the PWDHACK_PORT: `echo ../foo/bar >/dev/tcp/localhost/12345`
3. If the line starts with /, it is interpreted relative to PWDHACK_ROOT (which can be empty, but must be set!)

## Why?
I have a Plan9 server whose filesystem is mounted in my home directory
and I want filename completion via `rlwrap -c drawterm [...]`.

rlwrap tracks the PWD of its child process,
so I need to programmatically change drawterm's PWD.

There could be other uses for this tool.
