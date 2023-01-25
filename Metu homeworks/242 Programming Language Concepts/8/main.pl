:- module(main, [is_movie_directed_by/2, total_awards_nominated/2, all_movies_directed_by/2, total_movies_released_in/3, all_movies_released_between/4]).
:- [kb].

% DO NOT CHANGE THE UPPER CONTENT, WRITE YOUR CODE AFTER THIS LINE



is_movie_directed_by(T,D):- movie(T,D,_,_,_,_).

total_awards_nominated(T , N):- movie(T,_,_,O,E,G), N is O+E+G.

all_movies_directed_by(D, H):- findall(H, movie(H,D,_,_,_,_), H).

tot_mov([], Y, Acc, Acc).
tot_mov([H|T], Y, Acc, C):- movie(H,_,Y,_,_,_), Z is Acc+1 ,tot_mov(T, Y , Z, C).
tot_mov([H|T], Y, Acc, C):- movie(H,_,K,_,_,_),K =\= Y, tot_mov(T, Y , Acc, C).

total_movies_released_in(H, Y, C):- tot_mov(H, Y, 0, C).

all_movies_released_between([],_,_,[]).
all_movies_released_between([H|T] , Min , Max , [H|Res]] ):- movie(H,_,K,_,_,_), K => Min, K =< Max, all_movies_released_between(T , Min , Max , Res).
all_movies_released_between([H|T] , Min , Max , Res ):- movie(H,_,K,_,_,_), K < Min, all_movies_released_between(T , Min , Max , Res).
all_movies_released_between([H|T] , Min , Max , Res ):- movie(H,_,K,_,_,_), K > Max, all_movies_released_between(T , Min , Max , Res).




