//
//  Pong.h
//  Pong
//
//  Created by Peter Goldsborough on 10/05/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#ifndef __Pong__Pong__
#define __Pong__Pong__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif

struct Dot
{
    Dot(unsigned short xCoord = 0,
        unsigned short yCoord = 0)
    : x(xCoord), y(yCoord)
    { }
    
    short x;
    short y;
};

struct Ball : public Dot
{
    Ball(unsigned short deltaX = 1, unsigned short deltaY = 0)
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
    
    Paddle(unsigned short column,
           unsigned short bottomRow,
           unsigned short paddleSize);
    
    ~Paddle();
    
    void up();
    void down();
    
    short col() { return _dots[0].x; }
    
    short top() { return _dots[0].y; }
    short bottom() { return _dots[_paddleSize - 1].y; }
    
    unsigned short size() { return _paddleSize; }
    
    short operator[] (const unsigned short ind)
    { return _dots[ind].y; }
    
private:
    
    unsigned short _paddleSize;
    
    Dot * _dots;
};


class Pong
{
public:
    
    enum AIDiff
    {
        VERY_EASY,
        EASY,
        INTERMEDIATE,
        HARD,
        EXTREME
    };
    
    Pong(unsigned short rows,
         unsigned short cols,
         void (*PongCallback) (bool ** matrix),
         bool AI = false,
         unsigned short paddleSize = 0,
         unsigned short speed = 200);
    
    ~Pong();
    
    void start();
    void stop() { _stopped = true; }
    
    void update();
    
    bool gameOver() { return _gameOver; }
    
    int whoWon();
    
    void rightUp();
    void rightDown();
    
    void leftUp();
    void leftDown();
    
    void setAIDiff(unsigned short diff);
    
    void setSpeed(unsigned short millis) { _speed = millis; }
    
    unsigned short getSpeed() { return _speed; }
    
private:
    
    void _calcFutureY();
    void _AIMove();
    
    Ball _ball;
    
    Paddle * _paddleA;
    Paddle * _paddleB;
    
    bool _stopped;
    bool _gameOver;
    
    int _whoWon;
    
    unsigned short _speed;
    unsigned short _rows;
    unsigned short _cols;
    unsigned short _mid;
    
    bool ** _matrix;
    
    void (*_display) (bool ** matrix);
    
    bool _aiEnabled;
    bool * _aiProbab;
    
    short _futureY;
};

#endif /* defined(__Pong__Pong__) */
