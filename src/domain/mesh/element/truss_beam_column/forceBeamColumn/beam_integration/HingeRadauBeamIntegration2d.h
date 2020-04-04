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

// $Revision: 1.5 $
// $Date: 2003/06/10 00:36:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/HingeRadauBeamIntegration2d.h,v $

#ifndef HingeRadauBeamIntegration2d_h
#define HingeRadauBeamIntegration2d_h

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeBeamIntegration2d.h>

namespace XC {
class Matrix;
class ElementalLoad;
class Channel;
class FEM_ObjectBroker;

//! @ingroup BeamInteg
//
//! @brief Radau integration on beam elements.
class HingeRadauBeamIntegration2d: public HingeBeamIntegration2d
  {
  public:
    HingeRadauBeamIntegration2d(double E, double A, double I,double lpI, double lpJ);
    HingeRadauBeamIntegration2d();
  
    void getSectionLocations(int numSections, double L, double *xi) const;
    void getSectionWeights(int numSections, double L, double *wt) const;
  
    int addElasticFlexibility(double L, Matrix &fe);

    double getTangentDriftI(double L, double LI, double q2, double q3);
    double getTangentDriftJ(double L, double LI, double q2, double q3);

    BeamIntegration *getCopy(void) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    void Print(std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace

#endif
