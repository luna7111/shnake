# Shnake

This program is a built-in command for my minishell project. 
It's a simple command line implementation of the arcade game Snake but complying with the limited set of allowed functions by the project instructions.

Here are some perks i had to make:

## Random numbers

The apple position goes to a random place in the grid whenever the snake eats it, to generate pseudo-random numbers without using an already existing funcion like rand() i made a function that takes a static int and makes a bunch of arbitrary operations with some of the game variables.
To avoid deterministic behaviour one of those variables is a pointer allocates using malloc, which should be totally unpredictable to de user.

## Wait function

I hate this one. There wasn't any time/wait/sleep functions like usleep() allowed, so the only way i had to make the program "wait" was using busy wait. So the function just takes a really big number and iterates it to keep the processor bussy.
Compilers REALLY want to optimize stuff so i used a volatile variable to avoid the compiler omiting the "unnecesary" loop.
The problem with this one is the wait time depends too much on the speed of the processor, cache hits and that kind of stuff, so the snake will go too fast on some computers and painfully slow on others.
I'm still trying to figure out some solution to this problem.