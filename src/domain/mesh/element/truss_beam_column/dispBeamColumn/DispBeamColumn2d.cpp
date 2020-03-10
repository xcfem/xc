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

// $Revision: 1.25 $
// $Date: 2004/10/30 00:05:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/dispBeamColumn/DispBeamColumn2d.cpp,v $

// Written: MHS
// Created: Feb 2001
//
// Description: This file contains the class definition for XC::DispBeamColumn2d.

#include <domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2d.h>
#include "domain/component/Parameter.h"
#include <domain/mesh/node/Node.h>
#include <material/section/PrismaticBarCrossSection.h>
#include "domain/mesh/element/utils/coordTransformation/CrdTransf2d.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>
#include <cstring>
#include <domain/mesh/element/utils/Information.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "utility/recorder/response/ElementResponse.h"
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h"

XC::DispBeamColumn2d::DispBeamColumn2d(int tag, int nd1, int nd2, int numSec, const std::vector<PrismaticBarCrossSection *> &s, const CrdTransf2d &coordTransf, const BeamIntegration &bi, double r)
  :DispBeamColumn2dBase(tag, ELE_TAG_DispBeamColumn2d, nd1, nd2, numSec, s, bi, coordTransf, r)
  {}

XC::DispBeamColumn2d::DispBeamColumn2d(int tag, int numSec, const Material *m, const CrdTransf *trf, const BeamIntegration *bi)
  :DispBeamColumn2dBase(tag, ELE_TAG_DispBeamColumn2d,numSec,m,bi,trf)
  {}

XC::DispBeamColumn2d::DispBeamColumn2d(int tag)
  :DispBeamColumn2dBase(tag, ELE_TAG_DispBeamColumn2d)
  {}

//! @brief Virtual constructor.
XC::Element* XC::DispBeamColumn2d::getCopy(void) const
  { return new DispBeamColumn2d(*this); }

int XC::DispBeamColumn2d::update(void)
  {
    // Update the transformation
    theCoordTransf->update();

    // Get basic deformations
    const Vector &v= theCoordTransf->getBasicTrialDisp();

    const double L = theCoordTransf->getInitialLength();
    const double oneOverL = 1.0/L;
    const size_t numSections= getNumSections();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);

    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++)
      {
      int order = theSections[i]->getOrder();
      const ID &code = theSections[i]->getType();

      Vector e(workArea, order);

      double xi6 = 6.0*pts(i,0);

      int j;
      for(j = 0; j < order; j++) {
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  e(j) = oneOverL*v(0); break;
	case SECTION_RESPONSE_MZ:
	  e(j) = oneOverL*((xi6-4.0)*v(1) + (xi6-2.0)*v(2)); break;
	default:
	  e(j) = 0.0; break;
	}
      }

      // Set the section deformations
      theSections[i]->setTrialSectionDeformation(e);
    }

    return 0;
  }

void XC::DispBeamColumn2d::getBasicStiff(Matrix &kb, int initial) const
  {
    // Zero for integral
    kb.Zero();

    const size_t numSections= getNumSections();
    const double L = theCoordTransf->getInitialLength();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections,L);
    const double oneOverL = 1.0/L;


    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++)
      {
        const int order = theSections[i]->getOrder();
        const ID &code = theSections[i]->getType();

      Matrix ka(workArea, order, 3);
      ka.Zero();

      double xi6 = 6.0*pts(i,0);

      // Get the section tangent stiffness and stress resultant
      const Matrix &ks = theSections[i]->getSectionTangent();

      // Perform numerical integration
      //kb.addMatrixTripleProduct(1.0, *B, ks, wts(i)/L);
      const double wti = wts(i)*oneOverL;
      double tmp;
      int j, k;
      for (j = 0; j < order; j++) {
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  for (k = 0; k < order; k++)
	    ka(k,0) += ks(k,j)*wti;
	  break;
	case SECTION_RESPONSE_MZ:
	  for (k = 0; k < order; k++) {
	    tmp = ks(k,j)*wti;
	    ka(k,1) += (xi6-4.0)*tmp;
	    ka(k,2) += (xi6-2.0)*tmp;
	  }
	  break;
	default:
	  break;
	}
      }
      for (j = 0; j < order; j++) {
	switch (code(j)) {
	case SECTION_RESPONSE_P:
	  for (k = 0; k < 3; k++)
	    kb(0,k) += ka(j,k);
	  break;
	case SECTION_RESPONSE_MZ:
	  for (k = 0; k < 3; k++) {
	    tmp = ka(j,k);
	    kb(1,k) += (xi6-4.0)*tmp;
	    kb(2,k) += (xi6-2.0)*tmp;
	  }
	  break;
	default:
	  break;
	}
      }
    }
  }

