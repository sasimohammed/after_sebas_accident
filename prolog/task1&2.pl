%%%%%%%%%%%%%%%%%%%%%%%%%%%% Students Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%
student(ali, first).
student(sara, second).
student(omar, third).
student(mona, second).
student(yousef, first).
student(nour, fourth).
student(karim, third).

%%%%%%%%%%%%%%%%%%%%%%%%%%%% Books Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%
book(prolog_fundamentals, dr_hassan).
book(recursion_in_depth, dr_sara).
book(list_programming, dr_ahmed).
book(ai_intro, dr_mona).

%%%%%%%%%%%%%%%%%%%%%%%%%%%% Borrow Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%
borrowed(ali, prolog_fundamentals).
borrowed(ali, list_programming).
borrowed(sara, recursion_in_depth).
borrowed(sara, ai_intro).
borrowed(omar, recursion_in_depth).
borrowed(mona, prolog_fundamentals).
borrowed(mona, recursion_in_depth).
borrowed(mona, list_programming).
borrowed(yousef, list_programming).
borrowed(nour, ai_intro).
borrowed(karim, recursion_in_depth).

%%%%%%%%%%%%%%%%%%%%%%%%%%%% Topics Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%
topics(prolog_fundamentals, [facts, rules, queries, unification]).
topics(recursion_in_depth, [base_case, recursive_case, tracing, termination]).
topics(list_programming, [head_tail, member, append, length, prefix, suffix]).
topics(ai_intro, [search, logic, knowledge_representation]).

%%%%%%%%%%%%%%%%%%%%%%%%%%%% Ratings Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%
rating(ali, prolog_fundamentals, 85).
rating(ali, list_programming, 90).
rating(sara, recursion_in_depth, 95).
rating(sara, ai_intro, 88).
rating(omar, recursion_in_depth, 80).
rating(mona, prolog_fundamentals, 92).
rating(mona, recursion_in_depth, 89).
rating(mona, list_programming, 91).
rating(yousef, list_programming, 60).
rating(nour, ai_intro, 78).
rating(karim, recursion_in_depth, 83).



%%%%%%%%%%%%%%%%%%%%%%%%%%%% Task 1 :Get a list of all books borrowed by a specific student %%%%%%%%%%%%%%%%%%%%%%%%%%%%

books_borrowed_by_student(Student, Books) :-
    books_borrowed_by_student(Student, [], Books).

books_borrowed_by_student(Student, Acc, Books) :-
    borrowed(Student, Book),
    \+ member(Book, Acc),
    books_borrowed_by_student(Student, [Book|Acc], Books).

books_borrowed_by_student(Student, Acc, Books) :-
    borrowed(Student, Book),
    member(Book, Acc),
    books_borrowed_by_student(Student, Acc, Books).

member(X, [X|_]).
member(X, [_|T]) :- member(X, T).


%%%%%%%%%%%%%%%%%%%%%%%%%%%% Task 2 :Count how many students borrowed a specific book %%%%%%%%%%%%%%%%%%%%%%%%%%%%

borrowers_count(Book, N) :-
    borrowers_count(Book, [], N).

borrowers_count(Book, Acc, N) :-
    borrowed(Student, Book),
    \+ member(Student, Acc),
    borrowers_count(Book, [Student|Acc], RestCount),
    N is RestCount + 1.

borrowers_count(Book, Acc, N) :-
    borrowed(Student, Book),
    member(Student, Acc),
    borrowers_count(Book, Acc, N).

member(X, [X|_]).
member(X, [_|T]) :- member(X, T).








%%%%%%%%%%%%%%%%%%%%%%%%%%%% Task 5: Top Reviewer %%%%%%%%%%%%%%%%%%%%%%%%%%%%
top_reviewer(Student) :-
    rating(Student, _, Score),
    \+ (rating(_, _, OtherScore), OtherScore > Score), !.

%%%%%%%%%%%%%%%%%%%%%%%%%%%% Task 6: Most Common Topic %%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Main predicate
most_common_topic_for_student(Student, Topic) :-
    % Get all books first, then get their topics
    get_all_books(Student, Books),
    get_all_topics_from_books(Books, AllTopics),
    AllTopics \= [],
    most_frequent(AllTopics, Topic).

% Get all unique books borrowed by student
get_all_books(Student, Books) :-
    get_books_helper(Student, [], Books), !.

get_books_helper(Student, Acc, Books) :-
    borrowed(Student, Book),
    \+ member_book(Book, Acc), !,
    get_books_helper(Student, [Book|Acc], Books).
get_books_helper(_, Books, Books) :- !.

% Member check for books
member_book(X, [X|_]) :- !.
member_book(X, [_|T]) :-
    member_book(X, T), !.

% Get all topics from a list of books
get_all_topics_from_books([], []) :- !.
get_all_topics_from_books([Book|Rest], AllTopics) :-
    topics(Book, Topics),
    get_all_topics_from_books(Rest, RestTopics),
    append_lists(Topics, RestTopics, AllTopics), !.

% Simple append
append_lists([], L, L) :- !.
append_lists([H|T], L, [H|R]) :-
    append_lists(T, L, R), !.

% Find most frequent topic
most_frequent(List, Topic) :-
    get_unique(List, Unique), !,
    find_best(List, Unique, 0, none, Topic), !.

% Get unique elements
get_unique([], []) :- !.
get_unique([H|T], [H|Unique]) :-
    \+ member_topic(H, T), !,
    get_unique(T, Unique).
get_unique([_|T], Unique) :-
    get_unique(T, Unique), !.

% Member check for topics
member_topic(X, [X|_]) :- !.
member_topic(X, [_|T]) :-
    member_topic(X, T), !.

% Find best topic by comparing counts
find_best(_, [], _, Best, Best) :- !.
find_best(List, [H|T], MaxCount, CurrentBest, BestTopic) :-
    count(H, List, Count), !,
    (Count > MaxCount ->
        find_best(List, T, Count, H, BestTopic)
    ;
        find_best(List, T, MaxCount, CurrentBest, BestTopic)
    ), !.

% Count occurrences
count(_, [], 0) :- !.
count(X, [X|T], N) :-
    count(X, T, N1), !,
    N is N1 + 1.
count(X, [_|T], N) :-
    count(X, T, N), !.