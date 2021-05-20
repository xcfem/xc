//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
#include <cstdlib>
#include <cmath>
//#include "global.h"
#include "root.h"
// #include "matemat/matem.h"
// #include "matemat/matrices/m_double.h"
// #include "matemat/matrices/m_int.h"
// #include "matemat/matrices/solver_lu.h"

#define SIGN(i) (((i) == 0) ? 1 : -1)

int NewtonApprox(double *root, double tolerance,
        int maxIter, double* params,
        double (*f)(double, double*))
{
  int iter = 0;
  double h, diff;

  do {
     h = (fabs(*root) > 1) ? 0.01 * *root : 0.01;
     /* calculate guess refinement */
     diff = 2 * h * (*f)(*root, params) /
     ((*f)(*root + h, params) - (*f)(*root - h, params));
     /* update guess */
     *root -= diff;
     iter++;
  } while (iter <= maxIter && fabs(diff) > tolerance);
  return (fabs(diff) <= tolerance) ? 1 : 0;
}

int NewtonExact(double *root, double tolerance,
        int maxIter, double* params,
        double (*f)(double, double*),
        double (*fDeriv)(double, double*))
{
  int iter = 0;
  double diff;

  do {
     /* calculate guess refinement */
     diff = (*f)(*root, params) / (*fDeriv)(*root, params);
     /* update guess */
     *root -= diff;
     iter++;
  } while (iter <= maxIter && fabs(diff) > tolerance);
  return (fabs(diff) <= tolerance) ? 1 : 0;
}

int RichmondApprox(double *root, double tolerance,
        int maxIter, double* params,
        double (*f)(double, double*))
{
  int iter = 0;
  double h, diff, f1, f2, f3, fd1, fd2;

  do {
     h = (fabs(*root) > 1) ? 0.01 * *root : 0.01;
     f1 = (*f)(*root - h, params);      /* f(x-h) */
     f2 = (*f)(*root, params);          /* f(x)   */
     f3 = (*f)(*root + h, params);      /* f(x+h) */
     fd1 = (f3 - f1) / (2 * h);         /* f'(x)  */
     fd2 = (f3 - 2 * f2 + f1) / sqr(h); /* f''(x) */
     /* calculate guess refinement */
     diff = f1 * fd1 / (sqr(fd1) - 0.5 * f1 * fd2);
     /* update guess */
     *root -= diff;
     iter++;
  } while (iter <= maxIter && fabs(diff) > tolerance);
  return (fabs(diff) <= tolerance) ? 1 : 0;
}

int RichmondExact(double *root, double tolerance,
                  int maxIter, double* params,
                  double (*f)(double, double*),
                  double (*f1stDeriv)(double, double*),
                  double (*f2ndDeriv)(double, double*))
{
  int iter = 0;
  double diff, f1, fd1, fd2;

  do {
     f1 = (*f)(*root, params);
     fd1 = (*f1stDeriv)(*root, params);
     fd2 = (*f2ndDeriv)(*root, params);
     /* calculate guess refinement */
     diff = f1 * fd1 / (sqr(fd1) - 0.5 * f1 * fd2);
     /* update guess */
     *root -= diff;
     iter++;
  } while (iter <= maxIter && fabs(diff) > tolerance);
  return (fabs(diff) <= tolerance) ? 1 : 0;
}

int Combined(double low, double high, double tolerance,
             double* root, int maxIter, double* params,
             double (*f)(double, double*))
{
  int iter = 0;
  double h, diff;

  do {
     iter++;
     h = (fabs(*root) > 1) ? 0.01 * *root : 0.01;
     /* calculate guess refinement */
     diff = 2 * h * (*f)(*root, params) /
     ((*f)(*root + h, params) - (*f)(*root - h, params));
     *root -= diff;
     /* check if Newton's method yields a refined guess
     outside the range [low, high] */
     if (*root < low || *root > high) {
       /* apply Bisection method for this iteration */
       *root = (low + high) / 2;
       if ((*f)(*root, params) * (*f)(high, params) > 0)
         high = *root;
       else
         low = *root;
     }
  } while (iter <= maxIter && fabs(diff) > tolerance);
  return (fabs(diff) <= tolerance) ? 1 : 0;
}

