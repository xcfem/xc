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

// $Revision: 1.1 $
// $Date: 2006/01/17 21:12:56 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/LegendreBeamIntegration.cpp,v $

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LegendreBeamIntegration.h>
#include <iostream>

XC::LegendreBeamIntegration::LegendreBeamIntegration(void)
  : BeamIntegration(BEAM_INTEGRATION_TAG_Legendre) {}

XC::BeamIntegration *XC::LegendreBeamIntegration::getCopy(void) const
  { return new LegendreBeamIntegration(*this); }

void XC::LegendreBeamIntegration::getSectionLocations(int numSections, double L, double *xi) const
  {
    switch(numSections)
      {
      case 1:
        xi[0] = 0.0;
        break;
      case 2:
        xi[0] = -0.577350269189626;
        xi[1] =  0.577350269189626;
        break;
      case 3:
        xi[0] = -0.774596669241483;
        xi[1] =  0.0;
        xi[2] =  0.774596669241483;
        break;
      case 4:
        xi[0] = -0.861136311594053;
        xi[1] = -0.339981043584856;
        xi[2] =  0.339981043584856;
        xi[3] =  0.861136311594053;
        break;
      case 5:
        xi[0] = -0.906179845938664;
        xi[1] = -0.538469310105683;
        xi[2] =  0.0;
        xi[3] =  0.538469310105683;
        xi[4] =  0.906179845938664;
        break;
      case 6:
        xi[0] = -0.932469514203152;
        xi[1] = -0.661209386466265;
        xi[2] = -0.238619186083197;
        xi[3] =  0.238619186083197;
        xi[4] =  0.661209386466265;
        xi[5] =  0.932469514203152;
        break;
      case 7:
        xi[0] = -0.949107912342759;
        xi[1] = -0.741531185599394;
        xi[2] = -0.405845151377397;
        xi[3] =  0.0;
        xi[4] =  0.405845151377397;
        xi[5] =  0.741531185599394;
        xi[6] =  0.949107912342759;
        break;
      case 8:
        xi[0] = -0.960289856497536;
        xi[1] = -0.796666477413627;
        xi[2] = -0.525532409916329;
        xi[3] = -0.183434642495650;
        xi[4] =  0.183434642495650;
        xi[5] =  0.525532409916329;
        xi[6] =  0.796666477413627;
        xi[7] =  0.960289856497536;
        break;
      case 9:
        xi[0] = -0.968160239507626;
        xi[1] = -0.836031107326636;
        xi[2] = -0.613371432700590;
        xi[3] = -0.324253423403809;
        xi[4] =  0.0;
        xi[5] =  0.324253423403809;
        xi[6] =  0.613371432700590;
        xi[7] =  0.836031107326636;
        xi[8] =  0.968160239507626;
        break;
      case 10:
        xi[0] = -0.973906528517172;
        xi[1] = -0.865063366688985;
        xi[2] = -0.679409568299024;
        xi[3] = -0.433395394129247;
        xi[4] = -0.148874338981631;
        xi[5] =  0.148874338981631;
        xi[6] =  0.433395394129247;
        xi[7] =  0.679409568299024;
        xi[8] =  0.865063366688985;
        xi[9] =  0.973906528517172;
        break;
      default:
        break;
      }
    for(int i = 0; i < numSections; i++)
      xi[i]  = 0.5*(xi[i] + 1.0);
  }

void XC::LegendreBeamIntegration::getSectionWeights(int numSections, double L, double *wt) const
  {
    switch (numSections)
      {
      case 1:
        wt[0] = 2.0;
        break;
      case 2:
        wt[0] = 1.0;
        wt[1] = 1.0;
        break;
      case 3:
        wt[0] = 0.555555555555556;
        wt[1] = 0.888888888888889;
        wt[2] = 0.555555555555556;
        break;        
      case 4:    
        wt[0] = 0.347854845137454;
        wt[1] = 0.652145154862546;
        wt[2] = 0.652145154862546;
        wt[3] = 0.347854845137454;
        break;
      case 5:
        wt[0] = 0.236926885056189;
        wt[1] = 0.478628670499366;
        wt[2] = 0.568888888888889;
        wt[3] = 0.478628670499366;
        wt[4] = 0.236926885056189;
        break;
      case 6:
        wt[0] = 0.171324492379170;
        wt[1] = 0.360761573048139;
        wt[2] = 0.467913934572691;
        wt[3] = 0.467913934572691;
        wt[4] = 0.360761573048139;
        wt[5] = 0.171324492379170;
        break;
      case 7:
        wt[0] = 0.129484966168870;
        wt[1] = 0.279705391489277;
        wt[2] = 0.381830050505119;
        wt[3] = 0.417959183673469;
        wt[4] = 0.381830050505119;
        wt[5] = 0.279705391489277;
        wt[6] = 0.129484966168870;
        break;
      case 8:    
        wt[0] = 0.101228536290376;
        wt[1] = 0.222381034453374;
        wt[2] = 0.313706645877887;
        wt[3] = 0.362683783378362;
        wt[4] = 0.362683783378362;
        wt[5] = 0.313706645877887;
        wt[6] = 0.222381034453374;
        wt[7] = 0.101228536290376;
        break;
      case 9:    
        wt[0] = 0.081274388361574;
        wt[1] = 0.180648160694857;
        wt[2] = 0.260610696402935;
        wt[3] = 0.312347077040003;
        wt[4] = 0.330239355001260;
        wt[5] = 0.312347077040003;
        wt[6] = 0.260610696402935;
        wt[7] = 0.180648160694857;
        wt[8] = 0.081274388361574;
        break;
      case 10:
        wt[0] = 0.066671344308688;
        wt[1] = 0.149451349150581;
        wt[2] = 0.219086362515982;
        wt[3] = 0.269266719309996;
        wt[4] = 0.295524224714753;
        wt[5] = 0.295524224714753;
        wt[6] = 0.269266719309996;
        wt[7] = 0.219086362515982;
        wt[8] = 0.149451349150581;
        wt[9] = 0.066671344308688;
        break;
      default:
        break;
      }    
    for(int i = 0; i < numSections; i++)
      wt[i] *= 0.5;
  }

void XC::LegendreBeamIntegration::Print(std::ostream &s, int flag) const
  {
    s << "Legendre" << std::endl;
  }
