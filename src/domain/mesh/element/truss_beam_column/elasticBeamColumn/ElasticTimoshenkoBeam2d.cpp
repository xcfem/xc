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
// $URL$

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 03/13
// Revision: B
//
// Purpose: This file contains the class definition for ElasticTimoshenkoBeam2d.
// ElasticTimoshenkoBeam2d is a 2d beam element. As such it can only
// connect to a node with 3-dof.
//
// Revision Log:
//  - Revision B
//      Date:   12/24/2020
//      By:     Pearl Ranchal (ranchal@berkeley.edu)
//      Notes:  In setUp(), get element length from theCoordTransf instead of computing from nodal coordinates.
// 

#include "ElasticTimoshenkoBeam2d.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf2d.h"
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include "utility/matrix/Matrix.h"
#include "domain/mesh/node/Node.h"
#include "domain/component/Parameter.h"
#include "domain/load/ElementalLoad.h"
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"

// initialize the class wide variables
XC::Matrix XC::ElasticTimoshenkoBeam2d::theMatrix(6,6);
XC::Vector XC::ElasticTimoshenkoBeam2d::theVector(6);

//! @brief Default constructor.
//! @param tag: element identifier.
XC::ElasticTimoshenkoBeam2d::ElasticTimoshenkoBeam2d(int tag)
    : ElasticBeam2dBase(tag, ELE_TAG_ElasticTimoshenkoBeam2d),
      cMass(0), nlGeo(0), phi(0.0), L(0.0),
      ul(6), ql(6), ql0(6), kl(6,6), klgeo(6,6), Tgl(6,6), Ki(6,6), M(6,6),
      theLoad(6)
  {
    // zero fixed end forces vector
    ql0.Zero();
  }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param m: element material.
//! @param trf: element coordinate transformation.
XC::ElasticTimoshenkoBeam2d::ElasticTimoshenkoBeam2d(int tag,const Material *m,const CrdTransf *trf)
  : ElasticBeam2dBase(tag, ELE_TAG_ElasticBeam2d,m,trf),
    cMass(0), nlGeo(0), phi(0.0), L(0.0),
    ul(6), ql(6), ql0(6), kl(6,6), klgeo(6,6), Tgl(6,6), Ki(6,6), M(6,6),
    theLoad(6)
  {
    // zero fixed end forces vector
     ql0.Zero();
  }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param a: section area.
//! @param e: elastic modulus.
//! @param g: shear modulus.
//! @param i: moment of inertia.
//! @param avy: shear area.
//! @param Nd1: identifier of the first node.
//! @param Nd2: idenfifier of the second node.
//! @param coordTransf: coordinate transformation for the element.
//! @param r: density of the element material.
XC::ElasticTimoshenkoBeam2d::ElasticTimoshenkoBeam2d(int tag,
    double a, double e, double g, double iz, double avy,
    int Nd1, int Nd2, CrdTransf2d &coordTransf,
    double r, int cm)
  : ElasticBeam2dBase(tag, ELE_TAG_ElasticTimoshenkoBeam2d,a,avy/a,e,g,iz,Nd1,Nd2, coordTransf, r),
    cMass(cm), nlGeo(0), phi(0.0), L(0.0),
    ul(6), ql(6), ql0(6), kl(6,6), klgeo(6,6), Tgl(6,6), Ki(6,6), M(6,6),
    theLoad(6)
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
XC::Element* XC::ElasticTimoshenkoBeam2d::getCopy(void) const
  { return new ElasticTimoshenkoBeam2d(*this); }

//! @brief Compute the current strain.
const XC::Vector &XC::ElasticTimoshenkoBeam2d::computeCurrentStrain(void) const
  {
    static Vector retval(3);
    theCoordTransf->update();
    retval= theCoordTransf->getBasicTrialDisp()/L;
    return retval;
  }

//! @brief Set the element domain.
void XC::ElasticTimoshenkoBeam2d::setDomain(Domain *theDomain)
  {
    ElasticBeam2dBase::setDomain(theDomain);
    
    // set up the transformation matrix for orientation
    this->setUp();
  }