int Brent(double low, double high, double tolerance,
         double* root, int maxIter, double* params,
         double (*f)(double, double*))
{
  const double SMALL = 1.0e-7; /* epsilon */
  const double VERY_SMALL = 1.0e-10; /* near zero */

  double a = low;
  double b = high;
  double c = high;
  double fa = (*f)(low, params);
  double fb = (*f)(high, params);
  double fc = fb;

  /* check that the guesses contain the root */
  if((fa * fb) > 0)
    return 0; /* bad guesses */

  int iter = 1;
  double d=0.0, e=0.0, tol, small1, small2;
  double p=0.0, q, r, s, xm;

  /* start loop to refine the guess for the root */
  while (iter++ <= maxIter)
    {
      if((fb * fc) > 0)
        {
          c = a;
          fc = fa;
          e = b - a;
          d = e;
        }
      if(fabs(fc) < fabs(fb))
        {
          a = b; b = c;
          c = a; fa = fb;
          fb = fc;
          fc = fa;
        }
      tol = 2 * SMALL * fabs(b) + tolerance / 2;
      xm = (c - b) / 2;
      if(fabs(xm) <= tol || fabs(fb) <= VERY_SMALL)
        {
          *root = b;
          return 1;
        }
      if(fabs(e) >= tol && fabs(fa) > fabs(fb))
        {
          /* perform the inverse quadratic interpolation */
          s= fb / fa;
          if(fabs(a - c) <= VERY_SMALL)
            {
              p = 2 * xm * s;
              q = 1 - s;
            }
          else
            {
              q = fa / fc;
              r = fb / fc;
              q = s * (2 * xm * q * (q - r) - (b - a) * (r - 1));
              q = (q - 1) * (r - 1) * (s - 1);
            }
          /* determine if improved guess is inside
             the range */
          q = (p <= 0) ? q : -q;
          p = fabs(p);
          small1 = 3 * xm * q * fabs(tol * q);
          small2 = fabs(e * q);
          if(2 * p < ((small1 < small2) ? small1 : small2))
            {
              /* interpolation successfull */
              e = d;
              d = p / q;
            }
          else
            {
              /* use bisection because the interpolation
                 did not succeed */
              d = xm;
              e = d;
            }
        }
      else
        {
          /* use bisection because the range
             is slowly decreasing */
          d = xm;
          e = d;
        }
      /* copy most recent guess to variable a */
      a = b;
      fa = fb;
      /* evaluate improved guess for the root */
      if(fabs(d) > tol)
        b+= d;
      else
        b+= (xm > 0) ? fabs(tol) : -fabs(tol);
      fb = (*f)(b, params);
    }
  return 0;
}


int NewtonMultiRoots( double* roots, int* numRoots, int maxRoots,
                      double tolerance, int maxIter, double* params,
                      double (*f)(double, double*))
  {
    int iter, i;
    double h, diff, f1, f2, f3, root;

    *numRoots = 0;
    root = *roots;

    do
      {
        iter = 0;
        do
          {
            h = (fabs(root) > 1) ? 0.01 * root : 0.01;
            f1 = (*f)(root - h, params);
            f2 = (*f)(root, params);
            f3 = (*f)(root + h, params);
            if (*numRoots > 0)
              {
                for (i = 0; i < *numRoots; i++)
                  {
                    f1 /= (root - h - *(roots + i));
                    f2 /= (root - *(roots + i));
                    f3 /= (root + h - *(roots + i));
                  }
              }
            /* calculate guess refinement */
            diff = 2 * h * f2 / (f3 - f1);
            /* update guess */
            root -= diff;
            iter++;
          }
        while (iter <= maxIter && fabs(diff) > tolerance);
        if (fabs(diff) <= tolerance)
          {
            *(roots + *numRoots) = root;
            *numRoots += 1;
            if (root < 0)
              root *= 0.95;
            else
              if (root > 0)
                root *= 1.05;
              else
                root = 0.05;
          }
      }
    while (iter <= maxIter && *numRoots < maxRoots);
    return (*numRoots > 0) ? 1 : 0;
  }

