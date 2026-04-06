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
   
#ifndef TDConcreteMC10Base_h
#define TDConcreteMC10Base_h

#include "material/uniaxial/concrete/TDConcreteBase.h"
#include "material/uniaxial/concrete/MC10CreepSteps.h"
#include "material/uniaxial/concrete/MC10CreepShrinkageParameters.h"
#include "material/uniaxial/concrete/MC10CreepShrinkageState.h"

namespace XC {
  
class TDConcreteMC10Base : public TDConcreteBase
  {
  protected:
    // matpar : Concrete FIXED PROPERTIES
    double Ecm; //! 28-day modulus, necessary for normalizing creep coefficient.

    // hstv : Concrete HISTORY VARIABLES  current step
	
    //Added by AMK:
    MC10CreepShrinkageState creepShrinkageState;
    
    double phib_i; //!< basic phi coefficient (ntosic).
    double phid_i; //!< drying phi coefficient (ntosic).
    
    MC10CreepShrinkageParameters creepShrinkageParameters; //!< Creep and shrinkage parameters according to Model Code 10.
    MC10CreepSteps creepSteps;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    TDConcreteMC10Base(int tag, int classTag);
    TDConcreteMC10Base(int tag, int classTag, double _fc, double _ft, double _Ets, double _Ec, double _Ecm, double _beta, double _age, double _tcast, const MC10CreepShrinkageParameters &);
    void setup_parameters(void);

    double getEcm(void) const;
    void setEcm(const double &);

    void setCreepShrinkageParameters(const MC10CreepShrinkageParameters &);
    const MC10CreepShrinkageParameters &getCreepShrinkageParameters(void) const;
    
    double getPHIB_i(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double getPHID_i(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double getCreepBasic(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double getCreepDrying(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double setPhiBasic(double time, double tp) const; //Added by AMK //ntosic: split into basic and drying creep
    double setPhiDrying(double time, double tp) const; //Added by AMK //ntosic: split into basic and drying creep
    double setShrinkBasic(double time); //Added by AMK //ntosic: split into basic and drying shrinkage
    double setShrinkDrying(double time); //Added by AMK //ntosic: split into basic and drying shrinkage
    double getShrinkBasic(void) const; //Added by AMK //ntosic: split into basic and drying
    double getShrinkDrying(void) const; //Added by AMK //ntosic: split into basic and drying
    
    //! @brief Assign current concrete stiffness.
    virtual void setEt(const double &d)
      { this->creepShrinkageState.setEt(d); }
    //! @brief Returns current concrete stiffness.
    virtual double getEt(void) const
      { return this->creepShrinkageState.getEt(); }

    //! @brief Assign the concrete age at first loading.
    void setAge(const double &d)
      { this->creepShrinkageState.setAge(d); }
    //! @brief Return the concrete age at first loading.
    double getAge(void) const
      { return this->creepShrinkageState.getAge(); }
    
    //! @brief Assign the analysis time corresponding to concrete casting in days.
    void setTCast(const double &d)
      { this->creepShrinkageState.setTCast(d); }
    //! @brief Return the analysis time corresponding to concrete casting in days.
    double getTCast(void) const
      { return this->creepShrinkageState.getTCast(); }
    
    double getStrain(void) const
      { return creepShrinkageState.getStrain(); }
    
    double setCreepBasicStrain(double time, double stress); //Added by AMK //ntosic: split into basic and drying creep
    double setCreepDryingStrain(double time, double stress); //Added by AMK //ntosic: split into basic and drying creep
    
    int revertToLastCommit(void);    
    int revertToStart(void);        
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
  };

} // end of XC namespace

#endif

