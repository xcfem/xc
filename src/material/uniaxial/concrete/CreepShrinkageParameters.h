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

#ifndef CreepShrinkageParameters_h
#define CreepShrinkageParameters_h

#include "utility/kernel/CommandEntity.h"
#include "utility/actor/actor/MovableObject.h"
namespace XC {

//! @ingroup MatUnx
//
//! @brief Creep material.
class CreepShrinkageParameters: public CommandEntity, public MovableObject
  {
  private:
    double tcr; //!< creep relationship age.
    double epsshu; //!< ultimate shrinkage
    double epssha; //!< shrinkage parameter
    double epscra; //!< concrete creep exponent parameter
    double epscru; //!< ultimate concrete creep
    //double sigCr; // stress that creep curve is based on
    double epscrd;  //!< fitting constant within the creep time evolution function as per ACI 209R-92

  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    CreepShrinkageParameters(void);
    CreepShrinkageParameters(const double &_tcr, const double &_epsshu, const double &_epssha, const double &_epscru, const double &_epscra, const double &_epscrd);

    void setup_parameters(void);
    
    double getF2(const double &, const double &) const;
    double getShrink(const double &, const double &) const;
    
    void setUltimateShrinkage(const double &);
    double getUltimateShrinkage(void) const;
    void setShrinkageParameter(const double &);
    double getShrinkageParameter(void) const;
    void setCreepRelationshipAge(const double &);
    double getCreepRelationshipAge(void) const;
    void setUltimateConcreteCreep(const double &);
    double getUltimateConcreteCreep(void) const;
    void setCreepExponentParameter(const double &);
    double getCreepExponentParameter(void) const;
    void setCreepDParameter(const double &);
    double getCreepDParameter(void) const;

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace


#endif
