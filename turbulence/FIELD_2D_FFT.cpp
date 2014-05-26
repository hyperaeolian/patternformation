#include "FIELD_2D_FFT.h"
#include <assert.h>

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
FIELD_2D_FFT::FIELD_2D_FFT() :
  _xRes(-1), _yRes(-1), 
  _spatial(NULL), _frequency(NULL), 
  _forwardInit(false), _inverseInit(false)
{
}

FIELD_2D_FFT::FIELD_2D_FFT(const FIELD_2D& m) :
  _xRes(m.xRes()), _yRes(m.yRes()), 
  _spatial(NULL), _frequency(NULL), 
  _forwardInit(false), _inverseInit(false)
{
  _totalCells = _xRes * _yRes;
  FFT(m);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
FIELD_2D_FFT::~FIELD_2D_FFT()
{
  if (_spatial)
    delete[] _spatial;
  if (_frequency)
    delete[] _frequency;
}
  
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void FIELD_2D_FFT::FFT(const FIELD_2D& m)
{
  _xRes = m.xRes();
  _yRes = m.yRes();
  _totalCells = _xRes * _yRes;

  // if it's the first time, create the FFT vars
  if (!_forwardInit)
  {
    _spatial  = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * _totalCells);
    _frequency = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * _totalCells);

    _forwardPlan = fftw_plan_dft_2d(_xRes, _yRes, _spatial, _frequency, FFTW_FORWARD, FFTW_ESTIMATE);

    _forwardInit = true;
  }

  // populate the input
  for (int y = 0; y < _yRes; y++)
    for (int x = 0; x < _xRes; x++)
    {
      int index = x + _xRes * y;
      _spatial[index][0] = m[index];
      _spatial[index][1] = 0.0f;
    }

  // run the FFT
  fftw_execute(_forwardPlan);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void FIELD_2D_FFT::inverseFFT(FIELD_2D& m)
{
  assert(m.xRes() == _xRes);
  assert(m.yRes() == _yRes);

  // if it's the first time, create the FFT vars
  if (!_inverseInit)
  {
    assert(_spatial);
    assert(_frequency);

    _inversePlan= fftw_plan_dft_2d(_xRes, _yRes, _frequency, _spatial, FFTW_BACKWARD, FFTW_ESTIMATE);
    _inverseInit = true;
  }

  // run the FFT
  fftw_execute(_inversePlan);

  // populate the input
  for (int y = 0; y < _yRes; y++)
    for (int x = 0; x < _xRes; x++)
    {
      int index = x + _xRes * y;
      m[index] = _spatial[index][0];
    }

  // scale by array size
  m *= 1.0 / _totalCells;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
FIELD_2D FIELD_2D_FFT::inverseFFT(const FIELD_2D& real, const FIELD_2D& imaginary)
{
  assert(real.xRes() == _xRes); 
  assert(real.yRes() == _yRes); 
  assert(imaginary.xRes() == _xRes); 
  assert(imaginary.yRes() == _yRes); 

  FIELD_2D realShifted = real;
  FIELD_2D imaginaryShifted = imaginary;

  shift(realShifted);
  shift(imaginaryShifted);

  for (int x = 0; x < _totalCells; x++)
  {
    _frequency[x][0] = _totalCells * realShifted[x];
    _frequency[x][1] = _totalCells * imaginaryShifted[x];
  }
 
  FIELD_2D final(_xRes, _yRes); 
  // if it's the first time, create the FFT vars
  if (!_inverseInit)
  {
    assert(_spatial);
    assert(_frequency);

    _inversePlan= fftw_plan_dft_2d(_xRes, _yRes, _frequency, _spatial, FFTW_BACKWARD, FFTW_ESTIMATE);
    _inverseInit = true;
  }

  // run the FFT
  fftw_execute(_inversePlan);

  // populate the input
  for (int y = 0; y < _yRes; y++)
    for (int x = 0; x < _xRes; x++)
    {
      int index = x + _xRes * y;
      final[index] = _spatial[index][0];
    }

  // scale by array size
  final *= 1.0 / _totalCells;
  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void FIELD_2D_FFT::shift(FIELD_2D& field)
{
  int xRes = field.xRes();
  int yRes = field.yRes();

  FIELD_2D scratch(field);
  scratch = 0;

  int xHalf = xRes / 2;
  int yHalf = yRes / 2;

  int xMod = xRes % 2;
  int yMod = yRes % 2;

  for (int y = 0; y < yRes; y++)
    for (int x = 0; x < xHalf + xMod; x++)
    {
      int index = x + y * xRes;
      scratch[index] = field[index + xHalf];
    }
  for (int y = 0; y < yRes; y++)
    for (int x = xHalf; x < xRes; x++)
    {
      int index = x + y * xRes;
      scratch[index + xMod] = field[index - xHalf];
    }

  for (int y = 0; y < yHalf + yMod; y++)
    for (int x = 0; x < xRes; x++)
    {
      int original = x + y * xRes;
      int copy = x + (y + yHalf + yMod) * xRes;
      field[copy] = scratch[original];
    }

  for (int y = yHalf; y < yRes; y++)
    for (int x = 0; x < xRes; x++)
    {
      int original = x + y * xRes;
      int copy = x + (y - yHalf) * xRes;
      field[copy] = scratch[original];
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
FIELD_2D FIELD_2D_FFT::real()
{
  FIELD_2D final(_xRes, _yRes);

  for (int x = 0; x < _totalCells; x++)
    final[x] = _frequency[x][0];

  shift(final);
  final *= 1.0 / _totalCells;

  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
FIELD_2D FIELD_2D_FFT::imaginary()
{
  FIELD_2D final(_xRes, _yRes);

  for (int x = 0; x < _totalCells; x++)
    final[x] = _frequency[x][1];
  
  shift(final);
  final *= 1.0 / _totalCells;

  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
FIELD_2D FIELD_2D_FFT::abs()
{
  FIELD_2D final(_xRes, _yRes);

  for (int x = 0; x < _totalCells; x++)
    final[x] = sqrt(_frequency[x][1] * _frequency[x][1] + 
                    _frequency[x][0] * _frequency[x][0]);

  shift(final);

  return final;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
FIELD_2D_FFT& FIELD_2D_FFT::operator*=(const FIELD_2D_FFT& m)
{
  assert(m.xRes() == this->xRes());
  assert(m.yRes() == this->yRes());
  for (int x = 0; x < _totalCells; x++)
  {
    double re = _frequency[x][0];
    double im = _frequency[x][1];

    _frequency[x][0] = re * m._frequency[x][0] - 
                       im * m._frequency[x][1];
    _frequency[x][1] = re * m._frequency[x][1] + 
                       im * m._frequency[x][0];
  }

  return *this;
}