const XC::Matrix &XC::DispBeamColumn2d::getTangentStiff(void) const
  {
    static Matrix kb(3,3);

    this->getBasicStiff(kb);
    // Zero for integral
    q.Zero();

    const size_t numSections= getNumSections();

    const double L = theCoordTransf->getInitialLength();
    const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections, L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections, L);
    const double oneOverL = 1.0/L;

    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++)
      {

      int order = theSections[i]->getOrder();
      const XC::ID &code = theSections[i]->getType();

      Matrix ka(workArea, order, 3);
      ka.Zero();

      double xi6 = 6.0*pts(i,0);

      // Get the section tangent stiffness and stress resultant
      const XC::Matrix &ks = theSections[i]->getSectionTangent();
      const XC::Vector &s = theSections[i]->getStressResultant();

      // Perform numerical integration
      //kb.addMatrixTripleProduct(1.0, *B, ks, wts(i)/L);
      double wti = wts(i)*oneOverL;
      double tmp;
      int j, k;
      for(j = 0; j < order; j++) {
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  for(k = 0; k < order; k++)
	    ka(k,0) += ks(k,j)*wti;
	  break;
	case SECTION_RESPONSE_MZ:
	  for(k = 0; k < order; k++) {
	    tmp = ks(k,j)*wti;
	    ka(k,1) += (xi6-4.0)*tmp;
	    ka(k,2) += (xi6-2.0)*tmp;
	  }
	  break;
	default:
	  break;
	}
      }
      for(j = 0; j < order; j++) {
	switch (code(j)) {
	case SECTION_RESPONSE_P:
	  for(k = 0; k < 3; k++)
	    kb(0,k) += ka(j,k);
	  break;
	case SECTION_RESPONSE_MZ:
	  for(k = 0; k < 3; k++) {
	    tmp = ka(j,k);
	    kb(1,k) += (xi6-4.0)*tmp;
	    kb(2,k) += (xi6-2.0)*tmp;
	  }
	  break;
	default:
	  break;
	}
      }

      //q.addMatrixTransposeVector(1.0, *B, s, wts(i));
      double si;
      for(j = 0; j < order; j++)
	{
	  si = s(j)*wts(i);
	  switch(code(j)) {
	  case SECTION_RESPONSE_P:
	    q(0) += si; break;
	  case SECTION_RESPONSE_MZ:
	    q(1) += (xi6-4.0)*si; q(2) += (xi6-2.0)*si; break;
	  default:
	    break;
	}
      }

    }

    // Add effects of element loads, q = q(v) + q0
    q(0) += q0[0];
    q(1) += q0[1];
    q(2) += q0[2];

      // Transform to global stiffness
      K = theCoordTransf->getGlobalStiffMatrix(kb, q);
      if(isDead())
	K*=dead_srf;
      return K;
  }

