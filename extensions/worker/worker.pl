foreign_resource('worker', [main]).
foreign(main, c, main).

load(X) :- print('Prolog got: '), print(X),nl.
zmq_check(T,[T],['4','5','6','7','8']).


:- load_foreign_resource('worker').
:- main.
