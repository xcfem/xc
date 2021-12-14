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
//HingeBeamIntegration3d.h

#ifndef HingeBeamIntegration3d_h
#define HingeBeamIntegration3d_h

#include "PlasticLengthsBeamIntegration.h"
#include "material/section/repres/CrossSectionProperties3d.h"

namespace XC {
class FVector;

//! @ingroup PlasticHingeBeamInteg
//
//! @brief Base class for plastic hinge integration methods for 3D
//! beam-column elements.
class HingeBeamIntegration3d: public PlasticLengthsBeamIntegration
  {
  protected:
    CrossSectionProperties3d ctes_scc; //!< Mechanical properties of the section E,A,Iy,...

    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    HingeBeamIntegration3d(int classTag, double E, double A, double Iz,double Iy, double G, double J,double lpI, double lpJ);
    HingeBeamIntegration3d(int classTag, const CrossSectionProperties3d &cts= CrossSectionProperties3d(),const double &lpi=0.0,const double &lpj=0.0);
  
    void addElasticDeformations(ElementalLoad *,double loadFactor,double L,FVector &v0);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace

#endif