int XC::ElasticTimoshenkoBeam2d::commitState(void)
  {
    int errCode = 0;
    
    // commit the base class
    errCode += this->ElasticBeam2dBase::commitState();
    
    // no need to commit coordinate transformation
    // since it is only used to get type of transf
    // errCode += theCoordTransf->commitState();
    
    return errCode;
  }


const XC::Matrix &XC::ElasticTimoshenkoBeam2d::getTangentStiff(void) const
  {
    // zero the matrix
    theMatrix.Zero();
    
    if(nlGeo == 0)
      {
        // transform from local to global system
        theMatrix.addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
        
      }
    else
      {
        // initialize local stiffness matrix
        static Matrix klTot(6,6);
        klTot.addMatrix(0.0, kl, 1.0);
        
        // get global trial displacements
        const Vector &dsp1 = theNodes[0]->getTrialDisp();
        const Vector &dsp2 = theNodes[1]->getTrialDisp();
        static Vector ug(6);
        for (int i=0; i<3; i++)
	  {
            ug(i)   = dsp1(i);
            ug(i+3) = dsp2(i);
          }
        
        // transform response from the global to the local system
        ul.addMatrixVector(0.0, Tgl, ug, 1.0);
        
        // get the resisting forces in local system
        ql.addMatrixVector(0.0, kl, ul, 1.0);
        
        // add geometric stiffness to local stiffness
        if(ql(3) != 0.0)
            klTot.addMatrix(1.0, klgeo, ql(3));
        
        // transform from local to global system
        theMatrix.addMatrixTripleProduct(0.0, Tgl, klTot, 1.0);
      }
    if(isDead())
      theMatrix*=dead_srf;
    return theMatrix;
  }


const XC::Matrix &XC::ElasticTimoshenkoBeam2d::getInitialStiff(void) const
  {
    if(isDead())
      {
        theMatrix= Ki*dead_srf;
        return theMatrix;
      }
    else
      return Ki;
  }


const XC::Matrix &XC::ElasticTimoshenkoBeam2d::getMass(void) const
  {
    if(isDead())
      {
        theMatrix= M*dead_srf;
        return theMatrix;
      }
    else
      return M;
  }


void XC::ElasticTimoshenkoBeam2d::zeroLoad(void)
  {
    ElasticBeam2dBase::zeroLoad();
    theLoad.Zero();
    ql0.Zero();    
    return;
  }


int XC::ElasticTimoshenkoBeam2d::addLoad(ElementalLoad *load, double loadFactor)
  {
    if(isDead())
      std::cerr << getClassName() << "::" << __FUNCTION__ 
                << "; load over inactive element: "
                << getTag()  
                << std::endl;
    else
      {
	int type;
	const Vector &data = load->getData(type, loadFactor);

	if(type == LOAD_TAG_Beam2dUniformLoad)
	  {
	    double wt = data(0)*loadFactor;  // Transverse (+ve upward)
	    double wa = data(1)*loadFactor;  // Axial (+ve from node I to J)

	    double V = 0.5*wt*L;
	    double M = V*L/6.0;  // wt*L*L/12
	    double P = 0.5*wa*L;

	    // fixed end forces in local system
	    ql0(0) -= P;
	    ql0(1) -= V;
	    ql0(2) -= M;
	    ql0(3) -= P;
	    ql0(4) -= V;
	    ql0(5) += M;
	  }
	else
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      <<"; load type unknown for element: "
		      << this->getTag() << ".\n";
	    return -1;
	  }
      }
    return 0;
  }


int XC::ElasticTimoshenkoBeam2d::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // check for quick return
    const double rho= getRho();
    if (rho == 0.0)
        return 0;
    
    // assemble Raccel vector
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    static Vector Raccel(6);
    for (int i=0; i<3; i++)
      {
        Raccel(i)   = Raccel1(i);
        Raccel(i+3) = Raccel2(i);
      }
    
    // want to add ( - fact * M R * accel ) to unbalance
    theLoad.addMatrixVector(1.0, M, Raccel, -1.0);
    
    return 0;
  }