const XC::Matrix &XC::DispBeamColumn2d::getInitialBasicStiff(void) const
  {
    static Matrix kb(3,3);

    // Zero for integral
    kb.Zero();

    const size_t numSections= getNumSections();

    double L = theCoordTransf->getInitialLength();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections,L);
    double oneOverL = 1.0/L;

    // Loop over the integration points
    for(size_t i= 0;i<numSections;i++) {

      int order = theSections[i]->getOrder();
      const XC::ID &code = theSections[i]->getType();

      Matrix ka(workArea, order, 3);
      ka.Zero();

      double xi6 = 6.0*pts(i,0);

      // Get the section tangent stiffness and stress resultant
      const Matrix &ks = theSections[i]->getInitialTangent();

      // Perform numerical integration
      //kb.addMatrixTripleProduct(1.0, *B, ks, wts(i)/L);
      double wti = wts(i)*oneOverL;
      double tmp;
      int j, k;
      for(j = 0; j < order; j++) {
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  for(k = 0; k < order; k++)
	    ka(k,0) += ks(k,j)*wti;
	  break;
	case SECTION_RESPONSE_MZ:
	  for(k = 0; k < order; k++) {
	    tmp = ks(k,j)*wti;
	    ka(k,1) += (xi6-4.0)*tmp;
	    ka(k,2) += (xi6-2.0)*tmp;
	  }
	  break;
	default:
	  break;
	}
      }
      for(j = 0; j < order; j++) {
	switch (code(j)) {
	case SECTION_RESPONSE_P:
	  for(k = 0; k < 3; k++)
	    kb(0,k) += ka(j,k);
	  break;
	case SECTION_RESPONSE_MZ:
	  for(k = 0; k < 3; k++) {
	    tmp = ka(j,k);
	    kb(1,k) += (xi6-4.0)*tmp;
	    kb(2,k) += (xi6-2.0)*tmp;
	  }
	  break;
	default:
	  break;
	}
      }

    }
    return kb;
  }

const XC::Matrix &XC::DispBeamColumn2d::getInitialStiff(void) const
  {
    const XC::Matrix &kb = this->getInitialBasicStiff();
    // Transform to global stiffness
    K = theCoordTransf->getInitialGlobalStiffMatrix(kb);
    if(isDead())
      K*=dead_srf;
    return K;
  }



const XC::Vector &XC::DispBeamColumn2d::getResistingForce(void) const
  {
    const double L = theCoordTransf->getInitialLength();
    const size_t numSections= getNumSections();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections,L);

    // Zero for integration
    q.Zero();

    // Loop over the integration points
    for(size_t i= 0; i < numSections; i++) {

      int order = theSections[i]->getOrder();
      const ID &code = theSections[i]->getType();

      double xi6 = 6.0*pts(i,0);

      // Get section stress resultant
      const XC::Vector &s = theSections[i]->getStressResultant();

      // Perform numerical integration on internal force
      //q.addMatrixTransposeVector(1.0, *B, s, wts(i));

      double si;
      for(int j = 0; j < order; j++) {
	si = s(j)*wts(i);
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  q(0) += si; break;
	case SECTION_RESPONSE_MZ:
	  q(1) += (xi6-4.0)*si; q(2) += (xi6-2.0)*si; break;
	default:
	  break;
	}
      }

    }

    // Add effects of element loads, q = q(v) + q0
    q(0) += q0[0];
    q(1) += q0[1];
    q(2) += q0[2];

    // Vector for reactions in basic system
    Vector p0Vec= p0.getVector();

    P = theCoordTransf->getGlobalResistingForce(q, p0Vec);

    // Subtract other external nodal loads ... P_res = P_int - P_ext
    //P.addVector(1.0, load, -1.0);
    P(0) -= load(0);
    P(1) -= load(1);
    P(2) -= load(2);
    P(3) -= load(3);
    P(4) -= load(4);
    P(5) -= load(5);

    if(isDead())
      P*=dead_srf;
    return P;
  }

