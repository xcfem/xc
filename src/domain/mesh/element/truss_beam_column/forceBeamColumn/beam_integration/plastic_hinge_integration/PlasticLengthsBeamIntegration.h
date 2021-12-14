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
//PlasticLengthsBeamIntegration.h

#ifndef PlasticLengthsBeamIntegration_h
#define PlasticLengthsBeamIntegration_h

#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h"

namespace XC {

//! @ingroup BeamInteg
//!
//! @defgroup PlasticHingBeamInteg Plastic hinge integration methods on beam elements.
//
//! @ingroup PlasticHingeBeamInteg
//!
//! @brief Base class for plastic hinge integration methods on beam elements.
class PlasticLengthsBeamIntegration: public BeamIntegration
  {
  protected:
    double lpI;
    double lpJ;
    int parameterID;
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    PlasticLengthsBeamIntegration(int classTag,double lpI, double lpJ);
    PlasticLengthsBeamIntegration(int classTag);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &,int) const;
  };
} // end of XC namespace

#endif
