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
                                                                        
// $Revision: 1.9 $
// $Date: 2003/03/05 00:53:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Concrete01.h,v $
                                                                        
                                                                        
#ifndef Concrete01_h
#define Concrete01_h

// File: Concrete01.h
//
// Written: MHS 
// Created: 06/99
// Revision: A
//
//
// What: "@(#) Concrete01.h, revA"


#include <material/uniaxial/concrete/ConcreteBase.h>
#include <utility/matrix/Matrix.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Uniaxial Kent-Scott-Park concrete model with linear
//! unloading/reloading according to the work of Karsan-Jirsa and no
//! strength in tension. The model contains a compressive strength of fpc,
//! a strain at the compressive strength of epsc0, a crushing strength of
//! fpcu, and a strain at the crushing strength of epscu. Compressive
//! concrete parameters should be input as negative numeric values for
//! this model to behave properly. Specification of minimum and maximum
//! failure strains through the -min and -max switches is optional. The
//! argument matTag is used to uniquely identify the material object among
//! material objects in the BasicBuilder object.
class Concrete01: public ConcreteBase
  {
  private:
      /*** Material Properties ***/
    double fpcu; //!< Crushing strength

    void make_negative(void);
    void setup_parameters(void);
    void determineTrialState(double dStrain);

    void reload(void);
    void unload(void);
    void envelope(void);

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
    Matrix SHVs;
// AddingSensitivity:END ///////////////////////////////////////////
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    void calcula_trial_state(const double &strain);
  public:
    Concrete01(int tag, double fpc, double eco, double fpcu, double ecu);
    Concrete01(int tag);
    Concrete01(void);
    ~Concrete01(void);

    int setTrialStrain(double strain, double strainRate = 0.0); 
    int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0);

    //! @brief Returns initial tangent stiffness.
    inline double getInitialTangent(void) const
      {return 2.0*fpc/epsc0;}

    void setFpcu(const double &d);
    double getFpcu(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0) const;


// AddingSensitivity:BEGIN //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    double getStressSensitivity(int gradNumber, bool conditional);
    int    commitSensitivity(const double &strainGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace


#endif


