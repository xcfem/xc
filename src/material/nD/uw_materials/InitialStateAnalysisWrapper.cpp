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
                                                                        
// Written: Chris McGann
//          February 2011
                                                                      
// Description: This file contains the implementation for the InitialStateAnalysisWrapper class.
//              This wrapper can be used with any nDMaterial, and enables the use of the 
//              InitialStateAnalysis command for setting initial conditions.

#include "InitialStateAnalysisWrapper.h"

#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <domain/component/Parameter.h>

bool XC::InitialStateAnalysisWrapper::initialStateAnalysis= false;

XC::InitialStateAnalysisWrapper::InitialStateAnalysisWrapper(int tag)
  : NDMaterial(tag, ND_TAG_InitialStateAnalysisWrapper),
    theMainMaterial(), mDIM(0), mEpsilon_o(3), mStrain(3)
  {
    mEpsilon_o.Zero();
    mStrain.Zero();
  }

// full constructor
XC::InitialStateAnalysisWrapper::InitialStateAnalysisWrapper(int tag, NDMaterial &mainMat, int ndim)
  : NDMaterial(tag, ND_TAG_InitialStateAnalysisWrapper),
    theMainMaterial(), mDIM(ndim), mEpsilon_o(3*ndim-3), mStrain(3*ndim-3)
  {
    setup(ndim, mainMat);
  }

void XC::InitialStateAnalysisWrapper::setup(int ndim, const NDMaterial &mainMat)
  {
    mDIM= ndim;
    mEpsilon_o.resize(3*ndim-3);
    mEpsilon_o.Zero();
    mStrain.resize(3*ndim-3);
    mStrain.Zero();
  
    // get copy of the main material
    if(mDIM == 2)
      {
	const NDMaterial *tmp= mainMat.getCopy("PlaneStrain");
	this->setMaterial(*tmp);
	delete tmp;
      }
    else if(mDIM == 3)
      {
	const NDMaterial *tmp= mainMat.getCopy("ThreeDimensional");
	this->setMaterial(*tmp);
	delete tmp;
      }
    else
      {
	std::cerr << "Incompatible number of dimensions for InitialStateAnalysisWrapper - want 2 or 3" << std::endl;
      }
  }

// clone material
XC::NDMaterial *XC::InitialStateAnalysisWrapper::getCopy(const std::string &) const
  { return this->getCopy(); }

XC::NDMaterial *XC::InitialStateAnalysisWrapper::getCopy(void) const
  { return new InitialStateAnalysisWrapper(*this); }


const std::string &XC::InitialStateAnalysisWrapper::getType(void) const
  { return theMainMaterial.getType(); }

bool XC::InitialStateAnalysisWrapper::getInitialStateAnalysisPhase(void)
  { return initialStateAnalysis; }
void XC::InitialStateAnalysisWrapper::setInitialStateAnalysisPhase(const bool &b)
  { initialStateAnalysis= b; }

int XC::InitialStateAnalysisWrapper::getOrder(void) const
  { return theMainMaterial.getOrder(); }

//! @brief Return a pointer to the wrapped material.
const XC::NDMaterial *XC::InitialStateAnalysisWrapper::getMaterial(void) const
  { return theMainMaterial.getMaterial(); }

//! @brief Return a pointer to the wrapped material.
XC::NDMaterial *XC::InitialStateAnalysisWrapper::getMaterial(void)
  { return theMainMaterial.getMaterial(); }

//! @brief Sets the encapsulated material.
void XC::InitialStateAnalysisWrapper::setMaterial(const NDMaterial &material)
  { theMainMaterial.setMaterial(material); }

