def add1 (n: Nat): Nat := n + 1

#eval add1 7
#check id
#check (id)
#eval 1 + 2 * 5
#eval String.append "Hello, " "World!"
#check (1 - 2 : Int)

def isZero (n : Nat) : Bool :=
  match n with
  | Nat.zero   => true
  | Nat.succ _ => false

#eval isZero 0
#eval isZero 1

def isEven (n : Nat) : Bool :=
  match n with
  | Nat.zero   => true
  | Nat.succ k => not (isEven k)

#eval isEven 4
#eval isEven 5

structure PPoint (α : Type) where
  x : α
  y : α
deriving Repr

def onBoth (f : α -> β) (p : PPoint α) : PPoint β :=
  { x := f p.x, y := f p.y }

#eval onBoth (λ x => x * 2) { x := 1, y := 2 }
#eval List.map repr [1, 2, 3]

inductive Sign where
  | pos
  | neg
deriving Repr

def posOrNeg3 (s : Sign)
  :  match s with
  | Sign.pos => Nat
  | Sign.neg => Int
  := match s with
  | Sign.pos =>  3
  | Sign.neg => -3

#check (posOrNeg3 Sign.pos)
#check (posOrNeg3 Sign.neg)

#eval List.length (α := String) ["abc", "def"]
#eval ([].head?.getD 0)
#eval ([].headD 0 : Int)
#eval ([].head! : Nat)

#eval (("five", 5) : String × Int)
#eval (Sum.inl 42 : Int ⊕ String)
#eval ((Sum.inl 42 : Int ⊕ Empty).getLeft?)


def List.last? (xs : List α) : Option α :=
  match xs with
  | []     => none
  | [x]    => some x
  | _ :: t => last? t

#eval ([] : List Nat).last?
#eval [1].last?
#eval [1, 2, 3].last?

def List.findFirst? (p : α -> Bool) (xs : List α) : Option α :=
  match xs with
  | [] => none
  | h :: t => if p h then some h else t.findFirst? p

#eval [1, 3, 3, 7].findFirst? (· > 5)
#eval [1, 3, 3, 7].findFirst? (· > 9)

def Prod.swap (p : α × β) : β × α :=
  { fst := p.snd, snd := p.fst }

#eval ("five", 5).swap

def zipWith (f : x -> y -> z) (xs : List x) (ys : List y) : List z :=
  match xs, ys with
  | xh :: xt, yh :: yt => f xh yh :: zipWith f xt yt
  | _, _ => []

#eval zipWith (repr · ++ ·) [1, 2, 3] ["one", "two"]