XC::Response *XC::DispBeamColumn2d::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    const int argc= argv.size();
    const size_t numSections= getNumSections();
    // global force -
    if(argv[0] == "forces" || argv[0] == "force"
                || argv[0] == "globalForce" || argv[0] == "globalForces")
                return new ElementResponse(this, 1, P);

    // local force -
    else if(argv[0] == "localForce" || argv[0] == "localForces")
                return new ElementResponse(this, 2, P);
    // basic force -
    else if(argv[0] == "basicForce" || argv[0] == "basicForces")
      return new ElementResponse(this, 9, Vector(3));
    // basic stiffness -
    else if(argv[0] == "basicStiffness")
      return new ElementResponse(this, 19, Matrix(3,3));
    // chord rotation -
    else if(argv[0] == "chordRotation" || argv[0] == "chordDeformation"
             || argv[0] == "basicDeformation")
      return new ElementResponse(this, 3, Vector(3));
    // plastic rotation -
    else if(argv[0] == "plasticRotation" || argv[0] == "plasticDeformation")
      return new ElementResponse(this, 4, Vector(3));
    // rayleigh forces -
    else if(argv[0] == "RayleighForces" || argv[0] == "rayleighForces" || argv[0] == "dampingForces")
      return new ElementResponse(this, 12, P);
    // section response -
    else if(argv[0]=="sectionX")
      {
	if(argc > 2)
	  {
	    float sectionLoc= atof(argv[1]);

	    double xi[SectionMatrices::maxNumSections];
            const double L = theCoordTransf->getInitialLength();
	    beamIntegration->getSectionLocations(numSections, L, xi);

	    sectionLoc/=L;

	    float minDistance = fabs(xi[0]-sectionLoc);
	    int sectionNum = 0;
	    for(size_t i= 1; i < numSections; i++)
	      {
		if(fabs(xi[i]-sectionLoc) < minDistance)
		  {
		    minDistance = fabs(xi[i]-sectionLoc);
		    sectionNum = i;
		  }
	      }

	    // output.tag("GaussPointOutput");
	    // output.attr("number",sectionNum+1);
	    // output.attr("eta",xi[sectionNum]*L);
	    std::vector<std::string> argv2(argv);
	    argv2.erase(argv2.begin(),argv2.begin()+2);
	    return theSections[sectionNum]->setResponse(argv2, eleInfo);
	  }
	else
	  return 0;
      }
    else if(argv[0] == "section" || argv[0] == "-section")
      {
        if(argc <= 2)
          return 0;

	size_t sectionNum = atoi(argv[1]);
	if(sectionNum > 0 && sectionNum <= numSections)
	  {
	    std::vector<std::string> argv1(argv);
	    argv1.erase(argv1.begin(),argv1.begin()+1);
	    return theSections[sectionNum-1]->setResponse(argv1, eleInfo);
	  }
	else
	  return 0;
      }
    // curvature sensitivity along element length
    else if(argv[0]=="dcurvdh")
      return new ElementResponse(this, 5, Vector(numSections));
    // basic deformation sensitivity
    else if(argv[0]=="dvdh")
      return new ElementResponse(this, 6, Vector(3));

    else if(argv[0]=="integrationPoints")
      return new ElementResponse(this, 7, Vector(numSections));

    else if(argv[0]=="integrationWeights")
      return new ElementResponse(this, 8, Vector(numSections));

    else if(argv[0]== "energy") //by SAJalali
      return new ElementResponse(this, 10, 0.0);
    else
      return 0;
  }

int XC::DispBeamColumn2d::getResponse(int responseID, Information &eleInfo)
{
  double V;
  double L = theCoordTransf->getInitialLength();

  if(responseID == 1)
    return eleInfo.setVector(this->getResistingForce());

  else if(responseID == 2) {
      P(3) =  q(0);
      P(0) = -q(0)+p0[0];
      P(2) = q(1);
      P(5) = q(2);
      V = (q(1)+q(2))/L;
      P(1) =  V+p0[1];
      P(4) = -V+p0[2];
      return eleInfo.setVector(P);
  }

  // Chord rotation
  else if(responseID == 3)
    return eleInfo.setVector(theCoordTransf->getBasicTrialDisp());

  // Plastic rotation
  else if(responseID == 4) {
    static XC::Vector vp(3);
    static XC::Vector ve(3);
    const XC::Matrix &kb = this->getInitialBasicStiff();
    kb.Solve(q, ve);
    vp = theCoordTransf->getBasicTrialDisp();
    vp -= ve;
    return eleInfo.setVector(vp);
  }

  else
    return -1;
}

// AddingSensitivity:BEGIN ///////////////////////////////////