int DeflatePolyRoots(double* coeff, double initGuess,
                 double* roots, int* numRoots,
                 int polyOrder, int maxIter,
                 double tolerance)
{
  double* a;
  double* b;
  double* c;
  double diff;
  double z, x = initGuess;
  int i;
  int iter = 1;
  int n = polyOrder + 1;
  *numRoots = 0;

  /* allocate dynamic coefficients */
  a = (double*)malloc(n * sizeof(double));
  if (!a) /* allocation failed? */
     return 0;
  b = (double*)malloc(n * sizeof(double));
  if (!b) { /* allocation failed? */
     free(a);
     return 0;
  }
  c = (double*)malloc(n * sizeof(double));
  if (!c) { /* allocation failed? */
     free(a);
     free(b);
     return 0;
  }

  for (i = 0; i < n; i++)
     *(a + i) = *(coeff + i);

  while (iter++ <= maxIter && n > 1)
    {
      z = x;
      *(b + n - 1) = *(a + n - 1);
      *(c + n - 1) = *(a + n - 1);
      for (i = n - 2; i > 0; i--)
	{
	  *(b + i) = *(a + i) + z * *(b + i + 1);
	  *(c + i) = *(b + i) + z * *(c + i + 1);
        }
      *b = *a + z * *(b + 1);
      diff = *b / *(c + 1);
      x -= diff;
      if(fabs(diff) <= tolerance)
	{
	  iter = 1; /* reset iteration counter */
	  n--;
	  *(roots + *numRoots) = x;
	  *numRoots += 1;
	  /* update deflated roots */
	  for(i = 0; i < n; i++)
	    *(a + i) = *(b + i + 1);
	  /* get the last root */
	  if(n == 2)
	    {
	      n--;
	      *(roots + *numRoots) = -(*a);
	      *numRoots += 1;
	    }
	}
    }
  /* deallocate dynamic arrays */
  free(a);
  free(b);
  free(c);
  return 1;
}

int LBPolyRoots(double* coeff, polyRoot* roots,
                int polyOrder, double tolerance)
