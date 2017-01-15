//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
// $Revision: 1.7 $
// $Date: 2003/02/14 23:01:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/MultiYieldSurface.h,v $
                                                                        
// Written: ZHY
// Created: August 2000
//
// MultiYieldSurface.h
// -------------------
//

#ifndef _MultiYieldSurface_H_
#define _MultiYieldSurface_H_

#include <material/nD/soil/T2Vector.h>


#define LOCK_VALUE  1.0e+30

// global function to find the roots of a second order equation
double secondOrderEqn(double A, double B, double C, int i);

// define yield surface in stress space
namespace XC {
//! @ingroup SoilNDMat
//
//! @brief ??.
class MultiYieldSurface
  {
  private:
    double theSize;
    Vector theCenter;  
    double plastShearModulus;
  public:
    //constructors
    MultiYieldSurface();
    MultiYieldSurface(const Vector & center_init, double size_init, 
                    double plas_modul); 

  void setData(const Vector & center_init, double size_init,double plas_modul); 
  const Vector & center() const {return theCenter; }
  double size() const {return theSize; }
  double modulus() const {return plastShearModulus; }
  void  setCenter(const Vector & newCenter); 

  // friend ostream & operator<< (ostream & os, const MultiYieldSurface & );  
  // friend istream & operator>> (istream & is, MultiYieldSurface & );

};
} // end of XC namespace

#endif