//! @ Set value for parameter.
//!
//! From the parameterID value it should be possible to extract
//! information about:
//!  1) Which parameter is in question. The parameter could
//!     be at element, section, or material level.
//!  2) Which section and material number (tag) it belongs to.
//!
//! To accomplish this the parameterID is given the following value:
//!     parameterID = type + 1000*matrTag + 100000*sectionTag
//! ...where 'type' is an integer in the range (1-99) and added 100
//! for each level (from material to section to element).
//!
//! Example:
//!    If 'E0' (case 2) is random in material #3 of section #5
//!    the value of the parameterID at this (element) level would be:
//!    parameterID = 2 + 1000*3 + 100000*5 = 503002
//!    As seen, all given information can be extracted from this number.
int XC::DispBeamColumn2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // Initial declarations
    int ok = -1;
    const size_t numSections= getNumSections();
    const size_t argc= argv.size();

        // If the parameter belongs to the element itself
        if(argv[0] == "rho")
	  {
	    param.setValue(rho);
            return param.addObject(1, this);
	  }
        // If the parameter is belonging to a section or lower
        else if(argv[0] == "section" || argv[0] == "-section")
          {
	    // For now, no parameters of the section itself:
	    if(argc<5)
	      {
		std::cerr << getClassName() << "::" << __FUNCTION__
		          << "For now cannot handle parameters "
		          << "of the section itself." << std::endl;
		return -1;
	      }
	    // Get section and material tag numbers from user input
	    int paramSectionTag = atoi(argv[1]);

	    // Find the right section and call its setParameter method
	    for(size_t i=0; i<numSections; i++)
	      {
		if(paramSectionTag == theSections[i]->getTag())
		  {
		    std::vector<std::string> argv2(argv);
		    argv2.erase(argv2.begin(),argv2.begin()+2);
		    ok = theSections[i]->setParameter(argv2, param);
		  }
	      }

	    // Check if the ok is valid
	    if(ok < 0)
	      {
	        std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; could not set parameter. "
			  << std::endl;
	        return -1;
	      }
	    else
	      {
	        // Return the ok value (according to the above comments)
	        return ok;
	      }
	  }
        else if(argv[0] == "integration")
	  {
	    if(argc<2)
	      return -1;
	    else
	      {
		std::vector<std::string> argv2(argv);
		argv2.erase(argv2.begin(),argv2.begin()+1);
	        return beamIntegration->setParameter(argv2, param);
	      }
	  }
        else // Otherwise parameter is unknown for this class
	  { return -1; }
  }

int XC::DispBeamColumn2d::updateParameter(int parameterID, Information &info)
  {
        // If the parameterID value is not equal to 1 it belongs
        // to section or material further down in the hierarchy.

  const size_t numSections= getNumSections();
        if(parameterID == 1) {

                this->rho = info.theDouble;
                return 0;

        }
        else if(parameterID > 0 ) {

                // Extract the section number
                int sectionNumber = (int)( floor((double)parameterID) / (100000) );

                int ok = -1;
                for(size_t i=0; i<numSections; i++) {
                        if(sectionNumber == theSections[i]->getTag()) {
                                ok = theSections[i]->updateParameter(parameterID, info);
                        }
                }

                if(ok < 0) {
                        std::cerr << "XC::DispBeamColumn2d::updateParameter() - could not update parameter. " << std::endl;
                        return ok;
                }
                else {
                        return ok;
                }
        }
        else {
                std::cerr << "XC::DispBeamColumn2d::updateParameter() - could not update parameter. " << std::endl;
                return -1;
        }
}




int
XC::DispBeamColumn2d::activateParameter(int passedParameterID)
{
        // Note that the parameteID that is stored here at the
        // element level contains all information about section
        // and material tag number:
        parameterID = passedParameterID;

  const size_t numSections= getNumSections();
        if(passedParameterID == 0 ) {

          // "Zero out" all flags downwards through sections/materials
          for(size_t i=0; i<numSections; i++) {
            theSections[i]->activateParameter(passedParameterID);
          }
        }

        else if(passedParameterID == 1) {
          // Don't treat the 'rho' for now
        }

        else {

          // Extract section and material tags from the passedParameterID
          int activeSectionTag = (int)( floor((double)passedParameterID) / (100000) );

          // Go down to the sections and set appropriate flags
          for(size_t i=0; i<numSections; i++) {
            if(activeSectionTag == theSections[i]->getTag()) {
              theSections[i]->activateParameter(passedParameterID);
            }
          }
        }

        return 0;
}



const XC::Matrix &
XC::DispBeamColumn2d::getKiSensitivity(int gradNumber)
{
        K.Zero();
        return K;
}

const XC::Matrix &
XC::DispBeamColumn2d::getMassSensitivity(int gradNumber)
  {
        K.Zero();
        return K;
  }



