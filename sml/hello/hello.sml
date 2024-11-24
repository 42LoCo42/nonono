print "Hello, World!\n";

val x = fn x => x + 1

fun fac n =
  if n = 0 then 1 else n * fac (n - 1)

fun fac' 0 = 1
  | fac' n =
      n * fac' (n - 1)

type id = string

datatype binop = Plus | Minus | Times | Div

datatype stm =
  CompoundStm of stm * stm
| AssignStm of id * stm
| PrintStm of exp list
and exp =
  IdExp of id
| NumExp of int
| OpExp of exp * binop * exp
| EseqExp of stm * exp
