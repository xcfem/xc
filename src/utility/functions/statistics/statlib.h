// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//Statlib.h
#ifndef STATLIB_H
#define STATLIB_H

double Q(const double &x);
// Function that returns the "Normal"
// probability distribution integral

double QInv(double x);
//  calculate the inverse normal.

double T(const double &x, double df);
/* Function will return the probability of
  obtaining a student-t statistic, x, at
  df degrees of freedom.                       */

double TInv(double x, double df);
/* double will return the value of student-t statistic for a
	given probability, x, and df degrees of freedom.          */

double Chi(const double &x, double df);
/* Function will return the probability of obtaining a
  chi-squared statistic, x, at df degrees of freedom.  */

double ChiInv(double x, double df);
/* double will return the value of chi-squared statistic for a */
/* given probability, x, and df degrees of freedom.              */

double F(const double &x, double df1, double df2);
/* Function will return the probability of obtaining an
	F statistic, x, at df degrees of freedom.             */

double FInv(double x, double df1, double df2);
/* Function will return the value of the F statistic for a
  given probability, x, and df degrees of freedom.        */


#endif
