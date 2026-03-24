
%%%%%%%%%%%%%%%%%%% HELPER PREDICATE %%%%%%%%%%%%%%%%%%%
member(X, [X|_]).
member(X, [_|T]) :- member(X, T).

%%%%%%%%%%%%%%%%%%% TASK 1 %%%%%%%%%%%%%%%%%%%
books_borrowed_by_student(Student, Books) :-
    collect_books(Student, [], Books).

collect_books(Student, Acc, Books) :-
    borrowed(Student, Book),
    \+ member(Book, Acc), !,
    collect_books(Student, [Book|Acc], Books).
collect_books(_, Books, Books).

%%%%%%%%%%%%%%%%%%% TASK 2 %%%%%%%%%%%%%%%%%%%
borrowers_count(Book, N) :-
    count_students(Book, [], 0, N).

count_students(Book, Acc, CountSoFar, N) :-
    borrowed(Student, Book),
    \+ member(Student, Acc), !,
    NewCount is CountSoFar + 1,
    count_students(Book, [Student|Acc], NewCount, N).

count_students(_, _, N, N). 

%%%%%%%%%%%%%%%%%%% TASK 3 %%%%%%%%%%%%%%%%%%%
most_borrowed_book(B) :-
    book(B,_),
    count_borrowed(B,Count),
    not((book(Other,_),
     Other \= B, count_borrowed(Other, OtherCount),
      OtherCount > Count)),!.

count_borrowed(Book, Count) :-
    count_borrowed(Book,[],0,Count).  

count_borrowed(Book,Seen,Acc,Count) :-
    borrowed(Student,Book),
    \+ member(borrowed(Student,Book),Seen),
    NewAcc is Acc + 1,
    count_borrowed(Book,[borrowed(Student, Book)|Seen],NewAcc,Count). 

count_borrowed(_,_,Count,Count).

%%%%%%%%%%%%%%%%%%% TASK 4 %%%%%%%%%%%%%%%%%%%
ratings_of_book(Book, Result):-
    ratings_of_book(Book, [], Result).

ratings_of_book(Book,Acc,Result):-
    rating(Student,Book,Score),
    \+ member((Student,Score),Acc),
    ratings_of_book(Book,[(Student,Score)|Acc],Result),!.

ratings_of_book(_,Result,Result).

%%%%%%%%%%%%%%%%%%% TASK 5 %%%%%%%%%%%%%%%%%%%
top_reviewer(Student) :-
    rating(Student, _, Score),
    \+ (rating(_, _, OtherScore), OtherScore > Score), !.

%%%%%%%%%%%%%%%%%%% TASK 6 %%%%%%%%%%%%%%%%%%%
most_common_topic_for_student(Student, Topic) :-
    get_all_books(Student, Books),
    get_all_topics_from_books(Books, AllTopics),
    AllTopics \= [],
    most_frequent(AllTopics, Topic).

get_all_books(Student, Books) :-
    get_books_helper(Student, [], Books), !.

get_books_helper(Student, Acc, Books) :-
    borrowed(Student, Book),
    \+ member(Book, Acc), !,
    get_books_helper(Student, [Book|Acc], Books).
get_books_helper(_, Books, Books) :- !.

get_all_topics_from_books([], []) :- !.
get_all_topics_from_books([Book|Rest], AllTopics) :-
    topics(Book, Topics),
    get_all_topics_from_books(Rest, RestTopics),
    append_lists(Topics, RestTopics, AllTopics), !.

append_lists([], L, L) :- !.
append_lists([H|T], L, [H|R]) :-
    append_lists(T, L, R), !.

most_frequent(List, Topic) :-
    get_unique(List, Unique), !,
    find_best(List, Unique, 0, none, Topic), !.

get_unique([], []) :- !.
get_unique([H|T], [H|Unique]) :-
    \+ member(H, T), !,
    get_unique(T, Unique).
get_unique([_|T], Unique) :-
    get_unique(T, Unique), !.

find_best(_, [], _, Best, Best) :- !.
find_best(List, [H|T], MaxCount, CurrentBest, BestTopic) :-
    count(H, List, Count), !,
    (Count > MaxCount ->
        find_best(List, T, Count, H, BestTopic)
    ;
        find_best(List, T, MaxCount, CurrentBest, BestTopic)
    ), !.

count(_, [], 0) :- !.
count(X, [X|T], N) :-
    count(X, T, N1), !,
    N is N1 + 1.
count(X, [_|T], N) :-
    count(X, T, N), !.
