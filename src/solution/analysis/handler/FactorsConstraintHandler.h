// -*-c++-*-
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
//FactorsConstraintHandler.h

#ifndef FactorsConstraintHandler_h
#define FactorsConstraintHandler_h

#include <solution/analysis/handler/ConstraintHandler.h>

namespace XC {
class FE_Element;
class DOF_Group;

/**
 * @ingroup AnalysisCH
 *
 * @brief Base class for penalty and Lagrange constraints handlers.
 */
class FactorsConstraintHandler: public ConstraintHandler
  {
  protected:
    double alphaSP; //!< Factor used with the single freedom constraints.
    double alphaMP; //!< Factor used with the multi-freedom constraints.

    int sendData(Communicator &);
    int recvData(const Communicator &);
    FactorsConstraintHandler(ModelWrapper *,int classTag,const double &alphaSP,const double &alphaMP);
  public:
    //! @brief Set the factor used with single freedom constraints.
    inline void setAlphaSP(const double &a)
      { alphaSP= a; }
    //! @brief Return the factor used with single freedom constraints.
    double getAlphaSP(void) const
      { return alphaSP; }
    //! @brief Set the factor used with multi-freedom constraints.
    inline void setAlphaMP(const double &a)
      { alphaMP= a; }
    //! @brief Return the factor used with multi-freedom constraints.
    double getAlphaMP(void) const
      { return alphaMP; }
    
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif




