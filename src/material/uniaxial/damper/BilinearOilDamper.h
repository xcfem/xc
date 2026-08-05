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
                                                                        
// $Revision: 0 $
// $Date: May 2015 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/BilinearOilDamper.h,v $
                                                                        
// Written: Sarven Akcelyan and Dimitrios G. Lignos, PhD, McGill University
// Created: May 2015
// Updated: May 2015
// Revision: A

// Description: This file contains the class interface for 
// Oil Damper Model Relationship of the form  before relief valve ==> F = K*u_s = C*V_d  after relief valve ==> F= K*u_s = Fr + p*C*(V_d-Fr/C)
//
// References: 
// Akcelyan, S., and Lignos, D.G. (2015), “Adaptive Numerical Method Algorithms for Nonlinear Viscous and Bilinear Oil Damper Models Under Random Vibrations”, ASCE Journal of Engineering Mechanics, (under review)
// Kasai, K., Takahashi, O., and Sekiguchi, Y. (2004). "JSSI manual for building passive control technology part-10 time-history analysis model for nonlinear oil dampers." Proc., The 13th World Conference on Earthquake Engineering, Vancouver, B.C., Canada.

// Variables:
// $K: Elastic stiffness of linear spring (to model the axial flexibility of a viscous damper (brace and damper portion)
// $C: Viscous damping coefficient of the damper
// $Fr: Relief load
// $p: post-relief viscous damping coefficient ratio, (p=C2/C)
// $LGap: gap length to simulate the gap length due to the pin tolerance
// $NM:	Employed adaptive numerical algorithm (default value NM = 1; 1 = Dormand-Prince54, 2 = Finite differences)
// $RelTol: Tolerance for absolute relative error control of the adaptive iterative algorithm (default value 10^-6)
// $AbsTol: Tolerance for absolute error control of adaptive iterative algorithm (default value 10^-10)
// $MaxHalf: Maximum number of sub-step iterations within an integration step (default value 15)

#ifndef BilinearOilDamper_h
#define BilinearOilDamper_h

#include "material/uniaxial/damper/DamperBase.h"

namespace XC {
  
//! @brief represents the Maxwell Model (linear spring and nonlinear dashpot
//! in series).

//! The ViscousDamper material simulates the hysteretic response of nonlinear
//! viscous dampers. An adaptive iterative algorithm has been implemented and
//! validated to solve numerically the constitutive equations within a
//! nonlinear viscous damper with a high-precision accuracy.
//! @ingroup MatUnx
class BilinearOilDamper : public DamperBase
  {
  private:
    // Fixed Input Material Variables
    double Fr; //!< Damper relief load (default=1.0, Damper property).
    double p; //!< Post-relief viscous damping coefficient ratio (default=1.0, linear oil damper).

    double f(double v, double fd) const;    
  protected:
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public:
    BilinearOilDamper(int tag= 0); 
    BilinearOilDamper(int tag, double K, double C, double Fr= 1.0, double p= 1.0, double LGap= 0.0, double NM= 1, double RelTol= 1e-6, double AbsTol= 1e-10, double MaxHalf= 15);
    UniaxialMaterial *getCopy(void) const;

    inline void setDamperReliefLoad(const double &d)
      { this->Fr= d; }
    inline double getDamperReliefLoad() const
      { return this->Fr; }
    inline void setPostReliefViscousDamping(const double &d)
      { this->p= d; }
    inline double getDamperPostReliefViscousDamping() const
      { return this->p; }

    int setTrialStrain(double strain, double strainRate = 0.0); 
    
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
