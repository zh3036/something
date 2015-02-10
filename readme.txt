Now it is a only select echo server that does not deal with short count.

there are sevearl works:

0. create the fd set for select 
  keep the track of max index of fd list
                    max fd number

1. establish the initial listen socket
  add it to the select set

for every while loop

2. select on the fd set

3. after the return of select

3.1 if listen port is ready, it means a client connect,
  make a new socket add it to fd set
  update the releveant max fd and max index of fd list

3.2 if some normal ports is ready, then it means clients
  sent msgs, so go through the ready clients
  recv the msg andWrite back
  using normal IO does not care about short count



