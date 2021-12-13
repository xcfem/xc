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

//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           Cam Clay yield criterion                          	 #
//#                      (Ref. Wood p113)                    			 #
//# CLASS:             CamClayYieldSurface(for monotonic loading)                    #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              Mar. 28, 2001                                             #
//# UPDATE HISTORY:                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//================================================================================


#include "material/nD/Template3Dep/CamClayYieldSurface.h"
#include "utility/matrix/nDarray/basics.h"
#include "material/nD/Template3Dep/EPState.h"


//! @brief Default constructor.
XC::CamClayYieldSurface::CamClayYieldSurface(const double &Mp)  
  :M(Mp) {} 


//! @brief Virtual constructor.
XC::YieldSurface *XC::CamClayYieldSurface::getCopy(void)
  { return new CamClayYieldSurface(*this); }

//================================================================================
// Copy constrstructor
//================================================================================
//
//XC::CamClayYieldSurface::CamClayYieldSurface(CamClayYieldSurface &) { }
//


//! @brief  Yield criterion evaluation function F(EPState)
double XC::CamClayYieldSurface::f(const XC::EPState *EPS) const
  {
    const double p =EPS->getStress().p_hydrostatic();
    const double q =EPS->getStress().q_deviatoric();
	   
    const double po = EPS->getScalarVar(1);
    const double temp3 = q*q - M*M*p*(po - p);

    //printf("\n========Inside f = %.4f \n ", temp3);
    return temp3;
  }


//! @brief BJtensor dF/dsigma_ij
XC::BJtensor XC::CamClayYieldSurface::dFods(const XC::EPState *EPS) const
  {
    BJtensor dFoverds( 2, def_dim_2, 0.0);
    BJtensor I2("I", 2, def_dim_2);

    double p = EPS->getStress().p_hydrostatic();
    double q = EPS->getStress().q_deviatoric();
    double po = EPS->getScalarVar( 1 );
    BJtensor DpoDs = EPS->getStress().dpoverds();
    BJtensor DqoDs = EPS->getStress().dqoverds();

    double dFoverdp = -1.0*M*M*( po - 2.0*p );
    double dFoverdq = 2.0*q;

    dFoverds = DpoDs  * dFoverdp +
	       DqoDs  * dFoverdq;
    return dFoverds;
  }


//! @brief double xi_s1 = dF/dpo = -M*M*p  Derivative in terms of first scalar var 
double XC::CamClayYieldSurface::xi_s1( const XC::EPState *EPS ) const
  {
    //double p = EPS->getStress().Iinvariant1()/3.0;
    const double p = EPS->getStress().p_hydrostatic();
    return -1.0 * M * M * p;
  }

//! @brief Return the slope of critical state line.
double XC::CamClayYieldSurface::getM() const
  { return M; }

//! @brief Set the slope of critical state line.
void XC::CamClayYieldSurface::setM(const double &d)
  { M= d; }

//! @brief Print stuff.
std::ostream& XC::operator<<(std::ostream &os, const XC::CamClayYieldSurface &YS)
  {
    os << "Cam Clay Yield Surface Parameters: " << std::endl
       << "M = " << YS.getM() << std::endl;
    return os;
  }


