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
** file 'COPYRIGHT'  in main dInrectory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// Written: fmk
//
// What: "@(#) MultiFP2d.C, revA"

// we specify what header files we need
#include "MultiFP2d.h"
#include "material/uniaxial/MultiLinear.h"
#include "material/uniaxial/ENTMaterial.h"
#include "utility/recorder/response/ElementResponse.h"
#include "material/uniaxial/UniaxialMaterial.h"

static XC::Matrix ZeroLengthM4(4,4);   // class wide matrix for 4*4
static XC::Matrix ZeroLengthM6(6,6);   // class wide matrix for 6*6

//! @brief Free the friction model of the bearing.
void XC::MultiFP2d::free_friction_model(void)
  {
    if(theFrictionModel)
      delete theFrictionModel;
    theFrictionModel= nullptr;
  }

//! @brief Alloc the friction model of the bearing.
void XC::MultiFP2d::alloc_friction_model(const UniaxialMaterial &thefrnmdl)
  {
    free_friction_model();
    theFrictionModel= thefrnmdl.getCopy();
    if(!theFrictionModel)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to get copy of the friction model.\n";
	exit(-1);
      }
  }

//! @brief Free the vertical response of the bearing.
void XC::MultiFP2d::free_vertical_model(void)
  {
    if(theVerticalModel)
      delete theVerticalModel;
    theVerticalModel= nullptr;
  }

//! @brief Alloc the vertical response of the bearing.
void XC::MultiFP2d::alloc_vertical_model(const UniaxialMaterial &vert)
  {
    free_vertical_model();
    theVerticalModel= vert.getCopy();
    if(!theVerticalModel)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to get copy of the vertical model.\n";
	exit(-1);
      }
  }

XC::MultiLinear get_multilinear_material(const XC::Vector &R,
					 const XC::Vector &h,
					 const XC::Vector &D,
					 const XC::Vector &d,
					 const XC::Vector &mu)
  {
      const double L0 = R(0)-h(0);
      const double L1 = R(1)-h(1);
      const double L2 = R(2)-h(2);
    
      const double u1bar = L1*(D(1)-d(1))/(2.0*R(1));
      const double u2bar = L2*(D(2)-d(2))/(2.0*R(2));

      XC::Vector fy(5);
      XC::Vector u(5);
    
      fy(0) = mu(0);
      fy(1) = mu(1);
      fy(2) = mu(2);
    
      u(1) = 2*L0*(mu(1)-mu(0));
      u(0) = u(1)/100.0;
      u(2) = L0*(mu(1)+mu(2) - 2*mu(0)) + L1*(mu(2) - mu(1));
      u(3) = u(2) + (u1bar/L1 + mu(1) - mu(2))*(L1+L2);
      u(4) = u(3)+(u2bar/L2 + mu(2) - u1bar/L1 - mu(1))*(L0+L2);
    
      fy(3) = fy(2) + u1bar/L1 + mu(1) - mu(2);
      fy(4) = fy(3) + u2bar/L2 + mu(2) - u1bar/L1 - mu(1);
    
      return XC::MultiLinear(1, fy, u);     
  }

//! @brief Constructor
XC::MultiFP2d::MultiFP2d(int tag,  int Nd1, int Nd2, 
			 const UniaxialMaterial &friction,
			 const UniaxialMaterial &vertical,
			 double w0, int aCase)
  :Element0D(tag, ELE_TAG_MultiFP2d,Nd1,Nd2, 2),
   theFrictionModel(nullptr),
   theVerticalModel(nullptr),
   type(0), axialCase(aCase),
   W0(w0), cW(w0),
   theMatrix(), theVector()
  {
    alloc_friction_model(friction);
    alloc_vertical_model(friction);
  }

// typical constructor
XC::MultiFP2d::MultiFP2d(int tag, 
			 int Nd1, int Nd2,
			 int type,
			 const Vector &R,
			 const Vector &h,
			 const Vector &D,
			 const Vector &d,
			 const Vector &mu,
			 double Kv,
			 double w0,
			 int aCase)
  : Element0D(tag, ELE_TAG_MultiFP2d, Nd1,Nd2, 2),     
    theFrictionModel(nullptr),
    theVerticalModel(nullptr),
    type(0), axialCase(aCase),
    W0(w0), cW(w0),
    theMatrix(), theVector()
  {
    alloc_vertical_model(ENTMaterial(2, Kv));
    if(type == 3)
      {
	const MultiLinear tmp= get_multilinear_material(R, h, D, d, mu);
	alloc_friction_model(tmp);
      }
  }

//! @brief Default constructor.
XC::MultiFP2d::MultiFP2d(void)
  :Element0D(0, ELE_TAG_MultiFP2d, 0, 0, 2), 
   theFrictionModel(nullptr),
   theVerticalModel(nullptr),
   type(0), axialCase(0),
   W0(0.0), cW(0.0),
   theMatrix(), theVector()
  {
  }

