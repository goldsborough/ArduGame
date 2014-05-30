//
//  Tetris.cpp
//  Tetris
//
//  Created by Peter Goldsborough on 30/05/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#include "Tetris.h"

Tetromino::Tetromino()
{
    _leftEnd = _topEnd = _dots;
    _rightEnd = _bottomEnd = &_dots[3];
    _axis = &_dots[1];
    
    for (unsigned short i = 1; i < 4; ++i) _dots[i].x = i;
    
    _type = random(7);
    
    switch (_type)
    {
        case HANDLE:
        {
            _dots[3].x = 1;
            _dots[3].y = 1;
            
            _rightEnd = &_dots[2];
            
            break;
        }
        case AXE_A:
        {
            _dots[3].x--;
            _dots[3].y++;
            break;
        }
        case AXE_B: // one tile up
        {
            for (unsigned short i = 0; i < 4; ++i) _dots[i].y++;
            
            _dots[3].x--;
            _dots[3].y--;
            
            _topEnd = &_dots[3];
            _bottomEnd = &_dots[2];
            
            break;
        }
        case Z_A: // two tiles up
        {
            for (unsigned short i = 0; i < 4; ++i)
                _dots[i].y++;
            
            _dots[2].x--;
            _dots[2].y--;
            
            _dots[3].x--;
            _dots[3].y--;
            
            _topEnd = &_dots[3];
            _bottomEnd = &_dots[1];
            
            break;
        }
        case Z_B: // two tiles down
        {
            _dots[2].x--;
            _dots[2].y++;
            
            _dots[3].x--;
            _dots[3].y++;
            
            break;
        }
        case BOX:
        {
            _dots[2].x -= 2;
            _dots[2].y++;
            
            _dots[3].x -= 2;
            _dots[3].y++;
            
            break;
        }
            
        case LINE:
        default:
            break;
    }
}

void Tetromino::rotate(bool dir)
{
    if (_type == BOX) return;
    
    if (dir == RIGHT)
    {
        Dot * temp = _rightEnd;
        
        _rightEnd = _topEnd;
        _topEnd = _leftEnd;

        _leftEnd = _bottomEnd;
        _bottomEnd = temp;
        
        for (unsigned short i = 0; i < 4; ++i)
        {
            Dot& dot = _dots[i];
            
            if (&dot == _axis) continue;
            
            const short x = dot.x;
            const short y = dot.y;
            
            if (x < _axis->x)
            {
                dot.x += (_axis->x - x);
                dot.y -= (_axis->x - x);
            }
            
            else if (x > _axis->x)
            {
                dot.x -= (x- _axis->x);
                dot.y += (x - _axis->x);
            }
            
            if (y < _axis->y)
            {
                dot.x += (_axis->y - y);
                dot.y += (_axis->y - y);
            }
            
            else if (y > _axis->y)
            {
                dot.x -= (y - _axis->y);
                dot.y -= (y - _axis->y);
            }
        }
        
    }
    
    else
    {
        Dot * temp = _topEnd;
        
        _topEnd = _rightEnd;
        _rightEnd = _bottomEnd;
        
        _bottomEnd = _leftEnd;
        _leftEnd = temp;
        
        for (unsigned short i = 0; i < 4; ++i)
        {
            Dot& dot = _dots[i];
            
            if (&dot == _axis) continue;
            
            short x = dot.x;
            short y = dot.y;

            if (x < _axis->x)
            {
                dot.x += (_axis->x - x);
                dot.y += (_axis->x - x);
            }
            
            else if (x > _axis->x)
            {
                dot.x -= (x- _axis->x);
                dot.y -= (x - _axis->x);
            }
            
            if (y < _axis->y)
            {
                dot.x -= (_axis->y - y);
                dot.y += (_axis->y - y);
            }
            
            else if (y > _axis->y)
            {
                dot.x += (y - _axis->y);
                dot.y -= (y - _axis->y);
            }
        
        }
    }
}

void Tetromino::move(unsigned short dir)
{
    for (unsigned short i = 0; i < 4; ++i)
    {
        Dot& dot = _dots[i];
        
        switch(dir)
        {
            case LEFT:
                dot.x--;
                break;
                
            case RIGHT:
                dot.x++;
                break;
            case DOWN:
                dot.y++;
                break;
            case UP:
                dot.y--;
                break;
        }
    }
}

Tetris::Tetris(unsigned short rows,
               unsigned short cols,
               void (*displayCallback)(bool **),
               unsigned short speed)

: _rows(rows), _cols(cols), _speed(speed), _dsp(displayCallback),
  _stopped(1), _gameOver(0), _tetroLevel(rows - 1), _tetro(0)
{
    _matrix = new bool * [_rows];
    
    for (unsigned short i = 0; i < _rows; ++i)
    {
        _matrix[i] = new bool [_cols];
        
        for (unsigned short j = 0; j < _cols; ++j)
            _matrix[i][j] = 0;
    }
    
    _occupiedMatrix = new bool * [_rows];
    
    for (unsigned short i = 0; i < _rows; ++i)
    {
        _occupiedMatrix[i] = new bool [_cols];
        
        for (unsigned short j = 0; j < _cols; ++j)
            _occupiedMatrix[i][j] = 0;
    }
    
    randomSeed(analogRead(0));
    
    _newTetro();
    
    _lightTetro(*_tetro);
}

