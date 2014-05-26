//
//  Breakout.cpp
//  Breakout
//
//  Created by Peter Goldsborough on 11/05/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#include "Breakout.h"

Paddle::Paddle(size_type row,
               size_type initCol,
               const size_type paddleSize)

: _paddleSize(paddleSize)

{
    _dots = new Dot [_paddleSize];
 
    // fill the paddle
    for (int i = 0; i < _paddleSize; ++i)
    {
        _dots[i].x = i + initCol;
        _dots[i].y = row;
    }
}

void Paddle::right()
{
    for (size_type i = 0; i < _paddleSize; ++i)
        _dots[i].x++;
}

void Paddle::left()
{
    for (size_type i = 0; i < _paddleSize; ++i)
        _dots[i].x--;
}

Breakout::Breakout(size_type rows,
                   size_type cols,
                   void (*PongCallback) (bool ** matrix),
                   size_type difficulty,
                   size_type paddleSize,
                   size_type speed)

: _rows(rows), _cols(cols), _speed(speed), _display(PongCallback)
{
    // default paddle size is column number / 2 - 1
    // seems like a good size, consider an 8 * 8 matrix
    // that gives you 8 / 2 = 4 - 1 = 3
    if (paddleSize == 0)
        paddleSize = (_cols / 2) - 1;
    
    _paddle = new Paddle(_rows - 1, (_cols / 4) + 1, paddleSize);

    setDifficulty(difficulty);
    
    _stopped = true;
    _gameOver = false;
    
    // resurrect
    _lives = 3;
    
    _reset();
    
    // reset the balls edge
    _ballsRowEnd = (_rows / 2) - 2;
    
    _matrix = new bool * [_rows];
    
    // light up spaces that are used right now
    for (size_type i = 0; i < _rows; ++i)
    {
        _matrix[i] = new bool [_cols];
        
        for (size_type j = 0; j < _cols; ++j)
        {
            if ( i <= _ballsRowEnd ||
                (i == (_rows - 1) && j <= _paddle->rightEnd() && j >= _paddle->leftEnd()) ||
                (i == _ball.y && j == _ball.x))
            {
                _matrix[i][j] = true;
            }
            
            else _matrix[i][j] = false;
        }
    }
}

void Breakout::start()
{
    // reset game
    _stopped = false;
    _gameOver = false;
    
    // resurrect
    _lives = 3;
    
    _reset();
    
    // reset the balls edge
    _ballsRowEnd = (_rows / 2) - 2;
    
    // light up spaces that are used right now
    for (size_type i = 0; i < _rows; ++i)
    {
        for (size_type j = 0; j < _cols; ++j)
        {
            if ( i <= _ballsRowEnd ||
                (i == (_rows - 1) && j <= _paddle->rightEnd() && j >= _paddle->leftEnd()) ||
                (i == _ball.y && j == _ball.x))
            {
                _matrix[i][j] = true;
            }
            
            else _matrix[i][j] = false;
        }
    }
}

void Breakout::setDifficulty(size_type difficulty)
{
    // do boundary checking
    if (difficulty < 0) difficulty = 0;
    
    else if (difficulty > 4) difficulty = 4;
    
    _maxCount = (_cols / 2) * (5 - difficulty);
}

void Breakout::_addRow()
{
    // if the balls edge reaches the paddle level game over
    if (++_ballsRowEnd == _paddle->row())
        _gameOver = true;
    
    for (size_type i = 0; i < _cols; ++i)
        _matrix[_ballsRowEnd][i] = true;
}

void Breakout::_reset()
{
    // reset the ball position
    _ball.y = _rows / 2;
    _ball.x = _cols / 2;
    
    // and the ball acceleration
    _ball.dx = 0;
    _ball.dy = 1;
    
    // reposition the paddle
    while (_paddle->leftEnd() > (_cols / 4) + 1)
        left();
    
    while (_paddle->leftEnd() < (_cols / 4) + 1)
        right();
}

