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
// $Revision: 1.8 $
// $Date: 2003/02/14 23:01:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/MultiYieldSurface.cpp,v $
                                                                        
// Written: ZHY
// Created: August 2000
//
// MultiYieldSurface.cpp
// ---------------------
//

#include <cmath>
#include <cstdlib>
#include <material/nD/soil/MultiYieldSurface.h>


// YieldSurface class methods
XC::MultiYieldSurface::MultiYieldSurface():
theSize(0.0), theCenter(6), plastShearModulus(0.0)
  {}

XC::MultiYieldSurface::MultiYieldSurface(const XC::Vector & theCenter_init, 
                                     double theSize_init, double plas_modul):
theSize(theSize_init), theCenter(theCenter_init), plastShearModulus(plas_modul)
  {}

void XC::MultiYieldSurface::setData(const XC::Vector & theCenter_init, 
                                double theSize_init, double plas_modul)
  {
    theSize = theSize_init;
    theCenter = theCenter_init;
    plastShearModulus = plas_modul;
  }

void XC::MultiYieldSurface::setCenter(const XC::Vector & newCenter)
  {
    if(newCenter.Size() != 6)
      {
	std::cerr << "FATAL:XC::MultiYieldSurface::setCenter(XC::Vector &): vector size not equal 6" << std::endl;
        exit(-1);
      }
    theCenter = newCenter;
  }


/**********************************************
ostream & operator<< (ostream & os, const XC::MultiYieldSurface & a)
{
  os << "  theSize = " << a.theSize << std::endl 
     << "  theCenter = " << a.theCenter << std::endl
     << "  plastShearModulus = " << a.plastShearModulus << std::endl;
  
  return os;
}


istream & operator>> (istream & is, MultiYieldSurface & a)
{
  is >> a.theSize >> a.theCenter >> a.plastShearModulus;

  return is;
}
*********************************************/

double secondOrderEqn(double A, double B, double C, int i)
  {
    if(A == 0)
      {
	std::cerr << "FATAL:second_order_eqn: A=0." << std::endl;
	if(i==0) std::cerr << " when finding reference point on outer surface."                           << std::endl;
	else
	  std::cerr << " when moving active surface." << std::endl;
        exit(-1);
      }
    if(C == 0) return 0;
    if(B == 0)
      {
        if(C/A > 0)
          {
	    std::cerr << "FATAL:second_order_eqn: Complex roots.\n";
            exit(-1);
          }
        return sqrt(-C/A);
      }

    double determ, val1, val2, val;
    determ = B*B - 4.*A*C; 
    if(determ < 0)
      {
        std::cerr << "FATAL:second_order_eqn: Complex roots.\n";
        if(i==0)
          std::cerr << " when finding reference point on outer surface." <<std::endl;
        else
          std::cerr << " when moving active surface." <<std::endl;
        std::cerr << "B2=" << B*B << " 4AC=" << 4.*A*C <<std::endl; 
        exit(-1);
      }
  
    if(B > 0)
      val1 = (-B - sqrt(determ)) / (2.*A);
    else
      val1 = (-B + sqrt(determ)) / (2.*A);
    val2 = C / (A * val1);

    if(val1 < 0 && val2 < 0)
      {
	if(fabs(val1) < LOW_LIMIT)
          val1 = 0.;
        else
          if(fabs(val2) < LOW_LIMIT) val2 = 0.;
      }
    if(val1 < 0 && val2 < 0)
      {
        std::cerr << "FATAL:second_order_eqn: Negative roots.\n";
        if(i==0)
          std::cerr << " when finding reference point on outer surface." <<std::endl;
        else
          std::cerr << " when moving active surface." <<std::endl;
	std::cerr << "A=" << A << " B=" << B << " C=" << C << " det=" << determ << 
			" x1=" << val1 << " x2=" << val2 << std::endl;  
        exit(-1);   
      }
    if(val1 < 0)
      return  val2;
    else if(val2 < 0)
      return  val1;
    else
      {
        val = val1;
        if(val > val2)
          val = val2;
	return val;
      }
  }