const XC::Vector &XC::DispBeamColumn2d::getResistingForceSensitivity(int gradNumber)
  {
    const size_t numSections= getNumSections();
    const double L = theCoordTransf->getInitialLength();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections,L);
    const double oneOverL = 1.0/L;

    // Zero for integration
    q.Zero();
    static XC::Vector qsens(3);
    qsens.Zero();

    // Some extra declarations
    static XC::Matrix kbmine(3,3);
    kbmine.Zero();

    int j, k;
    double d1oLdh = 0.0;

    // Check if a nodal coordinate is random
    bool randomNodeCoordinate = false;
    static XC::ID nodeParameterID(2);
    nodeParameterID(0) = theNodes[0]->getCrdsSensitivity();
    nodeParameterID(1) = theNodes[1]->getCrdsSensitivity();
    if(nodeParameterID(0) != 0 || nodeParameterID(1) != 0) {

	    randomNodeCoordinate = true;

	    const XC::Vector &ndICoords = theNodes[0]->getCrds();
	    const XC::Vector &ndJCoords = theNodes[1]->getCrds();

	    double dx = ndJCoords(0) - ndICoords(0);
	    double dy = ndJCoords(1) - ndICoords(1);

	    if(nodeParameterID(0) == 1) // here x1 is random
	      d1oLdh = dx/(L*L*L);
	    if(nodeParameterID(0) == 2) // here y1 is random
	      d1oLdh = dy/(L*L*L);

	    if(nodeParameterID(1) == 1) // here x2 is random
	      d1oLdh = -dx/(L*L*L);
	    if(nodeParameterID(1) == 2) // here y2 is random
	      d1oLdh = -dy/(L*L*L);
    }

    // Loop over the integration points
    for(size_t i= 0; i < numSections; i++) {

      int order = theSections[i]->getOrder();
      const XC::ID &code = theSections[i]->getType();

	    double xi6 = 6.0*pts(i,0);
	    double wti = wts(i);

	    // Get section stress resultant gradient
	    const XC::Vector &s = theSections[i]->getStressResultant();
	    const XC::Vector &sens = theSections[i]->getStressResultantSensitivity(gradNumber,true);

	    // Perform numerical integration on internal force gradient
	    //q.addMatrixTransposeVector(1.0, *B, s, wts(i));

	    double si;
	    double sensi;
	    for(j = 0; j < order; j++) {
		    si = s(j)*wti;
		    sensi = sens(j)*wti;
		    switch(code(j)) {
		    case SECTION_RESPONSE_P:
			    q(0) += si;
			    qsens(0) += sensi;
			    break;
		    case SECTION_RESPONSE_MZ:
			    q(1) += (xi6-4.0)*si;
			    q(2) += (xi6-2.0)*si;
			    qsens(1) += (xi6-4.0)*sensi;
			    qsens(2) += (xi6-2.0)*sensi;
			    break;
		    default:
			    break;
		    }
	    }

	    if(randomNodeCoordinate) {


		    // Perform numerical integration to obtain basic stiffness matrix
		    //kb.addMatrixTripleProduct(1.0, *B, ks, wts(i)/L);
		    double tmp;

		    const XC::Matrix &ks = theSections[i]->getSectionTangent();
		    Matrix ka(workArea, order, 3);
		    ka.Zero();

		    for(j = 0; j < order; j++) {
			    switch(code(j)) {
			    case SECTION_RESPONSE_P:
				    for(k = 0; k < order; k++) {
					    ka(k,0) += ks(k,j)*wti;
				    }
				    break;
			    case SECTION_RESPONSE_MZ:
				    for(k = 0; k < order; k++) {
					    tmp = ks(k,j)*wti;
					    ka(k,1) += (xi6-4.0)*tmp;
					    ka(k,2) += (xi6-2.0)*tmp;
				    }
				    break;
			    default:
				    break;
			    }
		    }
		    for(j = 0; j < order; j++) {
			    switch (code(j)) {
			    case SECTION_RESPONSE_P:
				    for(k = 0; k < 3; k++) {
					    kbmine(0,k) += ka(j,k);
				    }
				    break;
			    case SECTION_RESPONSE_MZ:
				    for(k = 0; k < 3; k++) {
					    tmp = ka(j,k);
					    kbmine(1,k) += (xi6-4.0)*tmp;
					    kbmine(2,k) += (xi6-2.0)*tmp;
				    }
				    break;
			    default:
				    break;
			    }
		    }
	    }

    }

    static XC::Vector dqdh(3);
    const XC::Vector &dAdh_u = theCoordTransf->getBasicTrialDispShapeSensitivity();
    //dqdh = (1.0/L) * (kbmine * dAdh_u);
    dqdh.addMatrixVector(0.0, kbmine, dAdh_u, oneOverL);

    static XC::Vector dkbdh_v(3);
    const XC::Vector &A_u = theCoordTransf->getBasicTrialDisp();
    //dkbdh_v = (d1oLdh) * (kbmine * A_u);
    dkbdh_v.addMatrixVector(0.0, kbmine, A_u, d1oLdh);

    // Transform forces
    static XC::Vector dummy(3);                // No distributed loads

    // Term 5
    P = theCoordTransf->getGlobalResistingForce(qsens,dummy);

    if(randomNodeCoordinate) {
	    // Term 1
	    P += theCoordTransf->getGlobalResistingForceShapeSensitivity(q,dummy);

	    // Term 2
	    P += theCoordTransf->getGlobalResistingForce(dqdh,dummy);

	    // Term 4
	    P += theCoordTransf->getGlobalResistingForce(dkbdh_v,dummy);
    }

    return P;
  }



