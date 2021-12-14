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

#include "SteelBase0103.h"
#include "domain/component/Parameter.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include <domain/mesh/element/utils/Information.h>
#include <cmath>
#include <cfloat>
#include "utility/actor/actor/MovableVector.h"

//! @brief Sets all history and state variables to initial values
int XC::SteelBase0103::setup_parameters(void)
  {
    // History variables
    CminStrain= 0.0;
    CmaxStrain= 0.0;
    CshiftP= 1.0;
    CshiftN= 1.0;
    Cloading= 0;

    TminStrain= 0.0;
    TmaxStrain= 0.0;
    TshiftP= 1.0;
    TshiftN= 1.0;
    Tloading= 0;

    // State variables
    Cstrain= 0.0;
    Cstress= 0.0;
    Ctangent= E0;

    Tstrain= 0.0;
    Tstress= 0.0;
    Ttangent= E0;
    return 0;
  }

//! @brief Constructor.
XC::SteelBase0103::SteelBase0103(int tag, int classTag, double FY, double E, double B,double A1, double A2, double A3, double A4)
  : SteelBase(tag,classTag,FY,E,B,A1,A2,A3,A4) {}

//! @brief Constructor.
XC::SteelBase0103::SteelBase0103(int tag, int classTag)
  :SteelBase(tag,classTag,0.0,0.0,0.0,STEEL_0103_DEFAULT_A1,STEEL_0103_DEFAULT_A2,STEEL_0103_DEFAULT_A3,STEEL_0103_DEFAULT_A4) {}

//! @brief Constructor.
XC::SteelBase0103::SteelBase0103(int classTag)
  :SteelBase(0,classTag,0.0,0.0,0.0,STEEL_0103_DEFAULT_A1,STEEL_0103_DEFAULT_A2,STEEL_0103_DEFAULT_A3,STEEL_0103_DEFAULT_A4) {}

int XC::SteelBase0103::setTrialStrain(double strain, double strainRate)
  {
    if(fabs(strain)>fabs(10.0*getEpsy()))
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; Warning: the strain is very big: "
                << strain << std::endl;
    if(ezero!=0.0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; ERROR: initial strain not supported yet."
                << " Initial strain: " << ezero << std::endl;
    // Reset history variables to last converged state
    TminStrain= CminStrain;
    TmaxStrain= CmaxStrain;
    TshiftP= CshiftP;
    TshiftN= CshiftN;
    Tloading= Cloading;

    Tstrain= Cstrain;
    Tstress= Cstress;
    Ttangent= Ctangent;

    // Determine change in strain from last converged state
    const double dStrain= strain - Cstrain;

    if(fabs(dStrain) > DBL_EPSILON)
      {
        // Set trial strain
        Tstrain = strain;
        // Calculate the trial state given the trial strain
        determineTrialState(dStrain);
      }
    return 0;
  }

int XC::SteelBase0103::setTrial(double strain, double &stress, double &tangent, double strainRate)
  {
    setTrialStrain(strain,strainRate);
    stress= Tstress;
    tangent= Ttangent;
    return 0;
  }

double XC::SteelBase0103::getStrain(void) const
  { return Tstrain; }

double XC::SteelBase0103::getStress(void) const
  { return Tstress; }

//! @brief Return the material tangent stiffness.
double XC::SteelBase0103::getTangent(void) const
  { return Ttangent; }

//! @brief Commit the state of the material.
int XC::SteelBase0103::commitState(void)
  {
    // History variables
    CminStrain= TminStrain;
    CmaxStrain= TmaxStrain;
    CshiftP= TshiftP;
    CshiftN= TshiftN;
    Cloading= Tloading;

    // State variables
    Cstrain= Tstrain;
    Cstress= Tstress;
    Ctangent= Ttangent;
    return 0;
  }

//! @brief Reset material to last committed state
int XC::SteelBase0103::revertToLastCommit(void)
  {
    // Reset trial history variables to last committed state
    TminStrain= CminStrain;
    TmaxStrain= CmaxStrain;
    TshiftP= CshiftP;
    TshiftN= CshiftN;
    Tloading= Cloading;

    // Reset trial state variables to last committed state
    Tstrain = Cstrain;
    Tstress = Cstress;
    Ttangent = Ctangent;
    return 0;
  }

int XC::SteelBase0103::revertToStart(void)
  {
    int retval= SteelBase::revertToStart();
    // History variables
    setup_parameters();
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::SteelBase0103::sendData(Communicator &comm)
  {
    int res= SteelBase::sendData(comm);
    res+= comm.sendDoubles(Cstrain,Cstress,Ctangent,Tstrain,Tstress,Ttangent,getDbTagData(),CommMetaData(4));
    res+= comm.sendDoubles(CminStrain,CmaxStrain,CshiftP,CshiftN,getDbTagData(),CommMetaData(5));
    res+= comm.sendInts(Cloading,Tloading,getDbTagData(),CommMetaData(6));
    res+= comm.sendDoubles(TminStrain,TmaxStrain,TshiftP,TshiftN,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::SteelBase0103::recvData(const Communicator &comm)
  {
    int res= SteelBase::recvData(comm);
    res+= comm.receiveDoubles(Cstrain,Cstress,Ctangent,Tstrain,Tstress,Ttangent,getDbTagData(),CommMetaData(4));
    res+= comm.receiveDoubles(CminStrain,CmaxStrain,CshiftP,CshiftN,getDbTagData(),CommMetaData(5));
    res+= comm.receiveInts(Cloading,Tloading,getDbTagData(),CommMetaData(6));
    res+= comm.receiveDoubles(TminStrain,TmaxStrain,TshiftP,TshiftN,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Print stuff.
void XC::SteelBase0103::Print(std::ostream& s, int flag) const
  {
    s << "SteelBase0103 tag: " << this->getTag() << std::endl;
    s << "  fy: " << fy << " ";
    s << "  E0: " << E0 << " ";
    s << "  b:  " << b << " ";
    s << "  a1: " << a1 << " ";
    s << "  a2: " << a2 << " ";
    s << "  a3: " << a3 << " ";
    s << "  a4: " << a4 << " ";
  }
