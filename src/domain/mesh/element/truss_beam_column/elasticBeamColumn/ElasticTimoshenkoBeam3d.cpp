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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 03/13
// Revision: B
//
// Purpose: This file contains the class definition for ElasticTimoshenkoBeam3d.
// ElasticTimoshenkoBeam3d is a 3d beam element. As such it can only
// connect to a node with 6-dof. 
//
// Revision Log:
//  - Revision B
//      Date:   12/24/2020
//      By:     Pearl Ranchal (ranchal@berkeley.edu)
//      Notes:  In setUp(), get element length from theCoordTransf instead of computing from nodal coordinates.
// 

#include "ElasticTimoshenkoBeam3d.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf3d.h"
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include "utility/matrix/Matrix.h"
#include "domain/mesh/node/Node.h"
#include "domain/component/Parameter.h"
#include "domain/load/ElementalLoad.h"

// initialize the class wide variables
XC::Matrix XC::ElasticTimoshenkoBeam3d::theMatrix(12,12);
XC::Vector XC::ElasticTimoshenkoBeam3d::theVector(12);

//! @brief Default constructor.
//! @param tag: element identifier.
XC::ElasticTimoshenkoBeam3d::ElasticTimoshenkoBeam3d(int tag)
    : ElasticBeam3dBase(tag, ELE_TAG_ElasticTimoshenkoBeam3d),
    cMass(0), nlGeo(0), phiY(0.0), phiZ(0.0), L(0.0), ul(12), ql(12), ql0(12),
    kl(12,12), klgeo(12,12), Tgl(12,12), Ki(12,12), M(12,12), theLoad(12)
  {
    // zero fixed end forces vector
    ql0.Zero();
  }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param m: element material.
//! @param trf: element coordinate transformation.
XC::ElasticTimoshenkoBeam3d::ElasticTimoshenkoBeam3d(int tag,const Material *m,const CrdTransf *trf)
  : ElasticBeam3dBase(tag, ELE_TAG_ElasticBeam3d,m,trf),
    cMass(0), nlGeo(0), phiY(0.0), phiZ(0.0), L(0.0), ul(12), ql(12), ql0(12),
    kl(12,12), klgeo(12,12), Tgl(12,12), Ki(12,12), M(12,12), theLoad(12)
  {
    // zero fixed end forces vector
     ql0.Zero();
  }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param Nd1: identifier of the first node.
//! @param Nd2: idenfifier of the second node.
//! @param e: elastic modulus.
//! @param g: shear modulus.
//! @param a: section area.
//! @param jx: torsional section modulus of the element section.
//! @param iy: modulus of inertia the element section around the y axis.
//! @param iz: modulus of inertia the element section around the z axis.
//! @param avy: shear area along y axis.
//! @param avz: shear area along z axis.
//! @param coordTransf: coordinate transformation for the element.
//! @param r: density of the element material.
XC::ElasticTimoshenkoBeam3d::ElasticTimoshenkoBeam3d(int tag, int Nd1, int Nd2, 
    double e, double g, double a, double jx, double iy, double iz, double avy,
    double avz, CrdTransf3d &coordTransf, double r, int cm)
  : ElasticBeam3dBase(tag, ELE_TAG_ElasticTimoshenkoBeam3d,a,avy/a,avz/a,e,g,jx,iy,iz, Nd1,Nd2, coordTransf, r),
    cMass(cm), nlGeo(0), phiY(0.0),
    phiZ(0.0), L(0.0), ul(12), ql(12), ql0(12), kl(12,12), klgeo(12,12),
    Tgl(12,12), Ki(12,12), M(12,12), theLoad(12)
  {
    // get coordinate transformation type and save flag
    if(theCoordTransf->getClassName().substr(0,6)=="Linear")
      { nlGeo = 0; }
    else if(theCoordTransf->getClassName().substr(0,6)=="PDelta")
      { nlGeo = 1; }
    else if(theCoordTransf->getClassName().substr(0,5)=="Corot")
      {
        nlGeo = 1;
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "\n WARNING - Element: " << this->getTag() << std::endl
		  << "Unsupported Corotational transformation assigned.\n"
		  << "Using PDelta transformation instead.\n";
      }
    // zero fixed end forces vector
    ql0.Zero();
  }

