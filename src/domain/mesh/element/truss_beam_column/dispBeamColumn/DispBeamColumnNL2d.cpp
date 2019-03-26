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
                                                                        
// $Revision$
// $Date$
// $Source$

// Written: MHS
// Created: Feb 2001
//
// Description: This file contains the class definition for DispBeamColumnNL2d.

#include "DispBeamColumnNL2d.h"
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/utils/Information.h>
#include "material/section/ResponseId.h"
#include <material/section/PrismaticBarCrossSection.h>
#include "domain/mesh/element/utils/coordTransformation/CrdTransf2d.h"
#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h>
#include "domain/component/Parameter.h"
#include "utility/recorder/response/ElementResponse.h"
#include "utility/recorder/response/CompositeResponse.h"

XC::DispBeamColumnNL2d::DispBeamColumnNL2d(int tag, int nd1, int nd2,
				   int numSec, const std::vector<PrismaticBarCrossSection *> &s,
				   BeamIntegration &bi,
				   CrdTransf2d &coordTransf, double r)
  :DispBeamColumn2dBase(tag, ELE_TAG_DispBeamColumnNL2d, nd1, nd2, numSec, s, coordTransf, r), beamIntegration(nullptr)
  {
    beamIntegration = bi.getCopy();
  
    if(!beamIntegration)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed to copy beam integration\n";
        exit(-1);
      }
  }

XC::DispBeamColumnNL2d::DispBeamColumnNL2d(void)
  : DispBeamColumn2dBase(0, ELE_TAG_DispBeamColumnNL2d), beamIntegration(nullptr)
  {}

XC::DispBeamColumnNL2d::~DispBeamColumnNL2d()
  {    
    if(beamIntegration)
      {
	delete beamIntegration;
	beamIntegration= nullptr;
      }
  }

int XC::DispBeamColumnNL2d::update(void)
  {
    int err = 0;

    // Update the transformation
    theCoordTransf->update();

    // Get basic deformations
    const Vector &v= theCoordTransf->getBasicTrialDisp();

    const double L= theCoordTransf->getInitialLength();
    const double oneOverL = 1.0/L;

    const size_t numSections= getNumSections();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    // beamIntegration->getSectionLocations(numSections, L, xi);

    // Loop over the integration points
    for(size_t i= 0; i<numSections; i++)
      {

      int order = theSections[i]->getOrder();
      const ID &code = theSections[i]->getType();

      Vector e(workArea, order);

      double xi6 = 6.0*pts(i,0);
      //double xi6 = 6.0*xi[i];
      double zeta = pts(i,0);

      double theta = (3*zeta*zeta-4*zeta+1)*v(1) + (3*zeta*zeta-2*zeta)*v(2);

      int j;
      for(j = 0; j < order; j++) {
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  e(j) = oneOverL*v(0) + 0.5*theta*theta; break;
	case SECTION_RESPONSE_MZ:
	  e(j) = oneOverL*((xi6-4.0)*v(1) + (xi6-2.0)*v(2)); break;
	default:
	  e(j) = 0.0; break;
	}
      }

      // Set the section deformations
      err += theSections[i]->setTrialSectionDeformation(e);
    }

    if (err != 0) {
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed setTrialSectionDeformations()\n";
      return err;
    }

    return 0;
  }