//! @brief Sets the encapsulated material.
void XC::InitialStateAnalysisWrapper::setMaterial(const std::string &matName)
  {
    const Material *ptr_mat= getMaterialByName(matName);
    if(ptr_mat)
      {
	const NDMaterial *tmp= dynamic_cast<const NDMaterial *>(ptr_mat);
	if(tmp)
	  theMainMaterial.setMaterial(*tmp);
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
		    << "material identified by: '" << matName
		    << "' is not an uniaxial material." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
		<< "material identified by: '" << matName
		<< "' not found." << std::endl;
  }

int XC::InitialStateAnalysisWrapper::commitState(void)
  {
    return theMainMaterial.commitState();
  }

//! @brief Return the initial strain.
const XC::Vector &XC::InitialStateAnalysisWrapper::getInitialGeneralizedStrain(void) const
  { return mEpsilon_o; }

void XC::InitialStateAnalysisWrapper::setInitialGeneralizedStrain(const Vector &eps)
  { mEpsilon_o= eps; }

int XC::InitialStateAnalysisWrapper::revertToLastCommit(void)
  { return theMainMaterial.revertToLastCommit(); }

int XC::InitialStateAnalysisWrapper::revertToStart(void)
  {
    // update epsilon_o when InitialStateAnalysis off is called
    if(initialStateAnalysis)
      { mEpsilon_o += mStrain; }
    return theMainMaterial.revertToStart();
  }

int XC::InitialStateAnalysisWrapper::setTrialStrain(const Vector &strain_from_element)
// this function receives the strain from the element and sends strain to material
  {
    // add epsilon_o to the element strain
    mStrain = strain_from_element + mEpsilon_o;
	
    // send the sum to the main material
    theMainMaterial.setTrialStrain(mStrain);

    return 0;
  }

double XC::InitialStateAnalysisWrapper::getRho(void) const
// this function gets the mass density from the main material
  {
    return theMainMaterial.getRho();
  }

const XC::Vector &XC::InitialStateAnalysisWrapper::getStrain(void) const
// this function sends the strain back to the element
  { return theMainMaterial.getStrain(); }

const XC::Vector &XC::InitialStateAnalysisWrapper::getStress(void) const
// this function sends the stress back to the element
  {
    return theMainMaterial.getStress();
  }

//! brief this function sends the tangent back to the element
const XC::Matrix &XC::InitialStateAnalysisWrapper::getTangent(void) const
  { return theMainMaterial.getTangent(); }

const XC::Matrix &XC::InitialStateAnalysisWrapper::getInitialTangent(void) const
// this function sends the initial tangent back to the element
  { return theMainMaterial.getInitialTangent(); }

int XC::InitialStateAnalysisWrapper::getMainClassTag(void) const
// this function sends the class tag of the main material
  { return theMainMaterial.getMainClassTag(); }

//! @brief Send material.
int XC::InitialStateAnalysisWrapper::sendData(Communicator &comm)
  {
    int res= NDMaterial::sendData(comm);
    res+= comm.sendMovable(theMainMaterial, getDbTagData(), CommMetaData(1));
    res+= comm.sendInt(mDIM, getDbTagData(), CommMetaData(2));
    res+= comm.sendVector(mEpsilon_o, getDbTagData(), CommMetaData(3));
    res+= comm.sendVector(mStrain, getDbTagData(), CommMetaData(4));
    return res;
  }

//! @brief Receive material.
int XC::InitialStateAnalysisWrapper::recvData(const Communicator &comm)
  {
    int res= NDMaterial::recvData(comm);
    res+= comm.receiveMovable(theMainMaterial, getDbTagData(),CommMetaData(1));
    res+= comm.receiveInt(mDIM, getDbTagData(), CommMetaData(2));
    res+= comm.receiveVector(mEpsilon_o, getDbTagData(), CommMetaData(3));
    res+= comm.receiveVector(mStrain, getDbTagData(), CommMetaData(4));
    return res;
  }

int XC::InitialStateAnalysisWrapper::sendSelf(Communicator &comm)
  {
    inicComm(5);
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data.\n";    
    return res;
  }

int XC::InitialStateAnalysisWrapper::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ";  data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;
  }

void XC::InitialStateAnalysisWrapper::Print(std::ostream &s, int flag) const
  {
    s << "InitialStateAnalysisWrapper Material Tag: " << this-getTag() << std::endl;
    s << "wrapping the material: \n";
    theMainMaterial.Print(s, flag);
  }

int
XC::InitialStateAnalysisWrapper::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // this allows for the use of updateMaterialStage command for the main material
    int retval= -1;
    const size_t argc= argv.size();
    if(argv[0]=="updateMaterialStage")
      {
	if (argc < 2)
	  { return -1;}
	const int matTag = atoi(argv[1]);
	if (this->getTag() == matTag)
	  { retval= param.addObject(1,this); }
	else
	  { return -1; }
      }
    else if(argv[0]=="shearModulus")
      {
	if (argc < 2)
	  { return -1; }
	const int matTag = atoi(argv[1]);
	if (this->getTag() == matTag)
	  { retval= param.addObject(10,this); }
	else
	  { return -1; }
      }
    else if (argv[0] == "bulkModulus")
      {
	if (argc < 2)
	  { return -1; }
	const int matTag = atoi(argv[1]);
	if (this->getTag() == matTag)
	  { retval= param.addObject(11,this); }
	else
	  { return -1; }
      }
    else if (argv[0] == "frictionAngle")
      {
	if (argc < 2)
	  { return -1; }
	const int matTag = atoi(argv[1]);
	if (this->getTag() == matTag)
	  { retval= param.addObject(12,this); }
	else
	  { return -1; }
      }
    else if (argv[0] == "cohesion")
      {
	if (argc < 2)
	  { return -1; }
	const int matTag = atoi(argv[1]);
	if (this->getTag() == matTag)
	  { retval= param.addObject(12,this); }
	else
	  { return -1; }
      }
    return retval;
  }

int XC::InitialStateAnalysisWrapper::updateParameter(int responseID, Information &info)
  {
    // routes the updateParameter call to the main material
    theMainMaterial.updateParameter(responseID, info);	
    return 0;
  }

XC::Response *XC::InitialStateAnalysisWrapper::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    return theMainMaterial.setResponse(argv, info);
  }

int XC::InitialStateAnalysisWrapper::getResponse(int responseID, Information &matInfo)
  {
    return theMainMaterial.getResponse(responseID, matInfo);
  }