const XC::Vector &XC::ElasticTimoshenkoBeam2d::getResistingForce(void) const
  {
    // zero the residual
    theVector.Zero();
    
    // get global trial displacements
    const Vector &dsp1 = theNodes[0]->getTrialDisp();
    const Vector &dsp2 = theNodes[1]->getTrialDisp();
    static Vector ug(6);
    for (int i=0; i<3; i++)
      {
        ug(i)   = dsp1(i);
        ug(i+3) = dsp2(i);
      }
    
    // transform response from the global to the local system
    ul.addMatrixVector(0.0, Tgl, ug, 1.0);
    
    // get the resisting forces in local system
    ql.addMatrixVector(0.0, kl, ul, 1.0);
    
    // add P-Delta moments to local forces
    if ((ql(3) != 0.0) && (nlGeo == 1))
        ql.addMatrixVector(1.0, klgeo, ul, ql(3));
    
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


const XC::Vector &XC::ElasticTimoshenkoBeam2d::getResistingForceIncInertia(void) const
  {
    // first get the resisting forces
    theVector = this->getResistingForce();
    
    // subtract external load
    theVector.addVector(1.0, theLoad, -1.0);
    
    // add the damping forces from rayleigh damping
    if(!rayFactors.nullValues())
      theVector.addVector(1.0, this->getRayleighDampingForces(), 1.0);
    
    // check for quick return
    const double rho= getRho();
    if (rho == 0.0)
        return theVector;
    
    // add inertia forces from element mass
    const Vector &accel1 = theNodes[0]->getTrialAccel();
    const Vector &accel2 = theNodes[1]->getTrialAccel();
    static Vector accel(6);
    for (int i=0; i<3; i++)
      {
        accel(i)   = accel1(i);
        accel(i+3) = accel2(i);
      }
    theVector.addMatrixVector(1.0, M, accel, 1.0);
    
    if(isDead())
      theVector*=dead_srf;
    
    return theVector;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ElasticTimoshenkoBeam2d::getDbTagData(void) const
  {
    static DbTagData retval(13);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::ElasticTimoshenkoBeam2d::sendData(Communicator &comm)
  {
    int res= ElasticBeam2dBase::sendData(comm);
    res+= comm.sendDoubles(phi,L,getDbTagData(),CommMetaData(11));
    res+= comm.sendInts(cMass, nlGeo, getDbTagData(),CommMetaData(12));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ElasticTimoshenkoBeam2d::recvData(const Communicator &comm)
  {
    int res= ElasticBeam2dBase::recvData(comm);
    res+= comm.receiveDoubles(phi,L,getDbTagData(),CommMetaData(11));
    res+= comm.receiveInts(cMass, nlGeo,getDbTagData(),CommMetaData(12));
    return res;
  }

int XC::ElasticTimoshenkoBeam2d::sendSelf(Communicator &comm)
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

int XC::ElasticTimoshenkoBeam2d::recvSelf(const Communicator &comm)
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

void XC::ElasticTimoshenkoBeam2d::Print(std::ostream &s, int flag) const
  {
    if (flag == 0)
      {
        // print everything
        s << "Element: " << this->getTag();
        s << "  type: ElasticTimoshenkoBeam2d";
        s << "\tConnected Nodes: " << theNodes;
        s << "  section: " << getSectionProperties() << std::endl;
        s << "  coordTransf: " << theCoordTransf->getClassName() << std::endl;
        s << "  rho: " << getRho() << "  cMass: " << cMass << std::endl;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << std::endl;
    }
    
    // if (flag == OPS_PRINT_PRINTMODEL_JSON)
    //   {
    //     s << "\t\t\t{";
    //     s << "\"name\": " << this->getTag() << ", ";
    //     s << "\"type\": \"ElasticTimoshenkoBeam2d\", ";
    //     s << "\"nodes\": [" << connectedExternalNodes(0) << ", " << connectedExternalNodes(1) << "], ";
    //     s << "\"E\": " << E << ", ";
    //     s << "\"G\": " << G << ", ";
    //     s << "\"A\": " << A << ", ";
    //     s << "\"Avy\": " << Avy << ", ";
    //     s << "\"Iz\": " << Iz << ", ";
    //     s << "\"massperlength\": " << getRho() << ", ";
    //     s << "\"crdTransformation\": \"" << theCoordTransf->getTag() << "\"}";
    // }
  }


XC::Response *XC::ElasticTimoshenkoBeam2d::setResponse(const std::vector<std::string> &argv, Parameter &param)
  {
    Response *theResponse= nullptr;
    
    // output.tag("ElementOutput");
    // output.attr("eleType","ElasticTimoshenkoBeam2d");
    // output.attr("eleTag",this->getTag());
    // output.attr("node1",connectedExternalNodes[0]);
    // output.attr("node2",connectedExternalNodes[1]);
    
    // global forces
    if ((argv[0]=="force") || (argv[0]=="forces") ||
        (argv[0]=="globalForce") || (argv[0]=="globalForces"))
      {
        // output.tag("ResponseType","Px_1");
        // output.tag("ResponseType","Py_1");
        // output.tag("ResponseType","Mz_1");
        // output.tag("ResponseType","Px_2");
        // output.tag("ResponseType","Py_2");
        // output.tag("ResponseType","Mz_2");
        
        theResponse =  new ElementResponse(this, 1, theVector);
      }
    // local forces
    else if((argv[0]=="localForce") || (argv[0]=="localForces"))
      {
        // output.tag("ResponseType","N_1");
        // output.tag("ResponseType","V_1");
        // output.tag("ResponseType","M_1");
        // output.tag("ResponseType","N_2");
        // output.tag("ResponseType","V_2");
        // output.tag("ResponseType","M_2");
        
        theResponse = new ElementResponse(this, 2, theVector);
      }
    
    // output.endTag(); // ElementOutput
    
    return theResponse;
  }


int XC::ElasticTimoshenkoBeam2d::getResponse(int responseID, Information &eleInfo)
  {
    switch (responseID)
     {
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

int XC::ElasticTimoshenkoBeam2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const size_t argc= argv.size();
    if(argc < 1)
        return -1;
    else
      {
        const CrossSectionProperties2d &sprop= getSectionProperties();    
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
	else if(argv[0]=="I") // I of the beam interior
	  {
	    param.setValue(sprop.I());
	    return param.addObject(4, this);
	  }
        else if(argv[0]=="Avy")
	  {
	    param.setValue(sprop.getAvy());
            return param.addObject(5, this);
	  }
	else if(argv[0]=="rho") // rho of the beam interior
	  {
	    param.setValue(sprop.getRho());
	    return param.addObject(4, this);
	  }
      }
    return -1;
  }


int XC::ElasticTimoshenkoBeam2d::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case -1:
	  return -1;
      default:
        return getSectionProperties().updateParameter(parameterID,info);
      }
  }


void XC::ElasticTimoshenkoBeam2d::setUp(void)
  {
    // element projection
    static Vector dx(2);
    
    const Vector &ndICoords = theNodes[0]->getCrds();
    const Vector &ndJCoords = theNodes[1]->getCrds();
    
    dx = ndJCoords - ndICoords;
    
    // determine the element length
    L= theCoordTransf->getInitialLength();

    if(L == 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; element: " << this->getTag()
		  << " has zero length.\n";
        return;
      }
    
    // create transformation matrix from global to local system
    Tgl.Zero();
    Tgl(0,0) = Tgl(1,1) = Tgl(3,3) = Tgl(4,4) = dx(0)/L;
    Tgl(0,1) = Tgl(3,4) = dx(1)/L;
    Tgl(1,0) = Tgl(4,3) = -dx(1)/L;
    Tgl(2,2) = Tgl(5,5) = 1.0;
    
    const CrossSectionProperties2d &sprop= getSectionProperties();
    const double A= sprop.A(); // A
    const double EA= sprop.EA(); // EA
    const double EAoverL= EA/L;	// EA/L
    const double EIz= sprop.EI(); // EI
    const double Iz= sprop.I(); // I
    const double GAvy= sprop.GAAlpha(); // G*Avy
    // determine ratio of bending to shear stiffness
    phi = 12.0*EIz/(L*L*GAvy);
    
    // compute initial stiffness matrix in local system
    kl.Zero();
    kl(0,0) = kl(3,3) = EAoverL;
    kl(0,3) = kl(3,0) = -kl(0,0);
    double a1z = EIz/(L*L*L*(1.0 + phi));
    kl(1,1) = kl(4,4) = a1z*12.0;
    kl(1,4) = kl(4,1) = -kl(1,1);
    kl(2,2) = kl(5,5) = a1z*L*L*(4.0 + phi);
    kl(2,5) = kl(5,2) = a1z*L*L*(2.0 - phi);
    kl(1,2) = kl(2,1) = kl(1,5) = kl(5,1) = a1z*L*6.0;
    kl(2,4) = kl(4,2) = kl(4,5) = kl(5,4) = -kl(1,2);
    
    // compute geometric stiffness matrix in local system
    klgeo.Zero();
    if(nlGeo == 1)
      {
        double b1z = 1.0/(30.0*L*pow(1.0 + phi,2));
        klgeo(1,1) = klgeo(4,4) = b1z*(30.0*phi*phi + 60.0*phi + 36.0);
        klgeo(1,4) = klgeo(4,1) = -klgeo(1,1);
        klgeo(2,2) = klgeo(5,5) = b1z*L*L*(2.5*phi*phi + 5.0*phi + 4.0);
        klgeo(2,5) = klgeo(5,2) = -b1z*L*L*(2.5*phi*phi + 5.0*phi + 1.0);
        klgeo(1,2) = klgeo(2,1) = klgeo(1,5) = klgeo(5,1) = b1z*L*3.0;
        klgeo(2,4) = klgeo(4,2) = klgeo(4,5) = klgeo(5,4) = -klgeo(1,2);
      }
    
    // compute initial stiffness matrix in global system
    Ki.addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
    
    // compute mass matrix in global system
    M.Zero();
    const double rho= getRho();
    if(rho > 0.0)
      {
        if(cMass == 0)
          {
            // lumped mass matrix
            const double m = 0.5*rho*L;
            for(int i=0; i<2; i++)
              {
                M(i,i)     = m;
                M(i+3,i+3) = m;
              }
          }
        else
          {
            // consistent mass matrix
            Matrix mlTrn(6,6), mlRot(6,6), ml(6,6);
            mlTrn.Zero(); mlRot.Zero(); ml.Zero();
            double c1x = rho*L/210.0;
            mlTrn(0,0) = mlTrn(3,3) = c1x*70.0;
            mlTrn(0,3) = mlTrn(3,0) = c1x*35.0;
            double c1z = c1x/pow(1.0 + phi,2);
            mlTrn(1,1) = mlTrn(4,4) = c1z*(70.0*phi*phi + 147.0*phi + 78.0);
            mlTrn(1,4) = mlTrn(4,1) = c1z*(35.0*phi*phi + 63.0*phi + 27.0);
            mlTrn(2,2) = mlTrn(5,5) = c1z*L*L/4.0*(7.0*phi*phi + 14.0*phi + 8.0);
            mlTrn(2,5) = mlTrn(5,2) = -c1z*L*L/4.0*(7.0*phi*phi + 14.0*phi + 6.0);
            mlTrn(1,2) = mlTrn(2,1) = c1z*L/4.0*(35.0*phi*phi + 77.0*phi + 44.0);
            mlTrn(4,5) = mlTrn(5,4) = -mlTrn(1,2);
            mlTrn(1,5) = mlTrn(5,1) = -c1z*L/4.0*(35.0*phi*phi + 63.0*phi + 26.0);
            mlTrn(2,4) = mlTrn(4,2) = -mlTrn(1,5);
            double c2z = rho/A*Iz/(30.0*L*pow(1.0 + phi,2));
            mlRot(1,1) = mlRot(4,4) = c2z*36.0;
            mlRot(1,4) = mlRot(4,1) = -mlRot(1,1);
            mlRot(2,2) = mlRot(5,5) = c2z*L*L*(10.0*phi*phi + 5.0*phi + 4.0);
            mlRot(2,5) = mlRot(5,2) = c2z*L*L*(5.0*phi*phi - 5.0*phi - 1.0);
            mlRot(1,2) = mlRot(2,1) = mlRot(1,5) = mlRot(5,1) = -c2z*L*(15.0*phi - 3.0);
            mlRot(2,4) = mlRot(4,2) = mlRot(4,5) = mlRot(5,4) = -mlRot(1,2);
            // add translational and rotational parts
            ml = mlTrn + mlRot;
            // transform from local to global system
            M.addMatrixTripleProduct(0.0, Tgl, ml, 1.0);
        }
    }
}