void XC::DispBeamColumnNL2d::getBasicStiff(Matrix &kb, int initial) const
  {
    // Zero for integral
    kb.Zero();

    // Update the transformation
    theCoordTransf->update();

    // Get basic deformations
    const Vector &v = theCoordTransf->getBasicTrialDisp();

    const double L = theCoordTransf->getInitialLength();
    const double oneOverL = 1.0/L;

    const size_t numSections= getNumSections();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections,L);

 
    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++) {

      int order = theSections[i]->getOrder();
      const ID &code = theSections[i]->getType();

      Matrix ka(workArea, order, 3);
      ka.Zero();

      double xi6 = 6.0*pts(i,0);
      double zeta = pts(i,0);

      double c1 = 3*zeta*zeta-4*zeta+1;
      double c2 = 3*zeta*zeta-2*zeta;
      double theta = c1*v(1) + c2*v(2);

      // Get the section tangent stiffness and stress resultant
      const Matrix &ks = theSections[i]->getSectionTangent();
      const Vector &s = theSections[i]->getStressResultant();

      // Perform numerical integration
      //kb.addMatrixTripleProduct(1.0, *B, ks, wts(i)/L);
      //double wti = wts(i)*oneOverL;
      double wti = wts(i)*oneOverL;
      double tmp;
      int j, k;

      // Axial force term: int_0^L N*C'*C dx
      for(j = 0; j < order; j++) {
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  tmp = s(j)*wts(i)*L;
	  kb(1,1) += tmp*c1*c1;
	  kb(2,1) += tmp*c2*c1;
	  kb(1,2) += tmp*c1*c2;
	  kb(2,2) += tmp*c2*c2;
	  break;
	}
      }

      Matrix B(order,3);
      Matrix C(order,3);
      static Matrix C1(1,3);
      for(j = 0; j < order; j++) {
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  B(j,0) = 1.0;
	  C(j,1) = c1;
	  C(j,2) = c2;
	  C1(0,1) = c1;
	  C1(0,2) = c2;
	  break;
	case SECTION_RESPONSE_MZ:
	  B(j,1) = xi6-4.0;
	  B(j,2) = xi6-2.0;
	  break;
	default:
	  break;
	}
      }

      // B'*ks*B
      kb.addMatrixTripleProduct(1.0, B, ks, wti);

      Matrix kC(order,3);

      // B'*ks*C*theta
      kC.addMatrixProduct(0.0, ks, C, 1.0);
      kb.addMatrixTransposeProduct(1.0, B, kC, theta*wts(i));

      Matrix ks1(1,order);
      static Matrix ksB(1,3);

      for(j = 0; j < order; j++) {
	if (code(j) == SECTION_RESPONSE_P) {
	  for(int jj = 0; jj < order; jj++)
	    ks1(0,jj) = ks(j,jj);

	  ksB.addMatrixProduct(0.0, ks1, B, 1.0);

	  kb.addMatrixTransposeProduct(1.0, C1, ksB, theta*wts(i));

	  ksB.addMatrixProduct(0.0, ks1, C, 1.0);

	  kb.addMatrixTransposeProduct(1.0, C1, ksB, theta*theta*wts(i)*L);
	}
      }

      continue;

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
	  for(int jj = 0; jj < order; jj++) {
	    switch(code(jj)) {
	    case SECTION_RESPONSE_P:
	      tmp = ks(k,jj)*wts(i)*theta;
	      ka(k,1) += tmp*c1;
	      ka(k,2) += tmp*c2;
	      break;
	    }
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
  }

