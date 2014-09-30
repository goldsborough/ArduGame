# ArduGame Libary

<<<<<<< HEAD
In development.
=======
The ArduGame library is an Arduino / C++ based library currently consisting of 4 games: Pong, Snake, Breakout and Tetris. It provides very user friendly abstractions and interfaces whilst keeping the inner workings out of sight and out of mind.

## Compatibility 

Due to the way the ArduGame library is written, there are no restraints on the hardware setup used. Instead of requiring a fixed number of components with which to display the data, it gives you the freedom to write your own callback function.

## Usage

The library is split up into the individual games, however the interface is very similar. Let ArduGame be any of the four games:

1. Include the game header file

```cpp
#include <ArduGame.h>
```
2. Forward declare the callback function. It must:
  * Return void
  * Take a bool pointer pointer as its only argument

`void display(bool ** matrix);` 

3. Create an instance of the Game. The required arguments are:
  1. The number of rows
  2. The number of columns
  3. The callback function
          
`ArduGame game(8,8,display);`

4. In the setup function, you will probably want to call the start()
   member that all games have. It starts the game and is also used to
   reset the whole system once the game is over. Call `stop()` to stop the
   game.

```cpp
void setup()
{
  /* Other things */
  
  Serial.begin(9600); // Only needed for example callback
  
  game.start();
}
```
5. Define the callback function. The game passes you a two dimensional array
   with the same dimensions as you passed them in when creating the object. 
   Do not delete the pointer, please. The code below will display the game in
   the Serial monitor.

Work in progress.
>>>>>>> 8c8d057b343493653a4f1f23c0527e0e3b15cb05
