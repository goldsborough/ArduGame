#include "Pong.h"

Paddle::Paddle(unsigned short column,
               unsigned short bottomRow,
               unsigned short paddleSize)
: _paddleSize(paddleSize)
{
    _dots = new Dot [_paddleSize];
    
    for (int i = _paddleSize - 1; i >= 0; --i)
    {
        _dots[i].x = column;
        _dots[i].y = bottomRow--;
    }
}

void Paddle::up()
{
    for (unsigned short i = 0; i < _paddleSize; ++i)
        _dots[i].y--;
}

void Paddle::down()
{
    for (unsigned short i = 0; i < _paddleSize; ++i)
        _dots[i].y++;
}

Paddle::~Paddle()
{
    delete [] _dots;
}

Pong::Pong(unsigned short rows,
           unsigned short cols,
           void (*PongCallback) (bool ** matrix),
           bool AI,
           unsigned short paddleSize,
           unsigned short speed)

: _display(PongCallback), _rows(rows), _cols(cols), _speed(speed),
  _aiEnabled(AI), _mid(cols / 2), _stopped(1), _gameOver(0),
  _aiProbab(0), _whoWon(0)

{
    if (_aiEnabled)
    {
        _aiProbab = new bool [100];
        
        setAIDiff(INTERMEDIATE);
    }
    
    _ball.x = _cols / 2;
    _ball.y = _rows / 2 - 1;
    
    _ball.dx = -1;
    _ball.dy = 0;

    unsigned short bottomY1 = (_rows * 0.75) - 1;
    unsigned short bottomY2 = (_rows * 0.75) - 2;
    
    if (paddleSize == 0)
        paddleSize = (_rows / 2) - 1;
    
    _paddleA = new Paddle(0,bottomY1,paddleSize);
    _paddleB = new Paddle(_cols - 1,bottomY2,paddleSize);
    
    _matrix = new bool * [_rows];
    
    for (unsigned short i = 0; i < _rows; ++i)
    {
        _matrix[i] = new bool [_cols];
        
        for (unsigned short j = 0; j < _cols; ++j)
        {
            if ((i >= _paddleA->top() && i <= _paddleA->bottom() && j == _paddleA->col()) ||
                (i >= _paddleB->top() && i <= _paddleB->bottom() && j == _paddleB->col()) ||
                (i == _ball.y && j == _ball.x))
            {
                _matrix[i][j] = true;
            }
            
            else _matrix[i][j] = false;
        }
    }
    
    randomSeed(analogRead(0));
}

void Pong::start()
{
    _stopped = false;
    _gameOver = false;
    
    _whoWon = 0;
    
    _ball.x = _cols / 2;
    _ball.y = _rows / 2 - 1;
    
    _ball.dx = -1;
    _ball.dy = 0;
    
    static unsigned short bottomY1 = (_rows * 0.75) - 1;
    static unsigned short bottomY2 = (_rows * 0.75) - 2;
    
    while (_paddleA->bottom() < bottomY1) leftDown();
    while (_paddleA->bottom() > bottomY1) leftUp();
    
    while (_paddleB->bottom() < bottomY2) rightDown();
    while (_paddleB->bottom() > bottomY2) rightUp();
    
    for (unsigned short i = 0; i < _rows; ++i)
    {
        for (unsigned short j = 0; j < _cols; ++j)
        {
            if ((i >= _paddleA->top() && i <= _paddleA->bottom() && j == _paddleA->col()) ||
                (i >= _paddleB->top() && i <= _paddleB->bottom() && j == _paddleB->col()) ||
                (i == _ball.y && j == _ball.x))
            {
                _matrix[i][j] = true;
            }
            
            else _matrix[i][j] = false;
        }
    }
    
    _display(_matrix);
}

void Pong::setAIDiff(unsigned short diff)
{
    unsigned short end = 0;
    
    if (diff > 4) diff = 4;
    
    switch (diff)
    {
        case VERY_EASY:
            end = 70;
            break;
        case EASY:
            end = 75;
            break;
        case INTERMEDIATE:
            end = 80;
            break;
        case HARD:
            end = 85;
            break;
        case EXTREME:
            end = 90;
            break;
    }
    
    for (unsigned short i = 0; i < 100; ++i)
        _aiProbab[i] = (i < end) ? 1 : 0;
}

void Pong::_calcFutureY()
{
    short x = _ball.x;
    short y = _ball.y;
    
    short dx = _ball.dx;
    short dy = _ball.dy;
    
    while (x < _paddleB->col())
    {
        x += dx;
        y += dy;
        
        if (y <= 0 || y >= _rows - 1)
        {
            dy = -dy;
            
            y = (y <= 0) ? 0 : _rows - 1;
        }
    }
    
    _futureY = y;
}