const XC::Matrix &XC::DispBeamColumnNL2d::getTangentStiff(void)
  {
    static Matrix kb(3,3);

    this->getBasicStiff(kb);

    // Zero for integral
    q.Zero();

    // Update the transformation
    theCoordTransf->update();

    // Get basic deformations
    const Vector &v = theCoordTransf->getBasicTrialDisp();

    const double L = theCoordTransf->getInitialLength();
    const size_t numSections= getNumSections();
    const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections, L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections, L);

    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++) {

      int order = theSections[i]->getOrder();
      const ID &code = theSections[i]->getType();

      double xi6 = 6.0*pts(i,0);
      //double xi6 = 6.0*pts(i,0);
      double zeta = pts(i,0);

      double theta = (3*zeta*zeta-4*zeta+1)*v(1) + (3*zeta*zeta-2*zeta)*v(2);

      // Get the section tangent stiffness and stress resultant
      const Vector &s = theSections[i]->getStressResultant();

      // Perform numerical integration
      //q.addMatrixTransposeVector(1.0, *B, s, wts(i));
      double si;
      for(int j = 0; j < order; j++) {
	//si = s(j)*wts(i);
	si = s(j)*wts(i);
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  q(0) += si; break;
	case SECTION_RESPONSE_MZ:
	  q(1) += (xi6-4.0)*si;
	  q(2) += (xi6-2.0)*si; 
	  for(int jj = 0; jj < order; jj++) {
	    switch(code(jj)) {
	    case SECTION_RESPONSE_P:
	      q(1) += (3*zeta*zeta-4*zeta+1)*theta*s(jj)*wts(i)*L;
	      q(2) += (3*zeta*zeta-2*zeta)*theta*s(jj)*wts(i)*L;
	      break;
	    }
	  }
	  break;
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


const XC::Vector &XC::DispBeamColumnNL2d::getResistingForce(void)
  {
    // Update the transformation
    theCoordTransf->update();

    // Get basic deformations
    const Vector &v = theCoordTransf->getBasicTrialDisp();

    const double L = theCoordTransf->getInitialLength();
    const size_t numSections= getNumSections();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections, L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections, L);  

    // Zero for integration
    q.Zero();

    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++) {

      size_t order = theSections[i]->getOrder();
      const ID &code = theSections[i]->getType();

      double xi6 = 6.0*pts(i,0);
      double zeta = pts(i,0);

      double c1 = 3*zeta*zeta-4*zeta+1;
      double c2 = 3*zeta*zeta-2*zeta;
      double theta = c1*v(1) + c2*v(2);

      // Get section stress resultant
      const Vector &s = theSections[i]->getStressResultant();

      // Perform numerical integration on internal force
      //q.addMatrixTransposeVector(1.0, *B, s, wts(i));

      double si;
      for(size_t j = 0; j < order; j++) {
	//si = s(j)*wts(i);
	si = s(j)*wts(i);
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  q(0) += si; break;
	case SECTION_RESPONSE_MZ:
	  q(1) += (xi6-4.0)*si; 
	  q(2) += (xi6-2.0)*si; 
	  for(size_t jj = 0; jj < order; jj++) {
	    switch(code(jj)) {
	    case SECTION_RESPONSE_P:
	      q(1) += c1*theta*s(jj)*wts(i)*L;
	      q(2) += c2*theta*s(jj)*wts(i)*L;
	      break;
	    }
	  }
	  break;
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
    //P.addVector(1.0, Q, -1.0);
    P(0) -= load(0);
    P(1) -= load(1);
    P(2) -= load(2);
    P(3) -= load(3);
    P(4) -= load(4);
    P(5) -= load(5);

    return P;
  }

XC::Response *XC::DispBeamColumnNL2d::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    const int argc= argv.size();
    Response *theResponse= nullptr;

    // eleInfo.tag("ElementOutput");
    // eleInfo.attr("eleType","DispBeamColumnNL2d");
    // eleInfo.attr("eleTag",this->getTag());
    // eleInfo.attr("node1",connectedExternalNodes[0]);
    // eleInfo.attr("node2",connectedExternalNodes[1]);

    const double L = theCoordTransf->getInitialLength();
    const size_t numSections= getNumSections();
    const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections,L);
  // global force - 
    if ((argv[0]=="forces") || (argv[0]=="force") || (argv[0]=="globalForce") || (argv[0]=="globalForces"))
      {

    //eleInfo.tag("ResponseType","Px_1");
    //eleInfo.tag("ResponseType","Py_1");
    //eleInfo.tag("ResponseType","Mz_1");
    //eleInfo.tag("ResponseType","Px_2");
    //eleInfo.tag("ResponseType","Py_2");
    //eleInfo.tag("ResponseType","Mz_2");

    theResponse =  new ElementResponse(this, 1, P);
  
  
  // local force -
  } else if ((argv[0]=="localForce") || (argv[0]=="localForces"))
      {

    //eleInfo.tag("ResponseType","N1");
    //eleInfo.tag("ResponseType","V1");
    //eleInfo.tag("ResponseType","M1");
    //eleInfo.tag("ResponseType","N2");
    //eleInfo.tag("ResponseType","V2");
    //eleInfo.tag("ResponseType","M2");

    theResponse =  new ElementResponse(this, 2, P);
  

  // basic force -
  } else if ((argv[0]=="basicForce")  || (argv[0]=="basicForces") ) {

    //eleInfo.tag("ResponseType","N");
    //eleInfo.tag("ResponseType","M1");
    //eleInfo.tag("ResponseType","M2");

    theResponse =  new ElementResponse(this, 9, Vector(3));

  // basic stiffness -
  } else if ((argv[0]=="basicStiffness") ) {

    //eleInfo.tag("ResponseType","N");
    //eleInfo.tag("ResponseType","M1");
    //eleInfo.tag("ResponseType","M2");

    theResponse =  new ElementResponse(this, 19, Matrix(3,3));

  // chord rotation -
  } else if ((argv[0]=="chordRotation")  || (argv[0]=="chordDeformation")  
	     || (argv[0]=="basicDeformation") ) {

    //eleInfo.tag("ResponseType","eps");
    //eleInfo.tag("ResponseType","theta1");
    //eleInfo.tag("ResponseType","theta2");

    theResponse =  new ElementResponse(this, 3, Vector(3));
  
  // plastic rotation -
  } else if ((argv[0]=="plasticRotation")  || (argv[0]=="plasticDeformation") ) {

    //eleInfo.tag("ResponseType","epsP");
    //eleInfo.tag("ResponseType","theta1P");
    //eleInfo.tag("ResponseType","theta2P");

    theResponse =  new ElementResponse(this, 4, Vector(3));

  } else if ((argv[0]=="RayleighForces")  || (argv[0]=="rayleighForces") )
      { theResponse =  new ElementResponse(this, 12, P); }
  // section response -
  else if (argv[0]=="sectionX")
    {
    if (argc > 2) {
      float sectionLoc = atof(argv[1]);

      
      sectionLoc /= L;

      float minDistance = fabs(pts(0,0)-sectionLoc);
      int sectionNum = 0;
      for(size_t i = 1; i < numSections; i++)
	{
	if (fabs(pts(i,0)-sectionLoc) < minDistance) {
	  minDistance = fabs(pts(i,0)-sectionLoc);
	  sectionNum = i;
	}
	  }

      //eleInfo.tag("GaussPointOutput");
      //eleInfo.attr("number",sectionNum+1);
      //eleInfo.attr("eta",xi[sectionNum]*L);
      std::vector<std::string> argv2(argv.begin()+2,argv.end());
      theResponse = theSections[sectionNum]->setResponse(argv2, eleInfo);
    }
  }
  else if (argv[0]=="section")
    {

    if (argc > 1) {
      
      size_t sectionNum = atoi(argv[1]);
      
      if (sectionNum > 0 && sectionNum <= numSections && argc > 2) {

	//eleInfo.tag("GaussPointOutput");
	//eleInfo.attr("number",sectionNum);
        //const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections,L);
	//eleInfo.attr("eta",pts(sectionNum-1,0)*L);

	if(argv[2]=="dsdh")
	  {
            std::vector<std::string> argv2(argv.begin()+2,argv.end());
  	    theResponse = theSections[sectionNum-1]->setResponse(argv2, eleInfo);
	} else {
	  int order = theSections[sectionNum-1]->getOrder();
	  theResponse = new ElementResponse(this, 76, Vector(order));
	  Information &info = theResponse->getInformation();
	  info.theInt = sectionNum;
	}
	
	//eleInfo.endTag();
      
      } else if (sectionNum == 0) { // argv[1] was not an int, we want all sections, 
	
	CompositeResponse *theCResponse= new CompositeResponse();
	int numResponse = 0;
	//const double L = theCoordTransf->getInitialLength();
        //const size_t numSections= getNumSections();
        //const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections,L);
	
	for(size_t i=0; i<numSections; i++) {
	  
	  //eleInfo.tag("GaussPointOutput");
	  //eleInfo.attr("number",i+1);
	  //eleInfo.attr("eta",pts(i,0)*L);
	  
          std::vector<std::string> argv1(argv.begin()+1,argv.end());
	  Response *theSectionResponse = theSections[i]->setResponse(argv1, eleInfo);

	  //eleInfo.endTag();	  

	  if (theSectionResponse != 0) {
	    numResponse = theCResponse->addResponse(theSectionResponse);
	  }
	}
	
	if (numResponse == 0) // no valid responses found
	  delete theCResponse;
	else
	  theResponse = theCResponse;
      }
    }
  }
  
  // curvature sensitivity along element length
  else if (argv[0]=="dcurvdh")
    return new ElementResponse(this, 5, Vector(numSections));
  
  // basic deformation sensitivity
  else if (argv[0]=="dvdh")
    return new ElementResponse(this, 6, Vector(3));
  
  else if (argv[0]=="integrationPoints")
    return new ElementResponse(this, 7, Vector(numSections));
  
  else if (argv[0]=="integrationWeights")
    return new ElementResponse(this, 8, Vector(numSections));
  
  //eleInfo.endTag();
  return theResponse;
}