Tetris::~Tetris()
{
    for (unsigned short i = 0; i < _rows; ++i)
        delete [] _matrix[i];
    
    delete [] _matrix;
    
    for (unsigned short i = 0; i < _rows; ++i)
        delete [] _occupiedMatrix[i];
    
    delete [] _occupiedMatrix;
    
    delete _tetro;
}

void Tetris::reset()
{
    _gameOver = false;
    _stopped = false;
    
    _tetroLevel = 0;
    
    for (unsigned short i = 0; i < _rows; ++i)
    {
        for (unsigned short j = 0; j < _cols; ++j)
            _matrix[i][j] = 0;
    }
    
    for (unsigned short i = 0; i < _rows; ++i)
    {
        for (unsigned short j = 0; j < _cols; ++j)
            _occupiedMatrix[i][j] = 0;
    }
    
    _newTetro();
    
    _lightTetro(*_tetro);
}

void Tetris::update()
{
    if (_stopped) return;
    
    down();
    
    _checkRows();
    
    _display();
}

void Tetris::_display()
{
    unsigned long m = millis();
    
    while(millis() < m + _speed)
        _dsp(_matrix);
}

bool Tetris::_occupied(const Tetromino& tetro)
{
    if (tetro.bottomEnd() < _tetroLevel)
        return false;
    
    for (unsigned short i = 0; i < 4; ++i)
    {
        if (_occupiedMatrix[tetro[i].y][tetro[i].x])
            return true;
    }
    
    return false;
}

void Tetris::_lightTetro(const Tetromino& tetro, bool state)
{
    for(unsigned short i = 0; i < 4; ++i)
        _matrix[tetro[i].y][tetro[i].x] = state;
}

void Tetris::_newTetro()
{
    if (_tetro)
    {
        for (unsigned short i = 0; i < 4; ++i)
            _occupiedMatrix[(*_tetro)[i].y][(*_tetro)[i].x] = true;
        
        delete _tetro;
    }
    
    _tetro = new Tetromino;
    
    for (unsigned short i = 0, end = _cols / 4; i < end; ++i)
        if(! _gameOver) right();
    
    _display();
}

void Tetris::_rotate(Tetromino* tetro, bool dir)
{ tetro->rotate(dir); }

void Tetris::_move(Tetromino* tetro, unsigned short dir)
{ tetro->move(dir); }

template <class FUNC, class ARG>
void Tetris::_doSomething(FUNC func, ARG dir, ARG revDir)
{
    func(_tetro,dir);
    
    bool lessThanBottom = _tetro->bottomEnd() >= _rows;
    
    bool outOfBounds = _tetro->leftEnd() < 0        ||
                       _tetro->rightEnd() >= _cols  ||
                       lessThanBottom               ||
                       _tetro->topEnd() < 0;
    
    if (outOfBounds || _occupied(*_tetro))
    {
        
        if (! outOfBounds && _tetro->topEnd() <= 0)
        {
            _stopped = true;
            _gameOver = true;
            
            _display();
        }
        
        else
        {
            func(_tetro,revDir); // reverse direction
            
            if (_tetro->topEnd() < _tetroLevel)
                _tetroLevel = _tetro->topEnd();
            
            _lightTetro(*_tetro);
            
            if (! outOfBounds || lessThanBottom) _newTetro();
        }
        
    }
    
    else
    {
        // go back and unlight
        func(_tetro,revDir);
        _lightTetro(*_tetro,false);
        
        // go to new pos and light up
        func(_tetro,dir);
        _lightTetro(*_tetro);
        
    }
}

void Tetris::rotateLeft()
{
    _doSomething(_rotate, Tetromino::LEFT, Tetromino::RIGHT);
}

void Tetris::rotateRight()
{
    _doSomething(_rotate, Tetromino::RIGHT, Tetromino::LEFT);
}

void Tetris::right()
{
    _doSomething(_move, Tetromino::RIGHT, Tetromino::LEFT);
}

void Tetris::left()
{
    _doSomething(_move,Tetromino::LEFT,Tetromino::RIGHT);
}

void Tetris::down()
{
    _doSomething(_move,Tetromino::DOWN,Tetromino::UP);
}

void Tetris::_deleteRow(unsigned short row)
{
    for (short i = row, j = row - 1; j >= _tetroLevel; --i, --j)
    {
        for (short c = 0; c < _cols; ++c)
            _matrix[i][c] = _matrix[j][c];
    }
    
    /*
     * It might seem to make sense to just copy the values from _tetroLevel - 1
     * into _tetroLevel, however, if the tetromino currently falling down happens
     * to be right before the tetrolevel, like when your level is at row 1, the
     * values from the tetromino would be copied into the last row, which would
     * not be good.
     */
    
    for (short c = 0; c < _cols; ++c)
            _matrix[_tetroLevel][c] = 0;
    
    
    --_tetroLevel;
}

void Tetris::_checkRows()
{
    bool full;
    
    for (unsigned short r = _tetroLevel; r < _rows; ++r)
    {
        full = true;
        
        for (unsigned short c = 0; c < _cols; ++c)
        {
            if (! _occupied(r,c))
            {
                full = false;
                break;
            }
        }
        
        if (full)
        {
            _deleteRow(r);
            
            r = _tetroLevel;
        }
    }
}

