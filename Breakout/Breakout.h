//
//  Breakout.h
//  Breakout
//
//  Created by Peter Goldsborough on 11/05/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#ifndef __Breakout__Breakout__
#define __Breakout__Breakout__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif

typedef unsigned short size_type;

struct Dot
{
    Dot(size_type xCoord = 0, size_type yCoord = 0)
    : x(xCoord), y(yCoord)
    { }
    
    short x;
    short y;
};

struct Ball : public Dot
{
    Ball(size_type deltaX = 1, size_type deltaY = 0)
    : dx(deltaX), dy(deltaY)
    { }
    
    void move()
    {
        x += dx;
        y += dy;
    }
    
    short dx;
    short dy;
};

class Paddle
{
    
public:
    
    Paddle(size_type row,
           size_type initCol,
           const size_type paddleSize);
    
    ~Paddle()
    { delete [] _dots; }
    
    void right();
    void left();
    
    size_type leftEnd() { return _dots[0].x; }
    size_type rightEnd() { return _dots[_paddleSize - 1].x; }
    
    size_type row() { return _dots[0].y; }
    
    size_type size() { return _paddleSize; }
    
    size_type operator[] (const size_type ind)
    { return _dots[ind].x; }
    
private:
    
    const size_type _paddleSize;
    
    Dot * _dots;
};

class Breakout
{
public:
    
    enum Diff
    {
        VERY_EASY,
        EASY,
        INTERMEDIATE,
        HARD,
        EXTREME
    };
    
    Breakout(size_type rows,
             size_type cols,
             void (*PongCallback) (bool ** matrix),
             size_type difficulty = INTERMEDIATE,
             size_type paddleSize = 0,
             size_type speed = 200);
        
    ~Breakout();
    
    void start();
    
    void stop() { _stopped = true; }
    
    void update();
    
    bool gameOver() { return _gameOver; }
    
    short playerWon()
    {
        if (! gameOver())
            return 0;
        
        return (_won) ? 1 : -1;
    }
    
    void right();
    void left();
    
    void setSpeed(size_type millis) { _speed = millis; }
    
    void setDifficulty(size_type difficulty);
    
    size_type getLives() { return _lives; }
    
private:
    
    void _addRow();
    
    void _reset();
    
    Ball _ball;
    
    Paddle * _paddle;
    
    bool _stopped;
    bool _gameOver;
    
    bool _won;
    
    size_type _speed;
    size_type _rows;
    size_type _cols;
    
    size_type _ballsRowEnd;
    
    size_type _count;
    size_type _maxCount;
    
    size_type _lives;
    
    bool ** _matrix;
    
    void (*_display) (bool ** matrix);
};

#endif /* defined(__Breakout__Breakout__) */