int XC::DispBeamColumnNL2d::getResponse(int responseID, Information &eleInfo)
  {
    const double L = theCoordTransf->getInitialLength();
    const size_t numSections= getNumSections();
    //const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections,L);
    if (responseID == 12)
      return eleInfo.setVector(this->getRayleighDampingForces());

    else if (responseID == 19)
      {
        static Matrix kb(3,3);
        this->getBasicStiff(kb);
        return eleInfo.setMatrix(kb);
      }
    else if (responseID == 5) // Curvature sensitivity
      {
	/*
	  Vector curv(numSections);
	  const Vector &v = theCoordTransf->getBasicDispGradient(1);

	  double L = theCoordTransf->getInitialLength();
	  double oneOverL = 1.0/L;
	  //const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections,L);
	  double pts[2];
	  pts[0] = 0.0;
	  pts[1] = 1.0;

	  // Loop over the integration points
	  for(int i = 0; i < numSections; i++) {
	    int order = theSections[i]->getOrder();
	    const ID &code = theSections[i]->getType();
	    //double xi6 = 6.0*pts(i,0);
	    double xi6 = 6.0*pts[i];
	    curv(i) = oneOverL*((xi6-4.0)*v(1) + (xi6-2.0)*v(2));
	  }

	  return eleInfo.setVector(curv);
	*/

        Vector curv(numSections);

	/*
	// Loop over the integration points
	for(int i = 0; i < numSections; i++) {
	  int order = theSections[i]->getOrder();
	  const ID &code = theSections[i]->getType();
	  const Vector &dedh = theSections[i]->getdedh();
	  for(int j = 0; j < order; j++) {
	    if (code(j) == SECTION_RESPONSE_MZ)
	      curv(i) = dedh(j);
	  }
	}
	*/

        return eleInfo.setVector(curv);
      }

    // Basic deformation sensitivity
    else if (responseID == 6)
      {  
        const Vector &dvdh = theCoordTransf->getBasicDisplSensitivity(1);
        return eleInfo.setVector(dvdh);
      }
    else if (responseID == 7)
      {
	const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections,L);
	Vector locs(numSections);
	for(size_t i = 0; i < numSections; i++)
	  locs(i) = pts(i,0)*L;
	return eleInfo.setVector(locs);
      }
    else if (responseID == 8)
      {
        const double L = theCoordTransf->getInitialLength();
	const Vector &wts = beamIntegration->getIntegrPointWeights(numSections,L);
	Vector weights(numSections);
	for(size_t i = 0; i < numSections; i++)
	  weights(i) = wts(i)*L;
	return eleInfo.setVector(weights);
      }
    else
      return DispBeamColumn2dBase::getResponse(responseID, eleInfo);
  }