//! @brief Virtual constructor.
XC::Element* XC::ElasticTimoshenkoBeam3d::getCopy(void) const
  { return new ElasticTimoshenkoBeam3d(*this); }

//! @brief Compute the current strain.
const XC::Vector &XC::ElasticTimoshenkoBeam3d::computeCurrentStrain(void) const
  {
    static Vector retval(5);
    theCoordTransf->update();
    retval= theCoordTransf->getBasicTrialDisp()/L;
    return retval;
  }

//! @brief Set the element domain.
void XC::ElasticTimoshenkoBeam3d::setDomain(Domain *theDomain)
  {
    ElasticBeam3dBase::setDomain(theDomain);
    // set up the transformation matrix for orientation
    this->setUp();
  }


int XC::ElasticTimoshenkoBeam3d::commitState()
  {
    int errCode = 0;
    
    // commit the base class
    errCode += this->ElasticBeam3dBase::commitState();
    
    // no need to commit coordinate transformation
    // since it is only used to get type of transf
    // errCode += theCoordTransf->commitState();
    
    return errCode;
  }


const XC::Matrix &XC::ElasticTimoshenkoBeam3d::getTangentStiff() const
  {
    // zero the matrix
    theMatrix.Zero();
    
    if (nlGeo == 0)
      {
        // transform from local to global system
        theMatrix.addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
        
      }
    else
      {
        // initialize local stiffness matrix
        static Matrix klTot(12,12);
        klTot.addMatrix(0.0, kl, 1.0);
        
        // get global trial displacements
        const Vector &dsp1 = theNodes[0]->getTrialDisp();
        const Vector &dsp2 = theNodes[1]->getTrialDisp();
        static Vector ug(12);
        for(int i=0; i<6; i++)
	  {
            ug(i)   = dsp1(i);
            ug(i+6) = dsp2(i);
          }
        
        // transform response from the global to the local system
        ul.addMatrixVector(0.0, Tgl, ug, 1.0);
        
        // get the resisting forces in local system
        ql.addMatrixVector(0.0, kl, ul, 1.0);
        
        // add geometric stiffness to local stiffness
        if (ql(6) != 0.0)
            klTot.addMatrix(1.0, klgeo, ql(6));
        
        // transform from local to global system
        theMatrix.addMatrixTripleProduct(0.0, Tgl, klTot, 1.0);
      }
    if(isDead())
      theMatrix*=dead_srf;    
    return theMatrix;
  }


const XC::Matrix &XC::ElasticTimoshenkoBeam3d::getInitialStiff() const
  {
    if(isDead())
      {
        theMatrix= Ki*dead_srf;
        return theMatrix;
      }
    else
      return Ki;
  }


const XC::Matrix &XC::ElasticTimoshenkoBeam3d::getMass(void) const
  {
    if(isDead())
      {
        theMatrix= M*dead_srf;
        return theMatrix;
      }
    else
      return M;
  }

//! @brief Zero the loads on the element.
void XC::ElasticTimoshenkoBeam3d::zeroLoad(void)
  {
    theLoad.Zero();
    ql0.Zero();
  }


int XC::ElasticTimoshenkoBeam3d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << getClassName() << "::" << __FUNCTION__ 
                << "; load over inactive element: "
                << getTag()  
                << std::endl;
    else
      {
	int type;
	const Vector &data = theLoad->getData(type, loadFactor);
	if(type == LOAD_TAG_Beam3dUniformLoad)
	  {
	    double wy = data(0)*loadFactor;  // Transverse
	    double wz = data(1)*loadFactor;  // Transverse
	    double wx = data(2)*loadFactor;  // Axial (+ve from node I to J)

	    double Vy = 0.5*wy*L;
	    double Mz = Vy*L/6.0; // wy*L*L/12
	    double Vz = 0.5*wz*L;
	    double My = Vz*L/6.0; // wz*L*L/12
	    double P  = 0.5*wx*L;

	    // fixed end forces in local system
	    ql0(0)  -= P;
	    ql0(1)  -= Vy;
	    ql0(2)  -= Vz;
	    ql0(4)  += My;
	    ql0(5)  -= Mz;
	    ql0(6)  -= P;
	    ql0(7)  -= Vy;
	    ql0(8)  -= Vz;
	    ql0(10) -= My;
	    ql0(11) += Mz;
	  }
	else
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; load type unknown for element: " 
		       << this->getTag() << ".\n";
	    return -1;
	  }
      }
    return 0;
  }