/*
  solves for the roots of the following polynomial

     y = coeff[0] + coeff[1] X + coeff[2] X^2 +...+ coeff[n] X^n

Parameters:

   coeff      must be an array with at least polyOrder+1 elements.

   roots      output array of roots

   polyOrder  order of polynomial

   tolerance  tolerance of solutions

*/
{
  const double SMALL = 1.0e-8;
  double* a;
  double* b;
  double* d;
  double alfa1, alfa2;
  double beta1, beta2;
  double delta1, delta2, delta3;
  int i, j, k;
  int count;
  int n = polyOrder;
  int n1 = n + 1;
  int n2 = n + 2;
  int arraySize = n2 * sizeof(double);

  /* is the coefficient of the highest term zero? */
  if (fabs(coeff[0]) < SMALL)
    return 0;

  /* allocate dynamic coefficients */
  a = (double*) malloc(arraySize);
  if (!a) /* allocation failed? */
     return 0;
  b = (double*) malloc(arraySize);
  if (!b) { /* allocation failed? */
     free(a);
     return 0;
  }
  d = (double*) malloc(arraySize);
  if (!d) { /* allocation failed? */
     free(a);
     free(b);
     return 0;
  }

  for (i = 0; i < n1; i++)
     a[n1-i] = coeff[i];

  /* is highest coeff not close to 1? */
  if (fabs(a[1]  - 1) > SMALL) {
     /* adjust coefficients because a[1] != 1*/
     for (i = 2; i < n2; i++)
       a[i] /= a[1];
     a[1] = 1.0;
  }

  /* initialize root counter */
  count = 0;

  do {
     /*
     start the main Lin-Bairstow iteration loop
     initialize the counter and guesses for the
     coefficients of quadratic factor:

     p(x) = x^2 + alfa1 * x + beta1
     */

     alfa1 = 1;
     beta1 = 1;

     do {

       b[0] = 0;
       d[0] = 0;
       b[1] = 1;
       d[1] = 1;

       for (i = 2, j = 1, k = 0; i < n2; i++, j++, k++) {
         b[i] = a[i] - alfa1 * b[j] - beta1 * b[k];
         d[i] = b[i] - alfa1 * d[j] - beta1 * d[k];
       }
       j = n - 1;
       k = n - 2;
       delta1 = sqr(d[j]) - (d[n] - b[n]) * d[k];
       alfa2 = (b[n] * d[j] - b[n1] * d[k]) / delta1;
       beta2 = (b[n1] * d[j] - (d[n] - b[n]) * b[n]) / delta1;
       alfa1 += alfa2;
       beta1 += beta2;
     } while (fabs(alfa2) > tolerance ||
              fabs(beta2) > tolerance);

     delta1 = sqr(alfa1) - 4 * beta1;

     if (delta1 < 0) {
       /* imaginary roots */
       delta2 = sqrt(fabs(delta1)) / 2;
       delta3 = -alfa1 / 2;
       for (i = 0; i < 2; i++) {
         roots[count+i].isComplex = 1;
         roots[count+i].real = delta3;
         roots[count+i].imag = SIGN(i) * delta2;
       }
     }
     else {
       delta2 = sqrt(delta1);
       /* roots are real */
       for (i = 0; i < 2; i++) {
         roots[count+i].isComplex = 0;
         roots[count+i].imag = 0;
       }
       roots[count].real   = (delta2 - alfa1) / 2;
       roots[count+1].real = (delta2 + alfa1) / (-2);

     }
     /* update root counter */
     count += 2;

     /* reduce polynomial order */
     n -= 2;
     n1 -= 2;
     n2 -= 2;

     /* for n >= 2 calculate coefficients of
        the new polynomial */
     if (n >= 2)
       for (i = 1; i < n2; i++)
         a[i] = b[i];

  } while (n >= 2);

  if (n == 1) { /* obtain last single real root */
    roots[count].isComplex = 0;
    roots[count].real = -b[2];
    roots[count].imag = 0;
  }

  /* deallocate dynamic arrays */
  free(a);
  free(b);
  free(d);

  return 1;
}


int Newton2Functions(double *rootX, double* rootY,
                double tolerance, int maxIter,
                double* paramsX, double* paramsY,
                double (*fx)(double, double, double*),
                double (*fy)(double, double, double*))
{
  double Jacob;
  double fx0, fy0;
  double hX, hY;
  double diffX, diffY;
  double fxy, fxx, fyy, fyx;
  double x = *rootX;
  double y = *rootY;
  int iter = 1;

  do {
     hX = (fabs(x) < 1) ? 0.01 : 0.01 * x;
     hY = (fabs(y) < 1) ? 0.01 : 0.01 * y;
     fx0 = (*fx)(x, y, paramsX);
     fy0 = (*fy)(x, y, paramsY);
     fxx = ((*fx)(x + hX, y, paramsX) -
          (*fx)(x - hX, y, paramsX)) / 2 / hX;
     fyx = ((*fy)(x + hX, y, paramsY) -
          (*fy)(x - hX, y, paramsY)) / 2 / hX;
     fxy = ((*fx)(x, y + hY, paramsX) -
          (*fx)(x, y - hY, paramsX)) / 2 / hY;
     fyy = ((*fy)(x, y + hY, paramsY) -
          (*fy)(x, y - hY, paramsY)) / 2 / hY;
     Jacob = fxx * fyy - fxy * fyx;
     diffX = (fx0 * fyy - fy0 * fxy) / Jacob;
     diffY = (fy0 * fxx - fx0 * fyx) / Jacob;
     x -= diffX;
     y -= diffY;
     } while (iter++ <= maxIter &&
         (fabs(diffX) > tolerance ||
          fabs(diffY) > tolerance));

  *rootX = x;
  *rootY = y;
  return (fabs(diffX) <= tolerance &&
         fabs(diffY) <= tolerance) ? 1 : 0;

}