int XC::DispBeamColumnNL2d::getResponseSensitivity(int responseID, int gradNumber, Information &eleInfo)
  {
     const size_t numSections= getNumSections();
     // Basic deformation sensitivity
     if (responseID == 3)
       {  
         const Vector &dvdh = theCoordTransf->getBasicDisplSensitivity(gradNumber);
         return eleInfo.setVector(dvdh);
       }

  // Basic force sensitivity
  else if (responseID == 9) {
    static Vector dqdh(3);

    dqdh.Zero();

    return eleInfo.setVector(dqdh);
  }

  // dsdh
  else if (responseID == 76) {

    int sectionNum = eleInfo.theInt;
    int order = theSections[sectionNum-1]->getOrder();
    const ID &code = theSections[sectionNum-1]->getType();

    Vector dsdh(order);
    dsdh = theSections[sectionNum-1]->getStressResultantSensitivity(gradNumber, true);

    const Vector &v = theCoordTransf->getBasicTrialDisp();
    const Vector &dvdh = theCoordTransf->getBasicDisplSensitivity(gradNumber);

    double L = theCoordTransf->getInitialLength();
    double oneOverL = 1.0/L;

    const Matrix &ks = theSections[sectionNum-1]->getSectionTangent();

    Vector dedh(order);

    const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections,L);

    const double x = pts(sectionNum-1,0);

    //double xi6 = 6.0*pts(i,0);
    double xi6 = 6.0*x;
    double zeta = x;

    double theta = (3*zeta*zeta-4*zeta+1)*v(1) + (3*zeta*zeta-2*zeta)*v(2);
    double dthetadh = (3*zeta*zeta-4*zeta+1)*dvdh(1) + (3*zeta*zeta-2*zeta)*dvdh(2);

    int j;
    for(j = 0; j < order; j++) {
      switch(code(j)) {
      case SECTION_RESPONSE_P:
	dedh(j) = oneOverL*dvdh(0) + theta*dthetadh; break;
      case SECTION_RESPONSE_MZ:
	dedh(j) = oneOverL*((xi6-4.0)*dvdh(1) + (xi6-2.0)*dvdh(2)); break;
      default:
	dedh(j) = 0.0; break;
      }
    }

    dsdh.addMatrixVector(1.0, ks, dedh, 1.0);

    return eleInfo.setVector(dsdh);
  }

  else
    return -1;
}