XC::MultiFP2d::MultiFP2d(const MultiFP2d &other)
  :Element0D(other), theFrictionModel(nullptr), theVerticalModel(nullptr),
   type(other.type), axialCase(other.axialCase),
   W0(other.W0), cW(other.cW),
   theMatrix(other.theMatrix), theVector(other.theVector)
  {
    if(other.theFrictionModel)
      alloc_friction_model(*other.theFrictionModel);
    if(other.theVerticalModel)
      alloc_vertical_model(*other.theVerticalModel);
  }

XC::MultiFP2d &XC::MultiFP2d::operator=(const MultiFP2d &other)
  {
    Element0D::operator=(other);
    type= other.type;
    axialCase= other.axialCase;
    W0= other.W0;
    cW= other.cW;
    theMatrix= other.theMatrix;
    theVector= other.theVector;

    if(other.theFrictionModel)
      alloc_friction_model(*other.theFrictionModel);
    if(other.theVerticalModel)
      alloc_vertical_model(*other.theVerticalModel);
    return *this;
  }
	     
//! @brief Destructor.
XC::MultiFP2d::~MultiFP2d(void)
  {
    free_friction_model();
    free_vertical_model();
  }

//! @brief Virtual constructor.
XC::Element *XC::MultiFP2d::getCopy() const
  { return new MultiFP2d(*this); }

//! @brief to set a link to the enclosing Domain, ensure nodes exist in Domain
//! and set pointers to these nodes, also determines the length and 
//! transformation Matrix.
void XC::MultiFP2d::setDomain(Domain *theDomain)
  {
  // check Domain is not null - invoked when object removed from a domain
  if(theDomain)
    {
      Element0D::setDomain(theDomain);
      // first ensure nodes exist in Domain and set the node pointers

  
      // ensure connected nodes have correct number of dof's
      const int dofNd1 = theNodes[0]->getNumberDOF();
      const int dofNd2 = theNodes[1]->getNumberDOF();	
      if ((dofNd1 != dofNd2) || ((dofNd2 != 2) && (dofNd2 != 3)) )
	{
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; 2 or 3 dof required at nodes"
		    << std::endl;
	  exit(-1);
	  return;
	}	

      if(dofNd2 == 2)
	{
	  theMatrix= Matrix(4,4);
	  theVector= Vector(4);
	  this->setNumDOF(4);
	}
      else
	{
	  theMatrix= Matrix(6,6);
	  theVector= Vector(6);
	  this->setNumDOF(6);
	}

      this->update();
    }
  }   	 


int XC::MultiFP2d::commitState()
  {
    Element0D::commitState(); // commit the base class.
    theFrictionModel->commitState(); // commit friction model.
    theVerticalModel->commitState(); // commit material model.

    cW= -theVerticalModel->getStress(); // commit trial history variable.

    return 0;
  }

int XC::MultiFP2d::revertToLastCommit()
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    theFrictionModel->revertToLastCommit();
    theVerticalModel->revertToLastCommit();
  
    return 0;
  }

int XC::MultiFP2d::revertToStart()
  {
    theFrictionModel->revertToStart();
    theVerticalModel->revertToStart();
    cW = W0;  

    return 0;
  }

int XC::MultiFP2d::update()
  {
    static Vector delU(4);
    static Vector delP(4);

    const Vector &v1= theNodes[0]->getTrialDisp();
    const Vector &v2= theNodes[1]->getTrialDisp();

    const double strainX = v2(0)-v1(0);
    const double strainY = v2(1)-v1(1);

    theFrictionModel->setTrialStrain(strainX);
    theVerticalModel->setTrialStrain(strainY);

    const int numD = numDOF/2;

    double k1 = theFrictionModel->getTangent();
    double k2 = theVerticalModel->getTangent();
  
    double f1 = theFrictionModel->getStress();
    double f2 = theVerticalModel->getStress();

    double vLoad = cW;
    if (axialCase == 0)
      vLoad = W0;
    else if (axialCase == 2)
      vLoad = f2;

    k1*= vLoad;
    f1*= vLoad;

    theVector.Zero();
  
    theVector(0) = -f1;
    theVector(1) = -f2;
    theVector(0+numD) = f1;
    theVector(1+numD) = f2;

    theMatrix.Zero();
  
    theMatrix(0,0)           = k1;
    theMatrix(0+numD,0+numD) = k1;
    theMatrix(0+numD,0)      = -k1;
    theMatrix(0,0+numD)      = -k1;

    theMatrix(1,1)           = k2;
    theMatrix(1+numD,1+numD) = k2;
    theMatrix(1+numD,1)      = -k2;
    theMatrix(1,1+numD)      = -k2;
    
    return 0;
  }


const XC::Matrix &XC::MultiFP2d::getTangentStiff(void) const
  {
    return theMatrix;
  }

const XC::Matrix &XC::MultiFP2d::getInitialStiff(void) const
  {
    const int numD = numDOF/2;

    const double k1 = theFrictionModel->getInitialTangent()*W0;
    const double k2 = theVerticalModel->getInitialTangent();
  
    theMatrix.Zero();
  
    theMatrix(0,0)           = k1;
    theMatrix(0+numD,0+numD) = k1;
    theMatrix(0+numD,0)      = -k1;
    theMatrix(0,0+numD)      = -k1;

    theMatrix(1,1)           = k2;
    theMatrix(1+numD,1+numD) = k2;
    theMatrix(1+numD,1)      = -k2;
    theMatrix(1,1+numD)      = -k2;

    return theMatrix;
  }

