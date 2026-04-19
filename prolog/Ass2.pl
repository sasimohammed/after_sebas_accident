% =============================================================================
% RESCUE ROBOT -- Greedy Best-First Search (Maximize Survivors)
% =============================================================================
%
% HOW TO USE:
%   1. Load the file:
%        ?- consult('rescue_robot.pl').
%   2. Run with any grid directly:
%        ?- grid([[r,e,s],[d,f,e],[e,s,e]]).
%        ?- grid([[r,e,d,e,e],[e,e,f,e,s],[d,e,e,e,d],[e,s,e,f,s]]).
%
% STATE REPRESENTATION:
%   Each open-list node: entry(Priority, Pos, Path, Rescued)
%   - Pos     : current cell (Row, Col), 1-indexed from top-left
%   - Path    : list of all visited cells (newest first); blocks revisits
%   - Rescued : count of 's' cells collected so far along Path
%
% HEURISTIC (Greedy Best-First):
%   Priority = Rescued + RemainingUnvisitedSurvivors
%   The open list is kept sorted DESCENDING by Priority so the most
%   promising node (greedy estimate of total rescuable survivors) is
%   always expanded first.
%   Search stops early when Rescued == total survivors in the grid.
%
% NOTE: Greedy Best-First does not guarantee the global optimum.
% =============================================================================

:- use_module(library(lists)).

% ---------------------------------------------------------------------------
% GRID ACCESSORS  (grid passed as a parameter, not a global fact)
% ---------------------------------------------------------------------------

cell(Grid, R, C, V) :-
    nth1(R, Grid, Row), nth1(C, Row, V).

grid_dims(Grid, Rows, Cols) :-
    length(Grid, Rows),
    Grid = [R0|_], length(R0, Cols).

in_bounds(Grid, R, C) :-
    grid_dims(Grid, MR, MC),
    R >= 1, R =< MR, C >= 1, C =< MC.

passable(Grid, R, C) :-
    cell(Grid, R, C, V), V \= d, V \= f.

is_survivor(Grid, R, C) :-
    cell(Grid, R, C, s).

find_start(Grid, R, C) :-
    nth1(R, Grid, Row), nth1(C, Row, r).

all_survivors(Grid, Ss) :-
    findall((R,C), (nth1(R,Grid,Row), nth1(C,Row,s)), Ss).

total_survivors(Grid, N) :-
    all_survivors(Grid, Ss), length(Ss, N).

% ---------------------------------------------------------------------------
% MOVEMENT
% ---------------------------------------------------------------------------

neighbour(R, C, R1, C) :- R1 is R - 1.
neighbour(R, C, R1, C) :- R1 is R + 1.
neighbour(R, C, R, C1) :- C1 is C - 1.
neighbour(R, C, R, C1) :- C1 is C + 1.

reachable(Grid, R, C, Visited, NR, NC) :-
    neighbour(R, C, NR, NC),
    in_bounds(Grid, NR, NC),
    passable(Grid, NR, NC),
    \+ member((NR,NC), Visited).

% ---------------------------------------------------------------------------
% HEURISTIC
% ---------------------------------------------------------------------------

rem_survivors(Grid, Visited, N) :-
    all_survivors(Grid, All),
    include([X]>>(\+ member(X, Visited)), All, Rem),
    length(Rem, N).

priority(Grid, Visited, Rescued, P) :-
    rem_survivors(Grid, Visited, Rem),
    P is Rescued + Rem.

% ---------------------------------------------------------------------------
% SORTED OPEN LIST  (descending priority)
% ---------------------------------------------------------------------------

ins(E, [], [E]) :- !.
ins(entry(P1,A,B,C), [entry(P2,D,E,F)|T],
    [entry(P1,A,B,C), entry(P2,D,E,F)|T]) :- P1 >= P2, !.
ins(E, [H|T], [H|T2]) :- ins(E, T, T2).

ins_all([], O, O).
ins_all([H|T], O0, O2) :- ins(H, O0, O1), ins_all(T, O1, O2).

% ---------------------------------------------------------------------------
% EXPAND
% ---------------------------------------------------------------------------

expand(Grid, (R,C), Path, Rescued, Succs) :-
    findall(entry(P,(NR,NC),[(NR,NC)|Path],NR2),
            ( reachable(Grid, R, C, Path, NR, NC),
              ( is_survivor(Grid, NR, NC) -> NR2 is Rescued+1 ; NR2 = Rescued ),
              priority(Grid, [(NR,NC)|Path], NR2, P) ),
            Succs).

% ---------------------------------------------------------------------------
% GBFS LOOP
% ---------------------------------------------------------------------------

gbfs(_, _, BR, BP, Total, BR, BP) :- BR >= Total, !.
gbfs(_, [], BR, BP, _, BR, BP)    :- !.
gbfs(Grid, [entry(_,Pos,Path,Rescued)|Rest], BR, BP, Total, FR, FP) :-
    ( Rescued > BR -> BR2 = Rescued, BP2 = Path ; BR2 = BR, BP2 = BP ),
    expand(Grid, Pos, Path, Rescued, Succs),
    ins_all(Succs, Rest, Open2),
    gbfs(Grid, Open2, BR2, BP2, Total, FR, FP).

% ---------------------------------------------------------------------------
% solve(+Grid)   <-- call this from the REPL
% ---------------------------------------------------------------------------

grid(Grid) :-
    find_start(Grid, SR, SC),
    InitPath = [(SR,SC)],
    ( is_survivor(Grid, SR, SC) -> IR = 1 ; IR = 0 ),
    priority(Grid, InitPath, IR, P0),
    total_survivors(Grid, Total),
    gbfs(Grid, [entry(P0,(SR,SC),InitPath,IR)], 0, InitPath, Total,
         Rescued, RevPath),
    reverse(RevPath, Path),
    length(Path, Len), Steps is Len - 1,
    write('Path found: '), print_cells(Path), nl,
    format("Survivors rescued: ~w~n", [Rescued]),
    format("Number of steps: ~w~n",   [Steps]),
    !.

% ---------------------------------------------------------------------------
% PATH PRINTING
% ---------------------------------------------------------------------------

print_cells([]).
print_cells([(R,C)])      :- !, format("(~w,~w)", [R,C]).
print_cells([(R,C)|Rest]) :- format("(~w,~w) -> ", [R,C]), print_cells(Rest).

% ---------------------------------------------------------------------------
% DEMO  -- runs automatically on load, shows both examples
% ---------------------------------------------------------------------------

run_examples :-
    nl,
    write('=== Example 1: 4x5 Grid ==='), nl,
    grid([[r,e,d,e,e],
           [e,e,f,e,s],
           [d,e,e,e,d],
           [e,s,e,f,s]]),
    nl,
    write('=== Example 2: 3x3 Grid ==='), nl,
    grid([[r,e,s],
           [d,f,e],
           [e,s,e]]),
    nl.

:- initialization(run_examples, main).