// AddingSensitivity:BEGIN ///////////////////////////////////
int
XC::DispBeamColumnNL2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const size_t argc= argv.size();
    if (argc < 1)
      return -1;
  
  // If the parameter belongs to the element itself
  if(argv[0]=="rho")
    return param.addObject(1, this);
  
    const double L = theCoordTransf->getInitialLength();
    const size_t numSections= getNumSections();
    const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections,L);
  if(argv[0]=="sectionX")
    {
    if (argc < 3)
		return -1;
      
      const float sectionLoc = atof(argv[1])/L;

      float minDistance = fabs(pts(0,0)-sectionLoc);
      int sectionNum = 0;
      for(size_t i = 1; i < numSections; i++) {
	if (fabs(pts(i,0)-sectionLoc) < minDistance) {
	  minDistance = fabs(pts(i,0)-sectionLoc);
	  sectionNum = i;
	}
	  }
        std::vector<std::string> argv2(argv.begin()+2,argv.end());
	return theSections[sectionNum]->setParameter(argv2, param);
  }
  // If the parameter belongs to a section or lower
  if(argv[0]=="section")
    {
    
    if (argc < 3)
      return -1;
    
    // Get section number: 1...Np
    size_t sectionNum = atoi(argv[1]);
    
    if(sectionNum > 0 && sectionNum <= numSections)
      {
	std::vector<std::string> argv2(argv.begin()+2,argv.end());
        return theSections[sectionNum-1]->setParameter(argv2, param);
      } 
    else
      return -1;
  }
  
  else if (argv[0]=="integration")
    {
      if (argc < 2)
        return -1;
      std::vector<std::string> argv1(argv.begin()+1,argv.end());
      return beamIntegration->setParameter(argv1, param);
    }
  int result =-1;
  // Default, send to every object
  int ok = 0;
  for(size_t i = 0; i < numSections; i++)
    {
      ok = theSections[i]->setParameter(argv, param);
      if (ok != -1) 
        result = ok;
    }
  ok = beamIntegration->setParameter(argv, param);
  if (ok != -1)
    result = ok;

  return result;
}

int
XC::DispBeamColumnNL2d::updateParameter (int parameterID, Information &info)
{
  if (parameterID == 1) {
    rho = info.theDouble;
    return 0;
  }
  else
    return -1;  
}




int XC::DispBeamColumnNL2d::activateParameter(int passedParameterID)
  {
    parameterID = passedParameterID;
    return 0;
  }



const XC::Matrix &XC::DispBeamColumnNL2d::getInitialStiffSensitivity(int gradNumber)
{
  static Matrix kb(3,3);

  // Zero for integral
  kb.Zero();
  
    const double L = theCoordTransf->getInitialLength();
    const double oneOverL = 1.0/L;
  
    const size_t numSections= getNumSections();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections,L);

  // Loop over the integration points
  for(size_t i = 0; i < numSections; i++) {
    
    int order = theSections[i]->getOrder();
    const ID &code = theSections[i]->getType();
  
    Matrix ka(workArea, order, 3);
    ka.Zero();

    //double xi6 = 6.0*pts(i,0);
    double xi6 = 6.0*pts(i,0);
    
    // Get the section tangent stiffness and stress resultant
    const Matrix &ks = theSections[i]->getInitialTangentSensitivity(gradNumber);
    
    // Perform numerical integration
    //kb.addMatrixTripleProduct(1.0, *B, ks, wts(i)/L);
    //double wti = wts(i)*oneOverL;
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

  // Transform to global stiffness
  K = theCoordTransf->getInitialGlobalStiffMatrix(kb);
  
  return K;
}

const XC::Matrix &XC::DispBeamColumnNL2d::getMassSensitivity(int gradNumber)
  {
	K.Zero();
	return K;
  }