int XC::ElasticTimoshenkoBeam3d::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // check for quick return
    const double rho= getLinearRho(); // mass per unit length.
    if (rho == 0.0)
        return 0;
    
    // assemble Raccel vector
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    static Vector Raccel(12);
    for (int i=0; i<6; i++)  {
        Raccel(i)   = Raccel1(i);
        Raccel(i+6) = Raccel2(i);
    }
    
    // want to add ( - fact * M R * accel ) to unbalance
    theLoad.addMatrixVector(1.0, M, Raccel, -1.0);
    
    return 0;
  }


const XC::Vector& XC::ElasticTimoshenkoBeam3d::getResistingForce(void) const
  {
    // zero the residual
    theVector.Zero();
    
    // get global trial displacements
    const Vector &dsp1 = theNodes[0]->getTrialDisp();
    const Vector &dsp2 = theNodes[1]->getTrialDisp();
    static Vector ug(12);
    for (int i=0; i<6; i++)  {
        ug(i)   = dsp1(i);
        ug(i+6) = dsp2(i);
    }
    
    // transform response from the global to the local system
    ul.addMatrixVector(0.0, Tgl, ug, 1.0);
    
    // get the resisting forces in local system
    ql.addMatrixVector(0.0, kl, ul, 1.0);
    
    // add P-Delta moments to local forces
    if ((ql(6) != 0.0) && (nlGeo == 1))
        ql.addMatrixVector(1.0, klgeo, ul, ql(6));
    
    // add effects of element loads, ql = ql(ul) + ql0
    ql.addVector(1.0, ql0, 1.0);
    
    if(isDead()) //Set internal forces to zero when element is dead.
      ql*= dead_srf;
    
    // determine resisting forces in global system
    theVector.addMatrixTransposeVector(0.0, Tgl, ql, 1.0);
    
    if(isDead())
      theVector*=dead_srf;
    
    return theVector;
  }


