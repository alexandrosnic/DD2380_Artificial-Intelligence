%%  Company(-Solution)
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

reportsto(X, Y, Ls) :-
    member(X, Ls),
    member(Y, Ls),
    (
    (X = person(_, intern, _), Y = person(_, manager, _));
    (X = person(_, manager, _), Y = person(_, director, _));
    (X = person(_, director, _), Y = person(_, ceo, _))
    ).

floor(M, X, Ls) :-
    member(X,Ls),
    X=person(_,_,M).

position(N, X, Ls) :-
    member(X,Ls),
    X=person(_,N,_).


company(Com) :-
	% each member in the list Com of company is represented as:
	%      person(Name, Position, Floor)
	length(Com, 3),       
    % S1
    reportsto(xS1, yS1, Com), floor(3, yS1, Com), reportsto(person(rose, _, _), xS1, Com),
    % S2
	member(person(frank,manager,_), Com),
    % S3
    reportsto(xS3, yS3, Com), floor(12, xS3, Com), floor(3, yS3, Com),
    % S4
	member(person(harry,_,9), Com),
    % S5
	reportsto(person(frank,_,_), person(lily,_,_), Com),
    % S6
    floor(3, xS6, Com), reportsto(xS6, person(harry, _, _), Com),
    % S7
    reportsto(xS7, person(lily, _, _), Com), floor(12, xS7, Com),
    % S8
    position(manager, xS8, Com), reportsto(xS8, yS8, Com), position(director, yS8, Com),
    % S9
    position(manager, xS9, Com), reportsto(yS9, xS9, Com), position(intern,yS9, Com),
    % S10
    position(director, xS10, Com), reportsto(xS10, yS10, Com), position(ceo,yS10, Com),
    % S11
	member(person(rose,_,5), Com).


