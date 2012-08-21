foreign_resource('master', [main]).
foreign(main, c, main).

:- load_foreign_resource('master').
:- main.
