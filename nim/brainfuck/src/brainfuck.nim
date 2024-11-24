import std/[os, re, ropes, sequtils, strutils, sugar]

proc groupBy*[T](s: openarray[T], pred: proc(x, y: T): bool): seq[seq[T]] =
  if s.len < 1: return @[]
  result = @[@[s[0]]]
  for n in s[1..s.high]:
    if pred(n, result[result.high][0]):
      result[result.high].add n
    else:
      result.add @[n]

proc group*[T](s: openarray[T]): seq[seq[T]] =
  groupBy[T](proc(x, y: T): bool = x == y, s)

proc compile*(rawCode: string): Rope =
  let code = rawCode
    .replace(re"[^+\-<>.,\[\]]")
    .groupBy(proc(x, y: char): bool = x == y and x notin "[]")
    .map((g) => (g[0], g.len))

  var indentLevel = 0

  template add(expr: string) = result.add repeat(' ', indentLevel) & expr & '\n'

  add """
import std/[sequtils, strutils]
var
  tape = @[0.uint]
  tapePos = 0
proc nextTapePos(skip: int = 1) =
  inc tapePos, skip
  if tapePos >= tape.len:
    tape.add repeat(0.uint, skip)
"""

  # add statements
  for (op, len) in code:
    case op
    of '+': add "inc tape[tapePos], " & $len
    of '-': add "dec tape[tapePos], " & $len
    of '>': add "nextTapePos($1)" % [$len]
    of '<': add "dec tapePos, " & $len
    of '.': add "stdout.write tape[tapePos].chr.repeat($1)" % [$len]
    of ',': add "for i in 0 ..< $1: tape[tapePos] = stdin.readChar.uint" % [$len]
    of '[':
      add "while tape[tapePos] != 0:"
      inc indentLevel
    of ']': dec indentLevel
    else: discard

  # for debugging
  add "echo tape"

proc compileFile*(fileName: string) =
  let nimFile = changeFileExt(fileName, "nim")
  writeFile nimFile, $compile(readFile fileName)
  discard execShellCmd("nim c -d:release " & nimFile)

when isMainModule:
  if paramCount() < 1:
    echo compile(stdin.readAll)
  else:
    for f in commandLineParams():
      compileFile(f)
