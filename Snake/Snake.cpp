//
//  Snake.cpp
//  Snake
//
//  Created by Peter Goldsborough on 07/05/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#include "Snake.h"

Snake::Snake(size_type rows,
             size_type cols,
             void (*SnakeCallback) (bool ** matrix),
             size_type speed)

: _rows(rows), _cols(cols), _dir(RIGHT), _display(SnakeCallback),
  _speed(speed), _stopped(true)

{
    _matrix = new bool * [_rows];
    
    for (unsigned int r = 0; r < _cols; ++r)
    {
        _matrix[r] = new bool [_cols];
        
        for (unsigned int c = 0; c < _cols; ++c)
            _matrix[r][c] = false;
    }
    
    _head = new Node;
    _fruit = new Node;
    
    _genFruit();
    
    // place head in third quarter
    _head->x = ((int)_cols * 0.75) - 1;
    _head->y = (_rows / 2) - 1;
    
    _matrix[_head->y][_head->x] = true;
    
    _tail = _head;
    
    // attach cols / 4 nodes
    for (unsigned int i = 0, end = (_cols / 4); i < end; ++i) _attachNode();
}

void Snake::_lost()
{
    // create 2D bool array with all 0s (everything off)
    
    bool ** empty = new bool * [_rows];
    
    for (unsigned int r = 0; r < _cols; ++r)
    {
        empty[r] = new bool [_cols];
        
        for (unsigned int c = 0; c < _cols; ++c)
            empty[r][c] = false;
    }
    
    for (unsigned int i = 0; i < 5; ++i)
    {
        _display(_matrix);
        delay(500);
        
        _display(empty);
        
        delay(500);
    }
    
    _gameOver = true;
    _stopped = true;
}

void Snake::_changeDir(size_type oppos, size_type newDir)
{
    if(_dir != oppos) _dir = newDir;
}

void Snake::update()
{
    if (_stopped) return;
    
    // unlight last node
    _matrix[_tail->y][_tail->x] = false;
    
    // set all nodes to the position of the next node

    Node * itr = _tail;
    
    while(itr != _head)
    {
        *itr = *(itr->next);
        
        itr = itr->next;
    }
    
    // update head and check boundaries
    
    size_type& x = _head->x;
    size_type& y = _head->y;
    
    switch (_dir)
    {
        case UP:
            y = _outOfBounds(y - 1,UP) ? (_rows - 1) : y - 1;
            break;
            
        case DOWN:
            y = _outOfBounds(y + 1, DOWN) ? 0 : y + 1;
            break;
            
        case LEFT:
            x = _outOfBounds(x - 1, LEFT) ? (_cols - 1) : x - 1;
            break;
        
        case RIGHT:
            x = _outOfBounds(x + 1, RIGHT) ? 0 : x + 1;
            break;
    }
    
    if (_matrix[y][x])
    {
        if (*_head == *_fruit)
        {
            // attach new node
            _attachNode();
            
            // generate new fruit
            _genFruit();
        }
        
        else _lost();
    }
    
    else _matrix[y][x] = true;

    unsigned long m = millis();
    
    while (millis() < m + _speed)
        _display(_matrix);

}

void Snake::_genFruit()
{
    size_type r,c;
    
    // get random positions until they are
    // not occupied by the snake
    
    do
    {
        r = random(_rows); //rand() % _rows;
        c = random(_cols);
        
    } while (_matrix[r][c]);
    
    _matrix[r][c] = true;
    
    _fruit->x = c;
    _fruit->y = r;
}

bool Snake::_outOfBounds(const size_type pos, const DIR dir)
{
    switch (dir) {
        case UP:
        case LEFT:
            return pos < 0;
            break;
            
        case DOWN:
            return pos >= _rows;
            break;
            
        case RIGHT:
            return pos >= _cols;
            
        default:
            return true;
    }
}

void Snake::_attachNode()
{
    Node * n = new Node;
    
    short diffx = (_tail->next) ? _tail->x - _tail->next->x : -1;
    short diffy = (_tail->next) ? _tail->y - _tail->next->y : 0;
    
    // add right
    if (diffx == 1)
    {
        n->x =  _outOfBounds(_tail->x + 1,RIGHT) ? 0 : _tail->x + 1;
        n->y = _tail->y;
    }
    
    // add left
    else if (diffx == -1)
    {
        n->x = _outOfBounds(_tail->x - 1,LEFT) ? (_cols - 1) : (_tail->x - 1);
        n->y = _tail->y;
    }
    
    // add above
    else if (diffy == 1)
    {
        n->x = _tail->x;
        n->y = _outOfBounds(_tail->y - 1,UP) ? (_rows - 1) : _tail->y - 1;
    }
    
    // add below
    else if (diffy == -1)
    {
        n->x = _tail->x;
        n->y = _outOfBounds(_tail->y + 1,DOWN) ? 0 : _tail->y + 1;
    }
    
    n->next = _tail;
    
    _tail = n;
    
    // set node in matrix to true
    _matrix[_tail->y][_tail->x] = true;
}

Snake::~Snake()
{
    for (unsigned int i = 0; i < _rows; i++)
        delete [] _matrix[i];
    
    delete [] _matrix;
    
    delete _fruit;
    
    while (_tail != 0)
    {
        Node * n = _tail->next;
        delete _tail;
        _tail = n;
    }
}