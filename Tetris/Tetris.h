//
//  Tetris.h
//  Tetris
//
//  Created by Peter Goldsborough on 30/05/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#ifndef __Tetris__Tetris__
#define __Tetris__Tetris__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif

struct Dot
{
    Dot(short xCoord = 0, short yCoord = 0)
    : x(xCoord), y(yCoord)
    { }
    
    short x,y;
};

class Tetromino
{
    
public:
    
    enum Dir
    {
        LEFT,
        RIGHT,
        DOWN,
        UP
    };
    
    Tetromino();
    
    void rotate(bool dir);
    
    void move(unsigned short dir);
    
    short topEnd() const
    { return _topEnd->y; }
    
    short bottomEnd() const
    { return _bottomEnd->y; }
    
    short leftEnd() const
    { return _leftEnd->x; }
    
    short rightEnd() const
    { return _rightEnd->x; }
    
    const Dot& rotationAxis() const
    { return *_axis; }
    
    Dot& operator[] (const unsigned short i)
    { return _dots[i]; }
    
    const Dot& operator[] (const unsigned short i) const
    { return _dots[i]; }
    
private:
    
    enum Type
    {
        HANDLE,
        AXE_A,
        AXE_B,
        Z_A,
        Z_B,
        BOX,
        LINE
    };
    
    unsigned short _type;
    
    Dot _dots [4];
    
    Dot* _topEnd;
    Dot* _bottomEnd;
    Dot* _leftEnd;
    Dot* _rightEnd;
    
    Dot* _axis;
};

class Tetris
{
public:
 
    Tetris(unsigned short rows,
           unsigned short cols,
           void (*displayCallback)(bool **),
           unsigned short speed = 200);
    
    
    ~Tetris();
    
    void start()
    {
        _stopped = false;
        _display();
    }
    
    void stopped()
    { _stopped = true; }
    
    void reset();

    bool gameOver()
    { return _gameOver; }
    
    void setSpeed(unsigned short speed)
    { _speed = speed; }
    
    void rotateLeft();
    
    void rotateRight();
    
    void right();
    
    void left();
    
    void down();
    
    void update();
    
private:
    
    bool _occupied(const Tetromino& tetro);
    
    bool _occupied(unsigned short row, unsigned short col) const
    { return _matrix[row][col]; }
    
    void _lightTetro(const Tetromino& tetro, bool state = true);
    
    void _checkRows();
    
    void _deleteRow(unsigned short row);
    
    void _display();
    
    void _newTetro();
    
    // http://en.wikipedia.org/wiki/Fundamental_theorem_of_software_engineering
    
    static void _rotate(Tetromino* tetro, bool dir);
    
    static void _move(Tetromino* tetro, unsigned short dir);
    
    template <class FUNC, class ARG>
    void _doSomething(FUNC func, ARG dir, ARG revDir);
    
    bool _gameOver;
    bool _stopped;
    
    unsigned short _speed;
    
    short _tetroLevel;
    
    short _rows, _cols;
    
    Tetromino * _tetro;
    
    bool ** _matrix;
    bool ** _occupiedMatrix;
    
    void (*_dsp) (bool **);
};

#endif /* defined(__Tetris__Tetris__) */