void Pong::_AIMove()
{
    if (_ball.dx < 0) return;
    
    short dir;
    
    static short min1 [] = {0,1};
    static short zero [] = {-1,1};
    static short plus1 [] = {-1,0};
    
    // what direction the paddle should go into
    if (_futureY - _paddleB->bottom() > 0) dir = -1;
    
    else if (_futureY - _paddleB->top() < 0) dir = 1;
    
    else dir = 0;
    
    if (! _aiProbab[random(100)])
    {
        
        // put all other directions in the others array
        switch (dir) {
            case -1:
                dir = min1[random(2)];
                break;
            case 0:
                dir = zero[random(2)];
                break;
            case 1:
                dir = plus1[random(2)];
                break;
        }
    }
    
    if (dir == -1)
        rightDown();
    
    else if (dir == 1)
        rightUp();
}

int Pong::whoWon()
{
    if (! _gameOver)
        return 0;
    
    return _whoWon;
}


void Pong::update()
{
    if (_stopped) return;
    
    if (_aiEnabled)
        _AIMove();
    
    _display(_matrix);
    
    _matrix[_ball.y][_ball.x] = false;
    
    _ball.move();
     
    if (_ball.x <= _paddleA->col())
    {
        if (_ball.y <= 0) _ball.y = 0;
        
        else if (_ball.y >= _rows - 1) _ball.y = _rows - 1;
        
        int paddleMid = -(_paddleA->size() / 2);
        
        bool isEven = _paddleA->size() % 2 == 0;
        
        _gameOver = true;
        
        for (unsigned short i = 0; i < _paddleA->size(); ++i)
        {
            // if ball touches paddle
            if (_ball.y == (*_paddleA)[i])
            {
                _ball.dx = -_ball.dx;
                
                _ball.dy += paddleMid + i;
                
                if (_ball.dy <= 0 && isEven) _ball.dy--;
                
                _calcFutureY();
                
                _ball.move();
                
                _gameOver = false;
                
                break;
            }
        }
        
    }
    
    else if (_ball.x >= _paddleB->col())
    {
        if (_ball.y <= 0) _ball.y = 0;
        
        else if (_ball.y >= _rows - 1) _ball.y = _rows - 1;
        
        int paddleMid = -(_paddleB->size() / 2);
        
        bool isEven = _paddleB->size() % 2 == 0;
        
        _gameOver = true;
        
        for (unsigned short i = 0; i < _paddleB->size(); ++i)
        {
            // if ball touches paddle
            if (_ball.y == (*_paddleB)[i])
            {
                _ball.dx = -_ball.dx;
                
                _ball.dy += paddleMid + i;
                
                if (_ball.dy <= 0 && isEven) _ball.dy--;
                
                _ball.move();
                
                _gameOver = false;
                
                break;
            }
        }
    }
    
    // boundary checking top and bottom
    if (_ball.y <= 0 || _ball.y >= _rows - 1)
    {
        _ball.dy = -_ball.dy;
        
        _ball.y = (_ball.y <= 0) ? 0 : _rows - 1;
    }
    
    if (_gameOver)
    {
        _stopped = true;
        
        _whoWon = (_ball.x == 0) ? 1 : -1;
    }
    
    else _matrix[_ball.y][_ball.x] = true;
    
    unsigned long m = millis();
    
    while (millis() < m + _speed) _display(_matrix);
}

void Pong::rightUp()
{
    if (_paddleB->top() > 0)
    {
        _matrix[_paddleB->bottom()][_paddleB->col()] = 0;
        
        _paddleB->up();
        
        _matrix[_paddleB->top()][_paddleB->col()] = 1;
    }
}

void Pong::rightDown()
{
    
    if (_paddleB->bottom() < _rows - 1)
    {
        _matrix[_paddleB->top()][_paddleB->col()] = 0;
        
        _paddleB->down();
        
        _matrix[_paddleB->bottom()][_paddleB->col()] = 1;
    }
}

void Pong::leftUp()
{
    if (_paddleA->top() > 0)
    {
        _matrix[_paddleA->bottom()][_paddleA->col()] = 0;
        
        _paddleA->up();
        
        _matrix[_paddleA->top()][_paddleA->col()] = 1;
    }
}

void Pong::leftDown()
{
    if (_paddleA->bottom() < _rows - 1)
    {
        _matrix[_paddleA->top()][_paddleA->col()] = 0;
        
        _paddleA->down();
        
        _matrix[_paddleA->bottom()][_paddleA->col()] = 1;
    }
}

Pong::~Pong()
{
    for (unsigned short i = 0; i < _rows; ++i)
        delete [] _matrix[i];
    
    delete [] _matrix;
    
    delete _paddleA;
    delete _paddleB;
}