const XC::Vector& XC::ElasticTimoshenkoBeam3d::getResistingForceIncInertia(void) const
  {
    // first get the resisting forces
    theVector = this->getResistingForce();
    
    // subtract external load
    theVector.addVector(1.0, theLoad, -1.0);
    
    // add the damping forces from rayleigh damping
    if(!rayFactors.nullValues())
      theVector.addVector(1.0, this->getRayleighDampingForces(), 1.0);
    
    // check for quick return
    const double rho= getLinearRho(); // mass per unit length.
    if (rho == 0.0)
        return theVector;
    
    // add inertia forces from element mass
    const Vector &accel1 = theNodes[0]->getTrialAccel();
    const Vector &accel2 = theNodes[1]->getTrialAccel();
    static Vector accel(12);
    for (int i=0; i<6; i++)
      {
        accel(i)   = accel1(i);
        accel(i+6) = accel2(i);
      }
    theVector.addMatrixVector(1.0, M, accel, 1.0);
    
    if(isDead())
      theVector*=dead_srf;
    
    return theVector;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ElasticTimoshenkoBeam3d::getDbTagData(void) const
  {
    static DbTagData retval(13);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::ElasticTimoshenkoBeam3d::sendData(Communicator &comm)
  {
    int res= ElasticBeam3dBase::sendData(comm);
    res+= comm.sendDoubles(phiY, phiZ, L, getDbTagData(),CommMetaData(11));
    res+= comm.sendInts(cMass, nlGeo, getDbTagData(),CommMetaData(12));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ElasticTimoshenkoBeam3d::recvData(const Communicator &comm)
  {
    int res= ElasticBeam3dBase::recvData(comm);
    res+= comm.receiveDoubles(phiY, phiZ,L,getDbTagData(),CommMetaData(11));
    res+= comm.receiveInts(cMass, nlGeo,getDbTagData(),CommMetaData(12));
    return res;
  }

int XC::ElasticTimoshenkoBeam3d::sendSelf(Communicator &comm)
  {
    inicComm(13);
    int res= sendData(comm);

    const int dataTag= getDbTag();
    res= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; could not send data.\n";
    return res;
  }

int XC::ElasticTimoshenkoBeam3d::recvSelf(const Communicator &comm)
  {
    const int dataTag= getDbTag();
    inicComm(13);
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data.\n";
    else
      res+= recvData(comm);
    return res;
  }


void XC::ElasticTimoshenkoBeam3d::Print(std::ostream &s, int flag) const
  {
    if (flag == 0)
      {
        // print everything
        s << "Element: " << this->getTag(); 
        s << "  type: ElasticTimoshenkoBeam3d";
        s << "\tConnected Nodes: " << theNodes;
        s << "  section: " << getSectionProperties() << std::endl;
        s << "  coordTransf: " << theCoordTransf->getClassName() << std::endl;
        s << "  linear rho: " << getLinearRho()
	  << "  cMass: " << cMass << std::endl;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << std::endl;
      }
    
    // if (flag == OPS_PRINT_PRINTMODEL_JSON)
    //   {
    //     s << "\t\t\t{";
    //     s << "\"name\": " << this->getTag() << ", ";
    //     s << "\"type\": \"ElasticTimoshenkoBeam3d\", ";
    //     s << "\"nodes\": [" << connectedExternalNodes(0) << ", " << connectedExternalNodes(1) << "], ";
    //     s << "\"E\": " << E << ", ";
    //     s << "\"G\": " << G << ", ";
    //     s << "\"A\": " << A << ", ";
    //     s << "\"Avy\": " << Avy << ", ";
    //     s << "\"Avz\": " << Avz << ", ";
    //     s << "\"Jx\": " << Jx << ", ";
    //     s << "\"Iy\": " << Iy << ", ";
    //     s << "\"Iz\": " << Iz << ", ";
    //     s << "\"massperlength\": " << getLinearRho() << ", ";
    //     s << "\"crdTransformation\": \"" << theCoordTransf->getTag() << "\"}";
    //   }
  }

XC::Response *XC::ElasticTimoshenkoBeam3d::setResponse(const std::vector<std::string> &argv, Parameter &param)
  {
    Response *theResponse = 0;
    
    // output.tag("ElementOutput");
    // output.attr("eleType","ElasticTimoshenkoBeam3d");
    // output.attr("eleTag",this->getTag());
    // output.attr("node1",connectedExternalNodes[0]);
    // output.attr("node2",connectedExternalNodes[1]);
    
    // global forces
    if ((argv[0]=="force") || (argv[0]=="forces") ||
        (argv[0]=="globalForce") || (argv[0]=="globalForces"))
    {
        // output.tag("ResponseType","Px_1");
        // output.tag("ResponseType","Py_1");
        // output.tag("ResponseType","Pz_1");
        // output.tag("ResponseType","Mx_1");
        // output.tag("ResponseType","My_1");
        // output.tag("ResponseType","Mz_1");
        // output.tag("ResponseType","Px_2");
        // output.tag("ResponseType","Py_2");
        // output.tag("ResponseType","Pz_2");
        // output.tag("ResponseType","Mx_2");
        // output.tag("ResponseType","My_2");
        // output.tag("ResponseType","Mz_2");
        
        theResponse =  new ElementResponse(this, 1, theVector);
      }
    // local forces
    else if((argv[0]=="localForce") || (argv[0]=="localForces"))
      {
        // output.tag("ResponseType","N_1");
        // output.tag("ResponseType","Vy_1");
        // output.tag("ResponseType","Vz_1");
        // output.tag("ResponseType","T_1");
        // output.tag("ResponseType","My_1");
        // output.tag("ResponseType","Mz_1");
        // output.tag("ResponseType","N_2");
        // output.tag("ResponseType","Vy_2");
        // output.tag("ResponseType","Vz_2");
        // output.tag("ResponseType","T_2");
        // output.tag("ResponseType","My_2");
        // output.tag("ResponseType","Mz_2");
        
        theResponse =  new ElementResponse(this, 2, theVector);
      }
    
    //output.endTag(); // ElementOutput
    
    return theResponse;
  }


int XC::ElasticTimoshenkoBeam3d::getResponse(int responseID, Information &eleInfo)
  {
    switch (responseID) {
    case 1: // global forces
        return eleInfo.setVector(this->getResistingForce());
    
    case 2: // local forces
        theVector.Zero();
        // determine resisting forces in local system
        theVector = ql;
        
        return eleInfo.setVector(theVector);
    
    default:
        return -1;
    }
  }


int XC::ElasticTimoshenkoBeam3d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const size_t argc= argv.size();
    if (argc < 1)
        return -1;
    else
      {
        const CrossSectionProperties3d &sprop= getSectionProperties();    
        if(argv[0]=="E") // E of the beam interior
	  {
	    param.setValue(sprop.E());
	    return param.addObject(1, this);
	  }
	else if(argv[0]=="G")
	  {
	    param.setValue(sprop.G());
	    return param.addObject(2, this);
	  }
	else if(argv[0]=="A") // A of the beam interior
	  {
	    param.setValue(sprop.A());
	    return param.addObject(3, this);
	  }
	else if(argv[0]=="J") // I of the beam interior
	  {
	    param.setValue(sprop.J());
	    return param.addObject(4, this);
	  }
	else if(argv[0]=="Iy") // I of the beam interior
	  {
	    param.setValue(sprop.Iy());
	    return param.addObject(5, this);
	  }
	else if(argv[0]=="Iz") // I of the beam interior
	  {
	    param.setValue(sprop.Iz());
	    return param.addObject(6, this);
	  }
        else if(argv[0]=="Avy")
	  {
	    param.setValue(sprop.getAvy());
            return param.addObject(7, this);
	  }
        else if(argv[0]=="Avz")
	  {
	    param.setValue(sprop.getAvz());
            return param.addObject(8, this);
	  }
	else if(argv[0]=="rho") // rho of the beam interior
	  {
	    param.setValue(sprop.getRho());
	    return param.addObject(9, this);
	  }
	else if(argv[0]=="linearRho") // rho of the beam interior
	  {
	    param.setValue(sprop.getLinearRho());
	    return param.addObject(10, this);
	  }
      }
    return -1;
  }


int XC::ElasticTimoshenkoBeam3d::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case -1:
        return -1;
      default:
        return getSectionProperties().updateParameter(parameterID,info);
      }
  }


