// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: C $
// $Date: May 2015 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ViscousDamper.cpp,v $
                                                                        
// Written: Sarven Akcelyan and Dimitrios G. Lignos, PhD, McGill University
// Created: January 2013
// Updated: May 2015
// Revision: C
//
// Description: This file contains the class interface for 
// Viscous Damper Model Relationship of the form F = K*u_s = C*pow(V_d,alpha)
// Reference: 
// Akcelyan, S., and Lignos, D.G. (2015), “Adaptive Numerical Method Algorithms for Nonlinear Viscous and Bilinear Oil Damper Models Under Random Vibrations”, ASCE Journal of Engineering Mechanics, (under review)
// Kasai K, Oohara K. (2001). “Algorithm and Computer Code To Simulate Response of Nonlinear Viscous Damper”. Proceedings Passively Controlled Structure Symposium 2001, Yokohama, Japan.

// Variables:
// $K: Elastic stiffness of linear spring (to model the axial flexibility of a viscous damper (brace and damper portion)
// $C: Viscous damping coefficient of the damper
// $Alpha: Viscous damper exponent
// $LGap: gap length to simulate the gap length due to the pin tolerance
// $NM:	Employed adaptive numerical algorithm (default value NM = 1; 1 = Dormand-Prince54, 2=6th order Adams-Bashforth-Moulton, 3=modified Rosenbrock Triple)
// $RelTol:	Tolerance for absolute relative error control of the adaptive iterative algorithm (default value 10^-6)
// $AbsTol:	Tolerance for absolute error control of adaptive iterative algorithm (default value 10^-6)
// $MaxHalf: Maximum number of sub-step iterations within an integration step, h=dt*(0.5)^MaxHalf (default value 15)

#ifndef ViscousDamper_h
#define ViscousDamper_h

#include "material/uniaxial/damper/DamperBase.h"

namespace XC {

//! @brief Material which simulates the hysteretic response of bilinear
//! oil dampers with relief valve.
//!
//! Material which simulates the hysteretic response of bilinear
//! oil dampers with relief valve. Two adaptive iterative algorithms have
//! been implemented and validated to solve numerically the constitutive
//! equations within a bilinear oil damper with a high-precision accuracy. 
//! @ingroup MatUnx
class ViscousDamper : public DamperBase
  {
  private:
    // Fixed Input Material Variables
    double Alpha; //!< Velocity exponent.
  protected:
    int sendData(Communicator &);  
    int recvData(const Communicator &);    
  public: 
    ViscousDamper(int tag= 0); 
    ViscousDamper(int tag, double K, double C, double Alpha, double LGap= 0.0, double NM= 1, double RelTol= 1e-6, double AbsTol= 1e-10, double MaxHalf= 15);   

    inline void setVelocityExponent(const double &d)
      { this->Alpha= d; }
    inline double getVelocityExponent() const
      { return this->Alpha; }
    
    int setTrialStrain(double strain, double strainRate); 

    int ABM6(double vel0, double vel1, double y0, double h, double& yt, double& eps, double& error) const;
    int ROS(double vel0, double vel1, double y0, double h, double& y2, double& eps, double& error) const;
    double f(double v, double fd) const;

        
    UniaxialMaterial *getCopy(void) const;

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;
    
    // //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    // AddingSensitivity:END
    // ///////////////////////////////////////////
  };
} // end of XC namespace

#endif
