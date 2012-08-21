foreign_resource('worker', [main]).
foreign(main, c, main).

load(X) :- print('Prolog got: '), print(X),nl.

:- load_foreign_resource('worker').
:- main.