const XC::Vector &XC::MultiFP2d::getResistingForce() const
  {	
    return theVector;
  }

//! Add the given load to the element.
int XC::MultiFP2d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "load type unknown for element: "
	      << this->getTag() << "."
              << std::endl;
    
    return -1;
  }

//! @brief Add to the element the inertia load due to the given acceleration.
int XC::MultiFP2d::addInertiaLoadToUnbalance(const Vector &accel)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "not implemented yet."
              << std::endl;    
    return -1;
  }

//! @brief Send members through the communicator argument.
int XC::MultiFP2d::sendData(Communicator &comm)
  {
    int res= Element0D::sendData(comm);
    res+= comm.sendBrokedPtr(theFrictionModel, getDbTagData(), BrokedPtrCommMetaData(9, 10, 11));
    res+= comm.sendBrokedPtr(theVerticalModel, getDbTagData(), BrokedPtrCommMetaData(12, 13, 14));
    res+= comm.sendInts(type, axialCase, getDbTagData(), CommMetaData(15));
    res+= comm.sendDoubles(W0, cW, getDbTagData(), CommMetaData(16));
    res+= comm.sendMatrix(theMatrix, getDbTagData(),CommMetaData(17));
    res+= comm.sendVector(theVector, getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::MultiFP2d::recvData(const Communicator &comm)
  {
    int res= Element0D::recvData(comm);
    theFrictionModel= comm.getBrokedMaterial(theFrictionModel, getDbTagData(),BrokedPtrCommMetaData(9, 10, 11));
    theVerticalModel= comm.getBrokedMaterial(theVerticalModel, getDbTagData(),BrokedPtrCommMetaData(12, 13, 14));
    res+= comm.receiveInts(type, axialCase, getDbTagData(), CommMetaData(15));
    res+= comm.receiveDoubles(W0, cW, getDbTagData(), CommMetaData(16));
    res+= comm.receiveMatrix(theMatrix, getDbTagData(), CommMetaData(17));
    res+= comm.receiveVector(theVector, getDbTagData(), CommMetaData(18));
    return res;
  }

int XC::MultiFP2d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
		<< Color::def << std::endl;
    return res;
  }

int
XC::MultiFP2d::recvSelf(const Communicator &comm)
  {
    inicComm(19);
    
    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids."
		<< Color::def << std::endl;
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		     << "; failed to receive data."
		     << Color::def << std::endl;
      }
    return res;
  }

void XC::MultiFP2d::Print(std::ostream &s, int flag) const
  {
    s << "Element: " << this->getTag() 
      //<< " type: MultiFP2d  iNode: " << externalNodes(0)
      //<< " jNode: " << externalNodes(1) << std::endl
      << "material for normalized lateral force displacement response"
      << std::endl;
    this->theFrictionModel->Print(s, flag);
    s << "material for vertical force displacement response" << std::endl;
    this->theVerticalModel->Print(s, flag);
  }


XC::Response *XC::MultiFP2d::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    Response *theResponse = 0;

    /*
    output.tag("ElementOutput");
    output.attr("eleType",this->getClassType());
    output.attr("eleTag",this->getTag());
    int numNodes = this->getNumExternalNodes();
    const ID &nodes = this->getExternalNodes();
    static char nodeData[32];

    for (int i=0; i<numNodes; i++) {
      sprintf(nodeData,"node%d",i+1);
      output.attr(nodeData,nodes(i));
    }

    if (strcmp(argv[0],"force") == 0 || strcmp(argv[0],"forces") == 0 ||
	strcmp(argv[0],"globalForce") == 0 || strcmp(argv[0],"globalForces") == 0) {
      const Vector &force = this->getResistingForce();
      int size = force.Size();
      for (int i=0; i<size; i++) {
	sprintf(nodeData,"P%d",i+1);
	output.tag("ResponseType",nodeData);
      }
      theResponse = new ElementResponse(this, 1, this->getResistingForce());
    } else if (strcmp(argv[0],"friction") == 0 || strcmp(argv[0],"frictionModel") == 0) {
	theResponse =  theFrictionModel->setResponse(&argv[1], argc-1, output);
    } else if (strcmp(argv[0],"vertical") == 0 || strcmp(argv[0],"verticalModel") == 0) {
	theResponse =  theVerticalModel->setResponse(&argv[1], argc-1, output);
    } 

    output.endTag();
    */
    return theResponse;
  }



int  XC::MultiFP2d::getResponse(int responseID, Information &eleInfo)
  {
    // Vector res(this->getResistingForce());
    // res(2) = Ac;

    switch (responseID)
      {
      case -1:
	return -1;
      case 1: // global forces
	return eleInfo.setVector(this->getResistingForce());
	//  return eleInfo.setVector(res);
    
      case 2:
	return eleInfo.setVector(this->getRayleighDampingForces());
      default:
	return 0;
      }
  }

