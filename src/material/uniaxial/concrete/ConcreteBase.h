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
//ConcreteBase.h
                                              
#ifndef ConcreteBase_h
#define ConcreteBase_h


#include "material/uniaxial/concrete/RawConcrete.h"
#include "material/uniaxial/UniaxialStateVars.h"
#include "material/uniaxial/UniaxialHistoryVars.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Base class for concrete materials.
class ConcreteBase: public RawConcrete
  {
  protected:
    UniaxialHistoryVars convergedHistory; //!< CONVERGED history Variables
    UniaxialStateVars convergedState; //!< CONVERGED state Variables

    UniaxialHistoryVars trialHistory; //!< TRIAL history Variables
    UniaxialStateVars trialState;//!< TRIAL state Variables

    int sendData(Communicator &);
    int recvData(const Communicator &);

    void revert_to_commited_history(void);
    void revert_to_commited_state(void);
    void revert_to_commited(void);
  public:
    ConcreteBase(int tag, int classTag, double fpc, double eco, double ecu);
    ConcreteBase(int tag, int classTag);

    //! @brief Returns material strain.
    inline double getStrain(void) const
      { return trialState.getStrain(); }
    //! @brief Returns the material stress.
    double getStress(void) const
      { return trialState.getStress(); }
    //! @brief Returns the tangent to stress-strain diagram.
    double getTangent(void) const
      { return trialState.getTangent(); }
  };

//! @brief Reset trial history variables to last committed state
inline void ConcreteBase::revert_to_commited_history(void)
  { trialHistory= convergedHistory; }

//! @brief Reset trial staquete variables to last committed state
inline void ConcreteBase::revert_to_commited_state(void)
  { trialState= convergedState; }

//! @brief Reset trial state and history variables to last committed state
inline void ConcreteBase::revert_to_commited(void)
  {
    // Reset trial history variables to last committed state
    revert_to_commited_history();
    // Reset trial state variables to last committed state
    revert_to_commited_state();
  }

} // end of XC namespace


#endif


