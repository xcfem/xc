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

#ifndef UserDefinedBeamIntegrationBase_h
#define UserDefinedBeamIntegrationBase_h

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h>

#include <utility/matrix/Vector.h>

namespace XC {

//! @ingroup BeamInteg
//
//! @brief Base class for user defined integration on beam elements.
class UserDefinedBeamIntegrationBase: public BeamIntegration
  {
  protected:
    Vector pts;
    Vector wts;
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    UserDefinedBeamIntegrationBase(int , const Vector &, const Vector &);
    UserDefinedBeamIntegrationBase(int , const Vector &);
    UserDefinedBeamIntegrationBase(int);
    
    void getSectionLocations(int nIP, double L, double *xi) const;
    void getSectionWeights(int nIP, double L, double *wt) const;
    
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag = 0) const;  
 };
} // end of XC namespace

#endif
