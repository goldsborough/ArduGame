# The ArduGame Libary

The ArduGame library is an Arduino / C++ based library currently consisting of 3 games: Pong, Snake and Breakout. It provides very user friendly abstractions and interfaces whilst keeping the inner workings out of sight and out of mind.

## Compatibility 

Due to the way the ArduGame library is written, there are no restraints on the hardware setup used. Instead of requiring a fixed number of components with which to display the data, it gives you the freedom to write your own callback function.

## Usage

The library is split up into the individual games, however the interface is very similar. Let ArduGame be any of the three games:

1. Include the game header file and set up your pins

```#include <ArduGame.h>

/* Pin setup */```


void display(bool ** matrix); 

void setup()
{

}