const XC::Vector &XC::DispBeamColumnNL2d::getResistingForceSensitivity(int gradNumber)
{
  // Update the transformation
  theCoordTransf->update();
  
  // Get basic deformations
  const Vector &v = theCoordTransf->getBasicTrialDisp();

    const double L = theCoordTransf->getInitialLength();
    const size_t numSections= getNumSections();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections,L);


  // Zero for integration
  static Vector dqdh(3);
  dqdh.Zero();
  
  // Loop over the integration points
  for(size_t i = 0; i < numSections; i++)
    {
    
    int order = theSections[i]->getOrder();
    const ID &code = theSections[i]->getType();
    
    //double xi6 = 6.0*pts(i,0);
    double xi6 = 6.0*pts(i,0);
    double zeta = pts(i,0);

    double c1 = 3*zeta*zeta-4*zeta+1;
    double c2 = 3*zeta*zeta-2*zeta;
    double theta = c1*v(1) + c2*v(2);

    double wti = wts(i);
    
    // Get section stress resultant gradient
    const Vector &dsdh = theSections[i]->getStressResultantSensitivity(gradNumber,true);
    
    // Perform numerical integration on internal force gradient
    double sensi;
    for(int j = 0; j < order; j++) {
      sensi = dsdh(j)*wti;
      switch(code(j)) {
      case SECTION_RESPONSE_P:
	dqdh(0) += sensi; // 1
	dqdh(1) += c1*theta*sensi*L; // 2
	dqdh(2) += c2*theta*sensi*L; // 2
	break;
      case SECTION_RESPONSE_MZ:
	dqdh(1) += (xi6-4.0)*sensi; // 1
	dqdh(2) += (xi6-2.0)*sensi; // 1
	break;
      default:
	break;
      }
    }
  }
  


  if (theCoordTransf->isShapeSensitivity()) {
    double dLdh = theCoordTransf->getdLdh();

    std::vector<double> dptsdh(section_matrices.getMaxNumSections());
    beamIntegration->getLocationsDeriv(numSections, L, dLdh, &dptsdh[0]);

    std::vector<double> dwtsdh(section_matrices.getMaxNumSections());
    beamIntegration->getWeightsDeriv(numSections, L, dLdh, &dwtsdh[0]);

    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++)
      {

      double dwtLdh = wts(i)*dLdh + dwtsdh[i]*L;
      double dptLdh = pts(i,0)*dLdh + dptsdh[i]*L;

      int order = theSections[i]->getOrder();
      const ID &code = theSections[i]->getType();

      double xi6 = 6.0*pts(i,0);
      double zeta = pts(i,0);

      double c1 = 3*zeta*zeta-4*zeta+1;
      double c2 = 3*zeta*zeta-2*zeta;
      double theta = c1*v(1) + c2*v(2);

      Matrix B(order,3);
      Matrix dBdh(order,3);
      Matrix C(order,3);
      Matrix dCdh(order,3);
      Matrix C1(1,3);
      Matrix dC1dh(1,3);

      const Vector &s = theSections[i]->getStressResultant();
      const Matrix &ks = theSections[i]->getSectionTangent();

      double N = 0.0;

      for(int j = 0; j < order; j++) {
	switch(code(j)) {
	case SECTION_RESPONSE_P:
	  N += s(j);

	  B(j,0) = 1.0/L;
	  dBdh(j,0) = -dLdh/(L*L);

	  C(j,1) = c1;
	  C(j,2) = c2;
	  dCdh(j,1) = -(xi6-4.0)*pts(i,0)/L*dLdh + (xi6-4.0)/L*dptLdh;
	  dCdh(j,2) = -(xi6-2.0)*pts(i,0)/L*dLdh + (xi6-2.0)/L*dptLdh;

	  C1(0,1) = c1;
	  C1(0,2) = c2;
	  dC1dh(0,1) = -(xi6-4.0)*pts(i,0)/L*dLdh + (xi6-4.0)/L*dptLdh;
	  dC1dh(0,2) = -(xi6-2.0)*pts(i,0)/L*dLdh + (xi6-2.0)/L*dptLdh;
	  break;
	case SECTION_RESPONSE_MZ:
	  B(j,1) = (xi6-4.0)/L;
	  B(j,2) = (xi6-2.0)/L;
	  dBdh(j,1) = -(12*pts(i,0)-4.0)/(L*L)*dLdh + 6/(L*L)*dptLdh;
	  dBdh(j,2) = -(12*pts(i,0)-2.0)/(L*L)*dLdh + 6/(L*L)*dptLdh;
	  break;
	}
      }

      dqdh.addMatrixTransposeVector(1.0, dBdh, s, wts(i)*L); // 3

      dqdh(1) += dC1dh(0,1)*theta*N*wts(i)*L; // 4
      dqdh(2) += dC1dh(0,2)*theta*N*wts(i)*L; // 4

      double dCdhv = dC1dh(0,1)*v(1) + dC1dh(0,2)*v(2);
      dqdh(1) += C1(0,1)*dCdhv*N*wts(i)*L; // 5
      dqdh(2) += C1(0,2)*dCdhv*N*wts(i)*L; // 5

      Matrix tmp(order,3);
      tmp = dBdh;
      tmp.addMatrix(1.0, dCdh, theta);

      Matrix tmp2(3,3);
      tmp2.addMatrixTripleProduct(0.0, C, ks, tmp, 1.0);
      dqdh.addMatrixVector(1.0, tmp2, v, theta*wts(i)*L); // 7

      tmp2.addMatrixTripleProduct(0.0, B, ks, tmp, 1.0);
      dqdh.addMatrixVector(1.0, tmp2, v, wts(i)*L); // 6

      dqdh.addMatrixTransposeVector(1.0, B, s, dwtLdh); // 8

      dqdh(1) += C1(0,1)*theta*N*dwtLdh; // 9
      dqdh(2) += C1(0,2)*theta*N*dwtLdh; // 9

      continue;

      double si;
      for(int j = 0; j < order; j++)
	{
          si = s(j)*wts(i);
	  switch(code(j))
	    {
	    case SECTION_RESPONSE_P:
	      dqdh(0) += -dLdh/(L*L);
	      dqdh(1) += (-6*pts(i,0)+4.0)*pts(i,0)/L*theta*si*L*dLdh;
	      dqdh(2) += (-6*pts(i,0)+2.0)*pts(i,0)/L*theta*si*L*dLdh;
	      break;
	    case SECTION_RESPONSE_MZ:
	      dqdh(1) += (-12*pts(i,0)+4.0)/(L*L)*si*L*dLdh; 
	      dqdh(2) += (-12*pts(i,0)+2.0)/(L*L)*si*L*dLdh; 
	      break;
	    default:
	      break;
	    }
        }

    }

  }


  // Transform forces
  static Vector dp0dh(3);		// No distributed loads
  dp0dh.Zero();

  P.Zero();

  //////////////////////////////////////////////////////////////

  if (theCoordTransf->isShapeSensitivity()) {
    
    // Perform numerical integration to obtain basic stiffness matrix
    // Some extra declarations
    static Matrix kbmine(3,3);
    this->getBasicStiff(kbmine);
    
    // k dAdh u
    const Vector &dAdh_u = theCoordTransf->getBasicTrialDispShapeSensitivity();
    //dqdh.addMatrixVector(1.0, kbmine, dAdh_u, oneOverL);
    dqdh.addMatrixVector(1.0, kbmine, dAdh_u, 1.0);

    // dAdh^T q
    P += theCoordTransf->getGlobalResistingForceShapeSensitivity(q, dp0dh, gradNumber);
  }
  
  // A^T (dqdh + k dAdh u)
  P += theCoordTransf->getGlobalResistingForce(dqdh, dp0dh);
  
  return P;
}



