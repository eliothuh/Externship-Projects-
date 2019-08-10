# surreal_eyelid-interpreter
interpreter created for surreal_eyelid

## Bonus work
For Bonus, we have implemented a garbage collection. We create a function called mark in talloc.c. This function can mark the 
memory location that we still need to use so that they don't get cleaned up when we free. 
Everytime tfree() is called, it will print out the current length of the active linkedlist in talloc. Then it will print out the length
of the active linkedlist again after everything that is unmarked has been cleaned up. 

### Demo for Bonus work
We have two demos.
<br /> The first demo is that we mark the tokenized tree. We can see that when we call tfree() the first time, some values is not freed.
<br />However, it is hard to see from this demo that the number of Values being freed is correct. So we have a second demo where we 
create a linkedlist of length 4. We can see that the pointer that is marked is not cleaned up. We can also see from the length
of the active linkedlist that the number of Values being freed is corrent. 
