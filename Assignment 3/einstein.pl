% company(Company).
%   @param  Solution is a list of members that satisfy all constraints.

/* Company logical puzzle: 

Rose, Lily, Frank and Harry are four members of a famous company. They occupy different
positions within the company: an intern, a manager, a director, and a CEO. Each of them
reports to the its immediate superior, i.e. the intern reports to the manager, the manager
reports to the director, and the director reports to the CEO. 
Each of them sits on a different floor of a skyscraper: 
three, five, nine and twelve. 
We can use the following constants
Rose; Lily; Frank; Harry; 3 ; 5 ; 9 ; 12 ; intern; manager; director;CEO;

functions
Floor(x); Position(x);

and predicate
ReportsTo(x; y)
meaning that x reports to y. We know the following facts about them.

S1: ReportsTo(x, y) ^ Floor(y) = 3 => ReportsTo(Rose, x)
S2: Position(Frank) = manager
S3: ReportsTo(x, y) ^ Floor(x) = 12 => Floor(y) = 3
S4: Floor(Harry) = 9
S5: ReportsTo(Frank, Lily)
S6: Floor(x) = 3 => ReportsTo(x, Harry)
S7: ReportsTo(x, Lily) => Floor(x) = 12
S8: \negPosition(x) = Manager OR \negReportsTo(x, y) OR Position(y) = Director
S9: \negPosition(x) = Manager OR \negReportsTo(y, x) OR Position(y) = Intern
S10: \negPosition(x) = Director OR \negReportsTo(x, y) OR Position(y) = CEO
S11: Floor(Rose) = 5

Solve the puzzle
*/

% Render the members term as a nice table.
:- use_rendering(table,
		 [header(person('Name', 'Position', 'Floor'))]).

company(Com) :-
	% each member in the list Com of company is represented as:
	%      person(Name, Position, Floor)
	length(Com, 4),     
    
    % S1
    reportsto(X1, Y1, Com), floor(three, Y1,Com), reportsto(person(rose,_,_), X1, Com),
    % S2
    member(person(frank,manager,_), Com),
    % S3
    reportsto(X3, Y3, Com), floor(twelve, X3,Com), floor(three, Y3,Com),
    % S4
    member(person(harry,_,nine), Com),
    % S5
	reportsto(person(frank,_,_), person(lily,_,_), Com),
    % S6
    floor(three, X6,Com), reportsto(X6, person(harry,_,_), Com),
    % S7
    reportsto(X7, person(lily,_,_), Com), floor(twelve, X7,Com),
    % S8
    position(manager, X8, Com), reportsto(X8, Y8, Com), position(director, Y8, Com),
    % S9
    position(manager, X9, Com), reportsto(Y9, X9, Com), position(intern, Y9, Com),
    % S10
    position(director, X10, Com), reportsto(X10, Y10, Com), position(ceo, Y10, Com),
    % S11
    member(person(rose,_,five), Com).

reportsto(X, Y, Ls) :-
    member(X, Ls),
    member(Y, Ls),
    (
    (X = person(_, intern, _), Y = person(_, manager, _));
    (X = person(_, manager, _), Y = person(_, director, _));
    (X = person(_, director, _), Y = person(_, ceo, _))
    ).

floor(M, X, Ls) :-
    member(X, Ls),
    X = person(_, _, M).

position(N, X, Ls) :-
    member(X,Ls),
    X=person(_,N,_).

/** <examples>
?- company(Com).
*/

