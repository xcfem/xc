#include "utility/utils/misc_utils/matem.h"
#include <iostream>

double dicot_zero(double f(double), double a, double b,double &err)
  {
    double epsi= err;
    enum {inf=0,sup=1} modif;
    double c;
    double xx= 0.0;
    double x[2];
    x[0]= a;
    x[1]= b;
    if (f(a) > 0)
      c=1;
    else
      c=-1;
    err= x[sup]-x[inf];
    while ( err > epsi )
      {
	xx = 0.5*(x[inf]+x[sup]);
	if ( (c*f(xx)) > 0)
	  modif= inf;
	else
	  modif= sup;
	x[modif]= xx;
	err= x[sup]-x[inf];
      }
    return xx; 
  }

//! @brief eturns True if a is close in value to b. False otherwise
//! @param a: one of the values to be tested
//! @param b: the other value to be tested
//! @param rel_tol=1e-9: The relative tolerance -- the amount of error
//!                      allowed, relative to the absolute value of the
//!                      larger input value.
//! @param abs_tol=0.0: The minimum absolute tolerance level -- useful
//!                     for comparisons to zero.

// NOTES:

//     -inf, inf and NaN behave similarly to the IEEE 754 Standard. That
//     is, NaN is not close to anything, even itself. inf and -inf are
//     only close to themselves.

//     The function can be used with any type that supports comparison,
//     substratcion and multiplication, including Decimal, Fraction, and
//     Complex

//     Complex values are compared based on their absolute value.

//     See PEP-0485 for a detailed description
bool isclose(const double &a, const double &b, const double rel_tol, const double abs_tol)
  {
    if(a == b)  // short-circuit exact equality
      return true;

    if(rel_tol < 0.0 or abs_tol < 0.0)
      std::cerr << __FUNCTION__
		<< "; error tolerances must be non-negative."
		<< std::endl;

    if(std::isinf(std::abs(a)) or std::isinf(std::abs(b)))
        // This includes the case of two infinities of opposite sign, or
        // one infinity and one finite number. Two infinities of opposite sign
        // would otherwise have an infinite relative tolerance.
      return false;
    const double diff= std::abs(b - a);

    return (((diff <= std::abs(rel_tol * b)) or
	     (diff <= std::abs(rel_tol * a))) or
            (diff <= abs_tol));
  }