void XC::ElasticTimoshenkoBeam3d::setUp()
  {  
    // determine the element length
    L = theCoordTransf->getInitialLength();

    if(L == 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING element: " << this->getTag()
		  << " has zero length.\n";
        return;
      }
    
    // get local axes vectors (these are already normalized)
    static Vector xAxis(3);
    static Vector yAxis(3);
    static Vector zAxis(3);
    theCoordTransf->getLocalAxes(xAxis, yAxis, zAxis);
    
    // create transformation matrix from global to local system
    Tgl.Zero();
    Tgl(0,0) = Tgl(3,3) = Tgl(6,6) = Tgl(9,9)   = xAxis(0);
    Tgl(0,1) = Tgl(3,4) = Tgl(6,7) = Tgl(9,10)  = xAxis(1);
    Tgl(0,2) = Tgl(3,5) = Tgl(6,8) = Tgl(9,11)  = xAxis(2);
    Tgl(1,0) = Tgl(4,3) = Tgl(7,6) = Tgl(10,9)  = yAxis(0);
    Tgl(1,1) = Tgl(4,4) = Tgl(7,7) = Tgl(10,10) = yAxis(1);
    Tgl(1,2) = Tgl(4,5) = Tgl(7,8) = Tgl(10,11) = yAxis(2);
    Tgl(2,0) = Tgl(5,3) = Tgl(8,6) = Tgl(11,9)  = zAxis(0);
    Tgl(2,1) = Tgl(5,4) = Tgl(8,7) = Tgl(11,10) = zAxis(1);
    Tgl(2,2) = Tgl(5,5) = Tgl(8,8) = Tgl(11,11) = zAxis(2);
    
    const CrossSectionProperties3d &sprop= getSectionProperties();
    const double A= sprop.A(); // A
    const double EA= sprop.EA(); // EA
    const double EAoverL= EA/L;	// EA/L
    const double Jx= sprop.J(); // J
    const double GJx= sprop.GJ(); // GJ
    const double EIy= sprop.EIy(); // EIz
    const double EIz= sprop.EIz(); // EIz
    const double Iy= sprop.Iy(); // Iz
    const double Iz= sprop.Iz(); // Iz
    const double GAvy= sprop.GAAlphaY(); // G*Avy
    const double GAvz= sprop.GAAlphaZ(); // G*Avz
    
    // determine ratios of bending to shear stiffness
    phiY = 12.0*EIy/(L*L*GAvz);
    phiZ = 12.0*EIz/(L*L*GAvy);
    
    // compute initial stiffness matrix in local system
    kl.Zero();
    kl(0,0) = kl(6,6) = EAoverL;
    kl(0,6) = kl(6,0) = -kl(0,0);
    kl(3,3) = kl(9,9) = GJx/L;
    kl(3,9) = kl(9,3) = -kl(3,3);
    double a1y = EIy/(L*L*L*(1.0 + phiY));
    kl(2,2) = kl(8,8) = a1y*12.0;
    kl(2,8) = kl(8,2) = -kl(2,2);
    kl(4,4) = kl(10,10) = a1y*L*L*(4.0 + phiY);
    kl(4,10) = kl(10,4) = a1y*L*L*(2.0 - phiY);
    kl(2,4) = kl(4,2) = kl(2,10) = kl(10,2) = -a1y*L*6.0;
    kl(4,8) = kl(8,4) = kl(8,10) = kl(10,8) = -kl(2,4);
    double a1z = EIz/(L*L*L*(1.0 + phiZ));
    kl(1,1) = kl(7,7) = a1z*12.0;
    kl(1,7) = kl(7,1) = -kl(1,1);
    kl(5,5) = kl(11,11) = a1z*L*L*(4.0 + phiZ);
    kl(5,11) = kl(11,5) = a1z*L*L*(2.0 - phiZ);
    kl(1,5) = kl(5,1) = kl(1,11) = kl(11,1) = a1z*L*6.0;
    kl(5,7) = kl(7,5) = kl(7,11) = kl(11,7) = -kl(1,5);
    
    // compute geometric stiffness matrix in local system
    klgeo.Zero();
    if(nlGeo == 1)
      {
        double b1y = 1.0/(30.0*L*pow(1.0 + phiY,2));
        klgeo(2,2) = klgeo(8,8) = b1y*(30.0*phiY*phiY + 60.0*phiY + 36.0);
        klgeo(2,8) = klgeo(8,2) = -klgeo(2,2);
        klgeo(4,4) = klgeo(10,10) = b1y*L*L*(2.5*phiY*phiY + 5.0*phiY + 4.0);
        klgeo(4,10) = klgeo(10,4) = -b1y*L*L*(2.5*phiY*phiY + 5.0*phiY + 1.0);
        klgeo(2,4) = klgeo(4,2) = klgeo(2,10) = klgeo(10,2) = -b1y*L*3.0;
        klgeo(4,8) = klgeo(8,4) = klgeo(8,10) = klgeo(10,8) = -klgeo(2,4);
        double b1z = 1.0/(30.0*L*pow(1.0 + phiZ,2));
        klgeo(1,1) = klgeo(7,7) = b1z*(30.0*phiZ*phiZ + 60.0*phiZ + 36.0);
        klgeo(1,7) = klgeo(7,1) = -klgeo(1,1);
        klgeo(5,5) = klgeo(11,11) = b1z*L*L*(2.5*phiZ*phiZ + 5.0*phiZ + 4.0);
        klgeo(5,11) = klgeo(11,5) = -b1z*L*L*(2.5*phiZ*phiZ + 5.0*phiZ + 1.0);
        klgeo(1,5) = klgeo(5,1) = klgeo(1,11) = klgeo(11,1) = b1z*L*3.0;
        klgeo(5,7) = klgeo(7,5) = klgeo(7,11) = klgeo(11,7) = -klgeo(1,5);
      }
    
    // compute initial stiffness matrix in global system
    Ki.addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
    
    // compute mass matrix in global system
    M.Zero();
    const double rho= getLinearRho(); // mass per unit length.
    if(rho > 0.0)
      {
        if(cMass == 0)
	  {
            // lumped mass matrix
            double m = 0.5*rho*L;
            for (int i=0; i<3; i++)  {
                M(i,i)     = m;
                M(i+6,i+6) = m;
            }
          }
	else
	  {
            // consistent mass matrix
            Matrix mlTrn(12,12), mlRot(12,12), ml(12,12);
            mlTrn.Zero(); mlRot.Zero(); ml.Zero();
            double c1x = rho*L/210.0;
            mlTrn(0,0) = mlTrn(6,6) = c1x*70.0;
            mlTrn(0,6) = mlTrn(6,0) = c1x*35.0;
            double c2x = rho/A*Jx*L/210.0;
            mlTrn(3,3) = mlTrn(9,9) = c2x*70.0;
            mlTrn(3,9) = mlTrn(9,3) = c2x*35.0;
            double c1y = c1x/pow(1.0 + phiY,2);
            mlTrn(2,2) = mlTrn(8,8) = c1y*(70.0*phiY*phiY + 147.0*phiY + 78.0);
            mlTrn(2,8) = mlTrn(8,2) = c1y*(35.0*phiY*phiY + 63.0*phiY + 27.0);
            mlTrn(4,4) = mlTrn(10,10) = c1y*L*L/4.0*(7.0*phiY*phiY + 14.0*phiY + 8.0);
            mlTrn(4,10) = mlTrn(10,4) = -c1y*L*L/4.0*(7.0*phiY*phiY + 14.0*phiY + 6.0);
            mlTrn(2,4) = mlTrn(4,2) = -c1y*L/4.0*(35.0*phiY*phiY + 77.0*phiY + 44.0);
            mlTrn(8,10) = mlTrn(10,8) = -mlTrn(2,4);
            mlTrn(2,10) = mlTrn(10,2) = c1y*L/4.0*(35.0*phiY*phiY + 63.0*phiY + 26.0);
            mlTrn(4,8) = mlTrn(8,4) = -mlTrn(2,10);
            double c2y = rho/A*Iy/(30.0*L*pow(1.0 + phiY,2));
            mlRot(2,2) = mlRot(8,8) = c2y*36.0;
            mlRot(2,8) = mlRot(8,2) = -mlRot(2,2);
            mlRot(4,4) = mlRot(10,10) = c2y*L*L*(10.0*phiY*phiY + 5.0*phiY + 4.0);
            mlRot(4,10) = mlRot(10,4) = c2y*L*L*(5.0*phiY*phiY - 5.0*phiY - 1.0);
            mlRot(2,4) = mlRot(4,2) = mlRot(2,10) = mlRot(10,2) = c2y*L*(15.0*phiY - 3.0);
            mlRot(4,8) = mlRot(8,4) = mlRot(8,10) = mlRot(10,8) = -mlRot(2,4);
            double c1z = c1x/pow(1.0 + phiZ,2);
            mlTrn(1,1) = mlTrn(7,7) = c1z*(70.0*phiZ*phiZ + 147.0*phiZ + 78.0);
            mlTrn(1,7) = mlTrn(7,1) = c1z*(35.0*phiZ*phiZ + 63.0*phiZ + 27.0);
            mlTrn(5,5) = mlTrn(11,11) = c1z*L*L/4.0*(7.0*phiZ*phiZ + 14.0*phiZ + 8.0);
            mlTrn(5,11) = mlTrn(11,5) = -c1z*L*L/4.0*(7.0*phiZ*phiZ + 14.0*phiZ + 6.0);
            mlTrn(1,5) = mlTrn(5,1) = c1z*L/4.0*(35.0*phiZ*phiZ + 77.0*phiZ + 44.0);
            mlTrn(7,11) = mlTrn(11,7) = -mlTrn(1,5);
            mlTrn(1,11) = mlTrn(11,1) = -c1z*L/4.0*(35.0*phiZ*phiZ + 63.0*phiZ + 26.0);
            mlTrn(5,7) = mlTrn(7,5) = -mlTrn(1,11);
            double c2z = rho/A*Iz/(30.0*L*pow(1.0 + phiZ,2));
            mlRot(1,1) = mlRot(7,7) = c2z*36.0;
            mlRot(1,7) = mlRot(7,1) = -mlRot(1,1);
            mlRot(5,5) = mlRot(11,11) = c2z*L*L*(10.0*phiZ*phiZ + 5.0*phiZ + 4.0);
            mlRot(5,11) = mlRot(11,5) = c2z*L*L*(5.0*phiZ*phiZ - 5.0*phiZ - 1.0);
            mlRot(1,5) = mlRot(5,1) = mlRot(1,11) = mlRot(11,1) = -c2z*L*(15.0*phiZ - 3.0);
            mlRot(5,7) = mlRot(7,5) = mlRot(7,11) = mlRot(11,7) = -mlRot(1,5);
            // add translational and rotational parts
            ml = mlTrn + mlRot;
            // transform from local to global system
            M.addMatrixTripleProduct(0.0, Tgl, ml, 1.0);
	  }
      }
  }