void Breakout::update()
{
    // don't do anything if the game hasn't been
    // started yet or the game is over
    if (_stopped) return;
    
    // first display
    _display(_matrix);
    
    // unlight the current ball position
    _matrix[_ball.y][_ball.x] = false;
    
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * Basically, by doing every move individually instead
     * of just moving the ball according to it's dx/dy we
     * can have more control over what to do when the ball
     * hits something. Consider the ball moving towards the
     * balls end with a dy of 3. Usually the ball would just
     * move up 3 if we do it the easy way, however this is
     * horrible, since the ball should have been repelled by
     * the first layer of the ball. Instead, we loop over
     * this dy (3 times) and check the positions individually.
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    
    // store variables
    short dyTotal = _ball.dy;
    short dxTotal = _ball.dx;
    
    // get the sign right
    short dx = (dxTotal > 0) ? 1 : -1;
    short dy = (dyTotal > 0) ? 1 : -1;
    
    bool flag = true;
    
    while (flag && (dyTotal || dxTotal))
    {
        // if there is still a dy left
        if (dyTotal)
        {
            // increase the balls y
            _ball.y += dy;
            
            // decrease the stored dy
            dyTotal += -dy;
        }
        
        // same for dx
        if (dxTotal)
        {
            _ball.x += dx;

            dxTotal += -dx;
        }
        
        if (_ball.y == _paddle->row())
        {
            // do the physics
            
            // Negative because the more to the left
            // the ball hits the paddle, the more negative
            // the ball's dx becomes
            int paddleMid = -(_paddle->size() / 2);
            
            // If the paddle has no midpoint, the
            // change in acceleration has to be + 1
            // on the right side of the paddle
            bool isEven = _paddle->size() % 2 == 0;
            
            bool touchedPaddle = false;
            
            for (size_type i = 0; i < _paddle->size(); ++i)
            {
                // if the ball touches paddle
                if (_ball.x == (*_paddle)[i])
                {
                    // change the dy upwards obviously
                    _ball.dy = -_ball.dy;
                    
                    // depending on which part of the paddle
                    // the ball hits, change the dx. On the
                    // very left the dx will become most negative
                    // on the very right most positive
                    _ball.dx += paddleMid + i;
                    
                    // if the paddle is even, there can be no 0 dx change
                    if (_ball.dx <= 0 && isEven) _ball.dx--;
                    
                    touchedPaddle = true;
                    
                    break;
                }
            }
            
            // if none of the paddle's dots were touched
            // by the ball but the ball is still in the
            // last row, the ball is going to loser-land
            if (! touchedPaddle)
            {
                // if no more lives left
                // game over
                if (--_lives <= 0)
                    _gameOver = true;
                
                else _reset();
            }
            
            // if the paddle was touched
            else
            {
                // if the paddle touches exceed the current
                // difficulty's max count, add another row
                if (++_count >= _maxCount)
                {
                    _addRow();
                    _count = 0;
                }
                
                // move le ballon
                _ball.move();
            }
            
            flag = false;
        }
        
        // if the ball touches a ball from the wall
        // that just didn't make any sense
        if (_matrix[_ball.y][_ball.x])
        {
            //unlight it
            _matrix[_ball.y][_ball.x] = false;
            
            // if there are more balls in the direction
            // the ball is going in, go back, this is
            // what I saw when looking at other breakout
            // games anyway
            if (_matrix[_ball.y - dy][_ball.x + dx])
                _ball.dx = -_ball.dx;
            
            // go back down
            _ball.dy = -_ball.dy;
            
            _ball.move();
            
            flag = false;
        }
        
        // boundary checking - top
        if (_ball.y <= 0)
        {
            _ball.dy = -_ball.dy;
            _ball.y = 0;
            
            flag = false;
        }
        
        // boundary checking left and right
        else if (_ball.x <= 0 || _ball.x >= _cols - 1)
        {
            _ball.dx = -_ball.dx;
            
            _ball.x = (_ball.x <= 0) ? 0 : _cols - 1;
            
            flag = false;
        }
        
    }
    
    if (_gameOver) _stopped = true;
    
    else _matrix[_ball.y][_ball.x] = true;
    
    unsigned long m = millis();
    
    while (millis() < m + _speed)
        _display(_matrix);
}

void Breakout::right()
{
    // move right if there still is a right
    if (_paddle->rightEnd() < _cols - 1)
    {
        // unlight the left end
        _matrix[_rows - 1][_paddle->leftEnd()] = 0;
        
        // move right
        _paddle->right();
        
        // light the new right end
        _matrix[_rows - 1][_paddle->rightEnd()] = 1;
    }
}

void Breakout::left()
{
    // as for right()
    if (_paddle->leftEnd() > 0)
    {
        _matrix[_rows - 1][_paddle->rightEnd()] = 0;
        
        _paddle->left();
        
        _matrix[_rows - 1][_paddle->leftEnd()] = 1;
    }
}

Breakout::~Breakout()
{
    for (size_type i = 0; i < _rows; ++i)
        delete [] _matrix[i];
    
    delete [] _matrix;
    
    delete _paddle;
}