// NEW METHOD
int XC::DispBeamColumn2d::commitSensitivity(int gradNumber, int numGrads)
{
    // Get basic deformation and sensitivities
        const Vector &v = theCoordTransf->getBasicTrialDisp();

        static Vector vsens(3);
        vsens = theCoordTransf->getBasicDisplSensitivity(gradNumber);

        double L = theCoordTransf->getInitialLength();
        double oneOverL = 1.0/L;
  const size_t numSections= getNumSections();
        const Matrix &pts =  beamIntegration->getIntegrPointCoords(numSections,L);

        // Some extra declarations
        double d1oLdh = 0.0;

        // Check if a nodal coordinate is random
        bool randomNodeCoordinate = false;
        static XC::ID nodeParameterID(2);
        nodeParameterID(0) = theNodes[0]->getCrdsSensitivity();
        nodeParameterID(1) = theNodes[1]->getCrdsSensitivity();
        if(nodeParameterID(0) != 0 || nodeParameterID(1) != 0) {

                vsens += theCoordTransf->getBasicTrialDispShapeSensitivity();

                randomNodeCoordinate = true;

                const XC::Vector &ndICoords = theNodes[0]->getCrds();
                const XC::Vector &ndJCoords = theNodes[1]->getCrds();

                double dx = ndJCoords(0) - ndICoords(0);
                double dy = ndJCoords(1) - ndICoords(1);

                if(nodeParameterID(0) == 1) // here x1 is random
                  d1oLdh = dx/(L*L*L);
                if(nodeParameterID(0) == 2) // here y1 is random
                  d1oLdh = dy/(L*L*L);

                if(nodeParameterID(1) == 1) // here x2 is random
                  d1oLdh = -dx/(L*L*L);
                if(nodeParameterID(1) == 2) // here y2 is random
                  d1oLdh = -dy/(L*L*L);
        }

        // Loop over the integration points
        for(size_t i = 0; i < numSections; i++) {

                int order = theSections[i]->getOrder();
                const XC::ID &code = theSections[i]->getType();

                Vector e(workArea, order);

                double xi6 = 6.0*pts(i,0);

                for(int j = 0; j < order; j++) {
                        switch(code(j)) {
                        case SECTION_RESPONSE_P:
                                e(j) = oneOverL*vsens(0)
                                + d1oLdh*v(0);
                                break;
                        case SECTION_RESPONSE_MZ:
                                e(j) = oneOverL*((xi6-4.0)*vsens(1) + (xi6-2.0)*vsens(2))
                                + d1oLdh*((xi6-4.0)*v(1) + (xi6-2.0)*v(2));
                                break;
                        default:
                                e(j) = 0.0;
                                break;
                        }
                }

                // Set the section deformations
                theSections[i]->commitSensitivity(e,gradNumber,numGrads);
        }

        return 0;
}


// AddingSensitivity:END /////////////////////////////////////////////