// NEW METHOD
int XC::DispBeamColumnNL2d::commitSensitivity(int gradNumber, int numGrads)
{
  // Get basic deformation and sensitivities
  const Vector &v = theCoordTransf->getBasicTrialDisp();
  
  static Vector dvdh(3);
  dvdh = theCoordTransf->getBasicDisplSensitivity(gradNumber);
  
  double L = theCoordTransf->getInitialLength();
  double oneOverL = 1.0/L;
  const size_t numSections= getNumSections();
  const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);

  // Some extra declarations
  double d1oLdh = theCoordTransf->getd1overLdh();
  
  // Loop over the integration points
  for(size_t i = 0; i < numSections; i++) {
    
    int order = theSections[i]->getOrder();
    const ID &code = theSections[i]->getType();
    
    Vector e(workArea, order);
    
    double xi6 = 6.0*pts(i,0);
    
    double zeta = pts(i,0);

    double c1 = 3*zeta*zeta-4*zeta+1;
    double c2 = 3*zeta*zeta-2*zeta;
    double theta = c1*v(1) + c2*v(2);

    for(int j = 0; j < order; j++) {
      switch(code(j)) {
      case SECTION_RESPONSE_P:
	e(j) = oneOverL*dvdh(0)
	  + d1oLdh*v(0)
	  + theta*(c1*dvdh(1)+c2*dvdh(2)); 
	break;
      case SECTION_RESPONSE_MZ:
	e(j) = oneOverL*((xi6-4.0)*dvdh(1) + (xi6-2.0)*dvdh(2))
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

