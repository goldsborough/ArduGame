//
//  Snake.h
//  Snake
//
//  Created by Peter Goldsborough on 07/05/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#ifndef __Snake__Snake__
#define __Snake__Snake__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif


/*
        |
 width  |
        |______
 
         length
*/

/*
 Example usage:
 
 void loop()
 {
    // check whether the snake has crashed into itself
    if (! snake.gameOver())
    {
        // get new directions, if any
        if (digitalRead(0)) snake.up();
        else if (digitalRead(0)) snake.down();
 
        ...
        
        // make the snake move :)
        snake.move();
    }
 }
 */

class Snake
{
    
public:
    
    typedef short size_type;
    
    Snake(size_type rows,
          size_type cols,
          void (*SnakeCallback) (bool ** matrix),
          size_type speed = 200);
    
    ~Snake();
    
    void start()
    {
        _stopped = false;
        _gameOver = false;
        _display(_matrix);
    }
    
    void stop()
    { _stopped = true; }
    
    void update();
    
    void up() { _changeDir(DOWN, UP); }
    void down() { _changeDir(UP, DOWN); }
    void left() { _changeDir(RIGHT, LEFT); }
    void right() { _changeDir(LEFT,RIGHT); }
    
    void setSpeed(size_type millis) { _speed = millis; }
    
    size_type getSpeed() { return _speed; }
    
    bool gameOver() { return _gameOver; }
    
private:
    
    enum DIR
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };
    
    struct Node
    {
        Node() : next(0) { }
        
        void operator=(const Node& other)
        { x = other.x; y = other.y; }
        
        bool operator== (const Node& other)
        { return other.x == x && other.y == y; }
        
        bool operator!= (const Node& other)
        { return ! (*this == other); }
        
        size_type x,y;
        
        Node* next;
    };
    
    void _attachNode();
    
    void _genFruit();
    
    void _loop();
    
    void _lost();
    
    void _changeDir(size_type oppos, size_type newDir);
    
    bool _outOfBounds(const size_type pos, const DIR dir);
    
    Node * _head;
    Node * _tail;
    Node * _fruit;
    
    const size_type _rows;
    const size_type _cols;
    
    size_type _speed;
    
    bool _stopped;
    bool _gameOver;
    
    size_type _dir;
    
    bool ** _matrix;
    
    void (*_display) (bool ** matrix);
};

#endif /* defined(__Snake__Snake__) */
