% Utils
lenlist([], 0).
lenlist([_|T], L):- lenlist(T, L2), L is L2+1.

count([], _, 0).
count([Elem|T], Elem, C):- count(T, Elem, C2), C is C2+1.
count([Other|T], Elem, C):- Other \= Elem, count(T, Elem, C).

% Automaton processor
process([], State, Result):- end(State) -> Result = true; Result = false.
process([H|T], State, Result):- rule(State, H, NewState), process(T, NewState, Result).

collect(Word, ResList, State):- start(State), findall(Result, process(Word, State, Result), ResList).

% Output
process(Word):- collect(Word, ResList, State), lenlist(ResList, ResCount), count(ResList, true, TrueCount),
    format("~d of ~d paths (~25f) accepted for initial state ~p~n", [TrueCount, ResCount, ResCount rdiv TrueCount, State]),
    fail. % fail to immediately continue output
