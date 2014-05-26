#ifndef FIELD_2D_FFT_H
#define FIELD_2D_FFT_H

#include "FIELD_2D.h"
#include <fftw3.h>

using namespace std;

class FIELD_2D_FFT {
public:
  FIELD_2D_FFT();
  FIELD_2D_FFT(const FIELD_2D& m);
  ~FIELD_2D_FFT();

  void FFT(const FIELD_2D& m);
  void inverseFFT(FIELD_2D& m);

  FIELD_2D inverseFFT(const FIELD_2D& real, const FIELD_2D& imaginary);

  static void shift(FIELD_2D& field);

  const int xRes() const { return _xRes; };
  const int yRes() const { return _yRes; };
  const int totalCells() const { return _totalCells; };
  
  inline fftw_complex& operator[](int x) { return _frequency[x]; };
  FIELD_2D_FFT& operator*=(const FIELD_2D_FFT& m);

  // get the real component
  FIELD_2D real();

  // get the imaginary component
  FIELD_2D imaginary();

  // get the magnitude field
  FIELD_2D abs();

private:
  int _xRes;
  int _yRes;
  int _totalCells;
  fftw_complex* _spatial;
  fftw_complex* _frequency;

  bool _forwardInit;
  fftw_plan _forwardPlan;
  
  bool _inverseInit;
  fftw_plan _inversePlan;
};

#endif
