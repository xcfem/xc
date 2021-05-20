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
//statlib.cc

#include "statlib.h"

#include <cmath>
#include "utility/utils/misc_utils/matem.h"

//! @brief Return the integral of the probability function
//! corresponding to a "Normal" distribution.
double Q(const double &x)
  {
    const double twoPi = 2*M_PI;

    double result;
    double sum; //sumatorio.
    double xp; //x elevado a una potencia entera.
    double tempo;
    //Coefficient matrix.
    double b[5] = { 0.319381530, -0.356563782,1.781477937, -1.821255978,1.330274429 };

    tempo = 1.0 / ( 1.0 + 0.2316419 * fabs(x));
    /* Initialize summation */
    sum = 0.0;
    xp = 1.0;
    /* Loop to obtain summation term */
    for (size_t i= 0; i < 5; i++)
      {
	xp*= tempo; /* Update power factor */
	sum += b[i] * xp;
      }
    /* Calculate result */
    result = (exp(-x * x / 2.0) / sqrt(twoPi) * sum);
    result = (x >= 0.0) ? 1.0 - result : result;
    return result;
  }

inline double cheq_lim_x(const double &x)
  {
    //Chequeamos los limites de x
    if (x <= 0.0) return 0.0001;
    if (x >= 1.0)
      return 0.9999;
    else
      return x;
  }

double QInv(double x)
//  calculate the inverse normal.
  {
    double result;
    double sum1, sum2; //Se usan dos sumatorios.
    double tempo, xp;
    //Matrices de coeficientes primera y segunda.
    double c[4] = { 2.515517, 0.802853,0.010328, 0.0 };
    double d[4] = { 1.0, 1.432788, 0.189269, 0.001308 };

    //Chequeamos los limites de x
    x= cheq_lim_x(x);

    if (x <= 0.5)
      tempo = sqrt(log(1.0/sqr(x)));
    else
      tempo = sqrt(log(1.0/sqr(1.0 - x)));

    //Inicializa los sumatorios
    sum1 = 0.0;
    sum2 = 0.0;
    xp = 1.0;
    //Inicio del bucle para calcular sumas.
    for(size_t i = 0; i <4; i++)
      {
	sum1 += c[i] * xp;
	sum2 += d[i] * xp;
	xp *= tempo; /* Update power factor */
      }
    //Calcula el resultado.
    result = tempo - sum1 / sum2;
    result = (x > 0.5) ? -result : result;
    return result;
  }

double T(const double &x, double df)
/* Function will return the probability of
  obtaining a student-t statistic, x, at
  df degrees of freedom.                       */

  {
    double xt;
    xt = x * (1.0 - 0.25 / df) / sqrt(1.0 + sqr(x) / 2.0 / df);
    return (1.0 - Q(xt));
  }

double TInv(double x, double df)
/* double will return the value of student-t statistic for a
	given probability, x, and df degrees of freedom.          */

  {
    double sum, xp, xq;
    double Pwr[10]; /* Array of powers of xq */
    double term[5]; /* Array of terms */
    int i;

    //Chequeamos los límites de x.
    x= cheq_lim_x(x);

    xq = QInv(x);
    Pwr[1] = xq;

    //Loop to compute matrix of powers.
    for(i = 2; i <= 9; i++)
      Pwr[i] = Pwr[i-1] * xq;

    //Calcula los cuatro términos.
    term[1] = 0.25 * (Pwr[3] + Pwr[1]);
    term[2] = (5*Pwr[5] + 16*Pwr[3] + 3*Pwr[1])/96;
    term[3] = (3*Pwr[7] + 19*Pwr[5] + 17*Pwr[3] - 15*Pwr[1])/384;
    term[4] = (79*Pwr[9] + 776*Pwr[7] + 1482*Pwr[5] - 1920*Pwr[3] - 945*Pwr[1])/92160.0;

    //Inicializa el sumatorio y el exponente.
    sum = xq;
    xp = 1;
    //Bucle para añadir términos.
    for (i = 1; i <= 4; i++)
      {
        xp*= df; /* Actualiza el factor df */
        sum += term[i] / xp;
      }
    return sum;
  }

double Chi(const double &x, double df)
/* Function will return the probability of obtaining a
  chi-squared statistic, x, at df degrees of freedom.  */
{
  double k, xq;

  k = 2.0 / 9.0 / df;
  xq = (pow((x/df), 0.33333) - (1.0 - k))/ sqrt(k);
  return (1.0 - Q(xq));
}

double ChiInv(double x, double df)
/* double will return the value of chi-squared statistic for a */
/* given probability, x, and df degrees of freedom.              */

  {
    double k, tempo, xq;
    /* Check limits of x */
    x= cheq_lim_x(x);

    k = 2.0 / 9.0;
    xq = QInv(x);
    tempo = 1.0 - k / df + xq * sqrt(k / df);
    tempo = df * sqr(tempo)*tempo;
    return tempo; /* Return sought value */
  }

double F(const double &x, double df1, double df2)
/* Function will return the probability of obtaining an
	F statistic, x, at df degrees of freedom.             */

{
  double f1, f2, k;

  k = 2.0 / 9.0;
  f1 = pow(x, 0.333334) * (1 - k / df2) - (1 - k / df1);
  f2 = sqrt(k / df1 + pow(x, 0.666667) * k / df2);
  return 1.0 - Q(f1 / f2);
}

double FInv(double x, double df1, double df2)
/* Function will return the value of the F statistic for a
  given probability, x, and df degrees of freedom.        */

  {
    double alpha, beta, gamma, delta,A, B, C, k, result, xq;

    /* Check limits of x */
    x= cheq_lim_x(x);

    k = 2.0 / 9.0;
    xq = QInv(x);
    alpha = (1 - k / df2);
    beta = (1 - k /df1);
    gamma = 1.0 - (1 - k / df1);
    delta = 1.0 - (1 - k / df2);
    A = sqr(alpha) - sqr(xq) * delta;
    B = -2.0 * alpha * beta;
    C = sqr(beta) - sqr(xq) * gamma;

    result = (-1.0 * B + sqrt(B * B - 4.0 * A * C))/(2.0 * A);
    return sqr(result)*result;
  }
