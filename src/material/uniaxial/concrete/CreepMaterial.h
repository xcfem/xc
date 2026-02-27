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
   
//----------------------------------------------------------------------------------------------------------------------------
 // Developed by:
 // Michael H. Scott
 //
 // Based on TDConcrete implementations by:
 // Adam M. Knaack (adam.knaack@schaefer-inc.com) 
 // Schaefer-Inc, Cincinnati, Ohio, USA
 // Nikola D. Tosic (ntosic@imk.grf.bg.ac.rs)
 // Department for Materials and Structure, Faculty of Civil Engineering, University of Belgrade, Serbia
 // Yahya C. Kurama (ykurama@nd.edu)
 // Department of Civil and Environmental Engineering and Earth Sciences, College of Engineering, University of Notre Dame, Notre Dame, Indiana, USA
 //----------------------------------------------------------------------------------------------------------------------------

 //----------------------------------------------------------------------------------------------------------------------------
 // Description: This file contains the source code of CreepMaterial. 
 // CreepMaterial is a wrapper that imposes creep and shrinkage evoluation equations
 // to any uniaxialMaterial.
 //----------------------------------------------------------------------------------------------------------------------------

#ifndef CreepMaterial_h
#define CreepMaterial_h

#include "material/uniaxial/EncapsulatedUniaxialMaterial.h"
#include "material/uniaxial/concrete/ConcreteHistoryVars.h"
#include "material/uniaxial/concrete/CreepShrinkageParameters.h"
#include "material/uniaxial/concrete/ACICreepSteps.h"

namespace XC {

class RawConcrete;

//! @ingroup MatUnx
//
//! @brief Creep material.
class CreepMaterial: public EncapsulatedUniaxialMaterial
  {
  private:

    // matpar : Concrete FIXED PROPERTIES
    //	double fcT;  //Time Dependent Strength
    double age; //!< concrete age at first loading, Added by AMK
    double beta; //!< tension softening parameter
    double tcast; //!< the analysis time corresponding to concrete casting in days.
    CreepShrinkageParameters creepShrinkageParameters; //!< Creep and shrinkage parameters.

    // hstvP : Concerete HISTORY VARIABLES last committed step
    CreepConcreteHistoryVars hstvP; //!< = values at previous converged step

    // hstv : Concerete HISTORY VARIABLES  current step
    CreepConcreteHistoryVars hstv; //!< = values at current step (trial values)

    //Added by AMK:
    double epsInit;
    double sigInit;
    
    double eps_cr; //!< Creep strain.
    double eps_sh; //!< Shrinkage strain.
    double eps_m; //!< Mechanical strain.
    
    double epsP_cr; //!< Commited creep strain.
    double epsP_sh; //!< Commited shrinkage strain.
    double epsP_m; //!< Commited mechanical strain.
    
    double eps_total; //!< Total strain.
    double epsP_total; //!< Commited total strain.
    
    double t; //!< Time.
    double t_load; //!< Loaded time.
    double phi_i;
    double Et;
    
    int crack_flag;
    int crackP_flag;
    int iter; //!< Iteration number

    ACICreepSteps creepSteps;


    const RawConcrete *_get_concrete_material(void) const;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    CreepMaterial(int tag= 0);
    CreepMaterial(int tag, double _fc, double _fcu, double _epscu, double _ft, double _Ec, double _beta, double _age, double _tcast, const CreepShrinkageParameters &);
    CreepMaterial(int tag, UniaxialMaterial &matl, double _age, double _tcast, const CreepShrinkageParameters &);
    void setup_parameters(void);

    double getInitialTangent(void) const;
    UniaxialMaterial *getCopy(void) const;
    
    void setCreepShrinkageParameters(const CreepShrinkageParameters &);
    const CreepShrinkageParameters &getCreepShrinkageParameters(void) const;

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double setCreepStrain(double time, double stress); //Added by AMK
    double getCurrentTime(void) const; //Added by AMK
    double getStrain(void) const;
    double getPHI_i(void) const; //Added by AMK      
    double getStress(void) const;
    double getTangent(void) const;
    double getCreep(void) const; //Added by AMK
    double getMech(void) const; //Added by AMK
    double setPhi(double time, double tp) const; //Added by AMK
    double setShrink(double time); //Added by AMK
    double getShrink(void) const; //Added by AMK

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

    int getVariable(const std::string &variable, Information &) const;

    //Added by AMK for recording Creep and Mechanical Strain:
    Response *setResponse(const std::vector<std::string> &, Information &);
    int getResponse(int responseID, Information &matInformation);

  };
} // end of XC namespace


#endif

