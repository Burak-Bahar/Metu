:- module(main, [salad_type/2, person_can_eat/3, possible_order_options/3, count_per_type/2]).
:- [kb].

% DO NOT CHANGE THE UPPER CONTENT, WRITE YOUR CODE AFTER THIS LINE


salad_type([], Type).
salad_type([Item|T], Type):- not(not_vegan(Item)), Type = 'vegan' , salad_type(T, Type).
salad_type([Item|T], Type):- not(not_vegetarian(Item)), Type = 'vegetarian', salad_type(T, Type).
salad_type([Item|T], Type):- Type = 'omnivorous', salad_type(T, Type).


p_can_eat(Person, H, Salad):- vegan(Person), vegan_p(H, Salad).
p_can_eat(Person, H, Salad):- vegetarian(Person), vegetarian_p(H, Salad).
p_can_eat(Person, H, Salad):- omnivorous(Person), omni_p(H, Salad).

vegan_p([], []).
vegan_p([Item|T], [Item|Tt]):- not(not_vegan(Item)), vegan_p(T, Tt).

vegetarian_p([], []).
vegetarian_p([Item|T],  [Item|Tt]):- not(not_vegetarian(Item)), vegetarian_p(T, Tt).

omni_p([], []).
omni_p([Item|T],  [Item|Tt]):- omni_p(T, Tt).

person_can_eat(Person, [H|T], K):- p_can_eat(Person, H, K).
person_can_eat(Person, [H|T], K):- person_can_eat(Person, T , K).


orders(Person, H, ItemList):- vegan(Person), v_p(H, ItemList).
orders(Person, H, ItemList):- vegetarian(Person), vg_p(H, ItemList).
orders(Person, H, ItemList):- omnivorous(Person), o_p(H, ItemList).

n_in( A, []).
n_in( A, [H|T]):- A \= H, n_in( A, T).

v_p([], []).
v_p([H|T], [A|Rem]):- v_p(T, Rem), H = 'b', base(A), not(not_vegan(A)), n_in( A, Rem) . 
v_p([H|T], [A|Rem]):- v_p(T, Rem), H = 'p', protein(A), not(not_vegan(A)), n_in(A, Rem).
v_p([H|T], [A|Rem]):- v_p(T, Rem), H = 't', topping(A), not(not_vegan(A)) , n_in(A, Rem).
v_p([H|T], [A|Rem]):- v_p(T, Rem), H = 'c', condiment(A), not(not_vegan(A)), n_in(A, Rem) .
v_p([H|T], [A|Rem]):- v_p(T, Rem), H = 's', sauce(A), not(not_vegan(A)) , n_in(A, Rem).

vg_p([], []).
vg_p([H|T], [Ss|Rem]):- vg_p(T, Rem), H = 'b', base(Ss), not(not_vegetarian(Ss)), n_in(Ss, Rem).
vg_p([H|T], [Ss|Rem]):- vg_p(T, Rem), H = 'p', protein(Ss),not(not_vegetarian(Ss)), n_in(Ss, Rem) .
vg_p([H|T], [Ss|Rem]):- vg_p(T, Rem), H = 't', topping(Ss), not(not_vegetarian(Ss)), n_in(Ss, Rem).
vg_p([H|T], [Ss|Rem]):- vg_p(T, Rem), H = 'c', condiment(Ss), not(not_vegetarian(Ss)) , n_in(Ss, Rem) .
vg_p([H|T], [Ss|Rem]):- vg_p(T, Rem), H = 's', sauce(Ss), not(not_vegetarian(Ss)), n_in(Ss, Rem)  .

o_p([], []).
o_p([H|T], [Ss|Rem]):- o_p(T, Rem), H = 'b', base(Ss), n_in(Ss, Rem).
o_p([H|T], [Ss|Rem]):- o_p(T, Rem), H = 'p', protein(Ss), n_in(Ss, Rem) .
o_p([H|T], [Ss|Rem]):- o_p(T, Rem), H = 't', topping(Ss), n_in(Ss, Rem) .
o_p([H|T], [Ss|Rem]):- o_p(T, Rem), H = 'c', condiment(Ss), n_in(Ss, Rem).
o_p([H|T], [Ss|Rem]):- o_p(T, Rem), H = 's', sauce(Ss), n_in(Ss, Rem).

possible_order_options(Person, Order, ItemList):- atom_chars(Order, X), orders(Person, X, ItemList).

saladtype([], [],  V, Vg, O).


count_per_type([], []).
count_per_type([Ing|T], Typel):- saladtype(Ing, Typel, V, Vg, O), count_per_type(T, Typel).























