import strutils
import random

proc reverse*(s: string): string =
  for i in countdown(s.high, 0):
    result.add s[i]

echo reverse("foo")

proc readInt*: int =
  try:
    stdout.write "Input a positive integer: "
    result = stdin.readLine.parseInt
  except ValueError:
    raise newException(ValueError, "You must enter a positive integer!")

proc fac*(n: int): int =
  result = 1
  for i in 2 .. n:
    result *= i

# echo fac(readInt())

const foo: array[4 .. 6, int] = [6, 1, 7]
for ix, el in foo:
  echo el, " at ", ix

var my_seq = @[3, 1, 9, 6]
my_seq.add(7)
echo my_seq

proc `*`(a, b: openArray[int]): seq[int] =
  newSeq(result, a.len)
  for i in 0 ..< a.len:
    result[i] = a[i] * b[i]

echo [2, 3, 4] * [5, 6, 7]

type
  BinaryTree*[T] = ref object
    data: T
    le, ri: BinaryTree[T]

proc newNode*[T](data: T): BinaryTree[T] =
  new(result)
  result.data = data

proc add*[T](root: var BinaryTree[T], n: BinaryTree[T]) =
  var it = root.addr
  while it[] != nil:
    if cmp(n.data, it.data) < 0:
      it = it.le.addr
    else:
      it = it.ri.addr
  it[] = n

proc add*[T](root: var BinaryTree[T], data: T) =
  root.add(newNode(data))

proc `$`*[T](root: BinaryTree[T]): string =
  if root == nil:
    "null"
  else:
    "{\"data\": $1, \"le\": $2, \"ri\": $3}" % [$root.data, $root.le, $root.ri]

iterator flatten*[T](root: BinaryTree[T]): T =
  var stack = @[root]

  while stack.len > 0:
    var n = stack.pop()
    if n.le != nil:
      discard
    yield n.data
    if n.ri != nil:
      discard

random.randomize()
var root: BinaryTree[int]
# for i in 1 .. 20:
#  root.add(random.rand(1000))
root.add(5)
root.add(3)

echo root.repr
for i in root.flatten():
  echo i
