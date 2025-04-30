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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/MembranePlateFiberSection.cpp,v $

// Ed "C++" Love
//
//  Generic Plate Section with membrane
//


#include "material/section/plate_section/MembranePlateFiberSectionBase.h"
#include <material/nD/NDMaterial.h>
#include "material/ResponseId.h"
#include "domain/mesh/element/utils/Information.h"

//static vector and matrices
XC::Vector XC::MembranePlateFiberSectionBase::stressResultant(XC::MembranePlateFiberSectionBase::order);
XC::Matrix XC::MembranePlateFiberSectionBase::tangent(XC::MembranePlateFiberSectionBase::order, XC::MembranePlateFiberSectionBase::order);

//! @brief Initializes material pointers.
void XC::MembranePlateFiberSectionBase::init(const size_t &sz)
  { theFibers= MaterialVector<NDMaterial>(sz); }

//! @brief Allocates material pointers.
void XC::MembranePlateFiberSectionBase::alloc(const size_t &sz, const NDMaterial &templ)
  {
    free();
    theFibers= MaterialVector<NDMaterial>(sz);
    for(size_t i= 0;i<sz;i++ )
      theFibers[i]= templ.getCopy("PlateFiber");
  }

//! @brief Allocates material pointers.
void XC::MembranePlateFiberSectionBase::alloc(const std::vector<NDMaterial *> &fibers)
  {
    free();
    const size_t sz= fibers.size();
    if(sz>0)
      {
	theFibers= MaterialVector<NDMaterial>(sz);
	for(size_t i= 0;i<sz;i++)
	  {
	    NDMaterial *tmp= fibers[i];
	    if(tmp)
	      theFibers[i]= tmp->getCopy("PlateFiber");
	  }
      }
  }

//! @brief Copy the material pointers.
void XC::MembranePlateFiberSectionBase::copy_fibers(const MembranePlateFiberSectionBase &other)
  
  {
    free();
    const size_t sz= other.theFibers.size();
    if(sz>0)
      {
	theFibers= MaterialVector<NDMaterial>(sz);
	for(size_t i= 0;i<sz;i++ )
	  {
	    const NDMaterial *ndMatPtr= other.theFibers[i];
	    if(ndMatPtr)
	      theFibers[i]= ndMatPtr->getCopy();
	  }
      }
  }

//! @brief Releases material pointers.
void XC::MembranePlateFiberSectionBase::free(void)
  { theFibers.clearMaterials(); }

//! @brief Default constructor.
XC::MembranePlateFiberSectionBase::MembranePlateFiberSectionBase(int tag, int classTag, const size_t &numFibers)
  : PlateBase(tag, classTag),
    theFibers(0), strainResultant(order), initialStrain(order)
  { init(numFibers); }

//! @brief full constructor
XC::MembranePlateFiberSectionBase::MembranePlateFiberSectionBase(int tag, int classTag, const double &thickness, const size_t &numFibers, const NDMaterial &Afiber)
  : PlateBase( tag, classTag, thickness, Afiber.getRho()),
    theFibers(0), strainResultant(order), initialStrain(order)
  {
    alloc(numFibers, Afiber);
  }

//! @brief full constructor
XC::MembranePlateFiberSectionBase::MembranePlateFiberSectionBase(int tag, int classTag, const double &thickness, const std::vector<NDMaterial *> &fibers)
  : PlateBase( tag, classTag, thickness, 0.0),
    theFibers(0), strainResultant(order), initialStrain(order)
  {
    alloc(fibers);
    if(!fibers.empty())
      {
	double avgRho= 0.0;
	for(std::vector<NDMaterial *>::const_iterator i= fibers.begin(); i!=fibers.end(); i++)
	  { avgRho+= (*i)->getRho(); }
	avgRho/= fibers.size();
	PlateBase::setRho(avgRho);
      }
  }

//! @brief Copy constructor.
XC::MembranePlateFiberSectionBase::MembranePlateFiberSectionBase(const MembranePlateFiberSectionBase &other)
  : PlateBase(other),
    theFibers(0), strainResultant(other.strainResultant), initialStrain(other.initialStrain)
  {
    copy_fibers(other);
  }

//! @brief Assignment operator.
XC::MembranePlateFiberSectionBase &XC::MembranePlateFiberSectionBase::operator=(const MembranePlateFiberSectionBase &other)
  {
    PlateBase::operator=(other);
    strainResultant= other.strainResultant;
    initialStrain= other.initialStrain;
    copy_fibers(other);
    return *this;
  }


//! @brief Destructor
XC::MembranePlateFiberSectionBase::~MembranePlateFiberSectionBase(void) 
  { free(); }

//! @brief Assign the same material to all the fibers.
void XC::MembranePlateFiberSectionBase::setMaterial(const NDMaterial &ndmat)
  { alloc(theFibers.size(), ndmat); }

//! @brief Create a fiber for each material on the list.
void XC::MembranePlateFiberSectionBase::setMaterial(const std::vector<NDMaterial *> &fibers)
  { alloc(fibers); }

//! @brief Create a fiber for each material on the list.
void XC::MembranePlateFiberSectionBase::setMaterial(const std::vector<std::string> &matNames)
  {
    const size_t sz= matNames.size();
    std::vector<NDMaterial *> mats(sz);
    for(size_t i= 0;i<sz;i++)
      {
	const std::string matName= matNames[i];
	const Material *tmp= getMaterialByName(matName);
	if(tmp)
	  {
	    const NDMaterial *ndMat= dynamic_cast<const NDMaterial *>(tmp);
	    mats[i]= const_cast<NDMaterial *>(ndMat);
	    if(!ndMat)
  	      std::cerr << getClassName() << "::" << __FUNCTION__
	                << "; material " << matName
		        << " of type: " << tmp->getClassName()
			<< " is not valid." << std::endl;
	  }
	else
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; material: " << matName << " not found." << std::endl;
	    exit(-1);
	  }
      }
    this->setMaterial(mats);
  }

//! @brief Create a fiber for each material on the list.
//! @param matNames: names of the materials for the fibers.
void XC::MembranePlateFiberSectionBase::setMaterialPy(const boost::python::list &matNames)
  {
    const size_t sz= len(matNames);
    std::vector<std::string> mat_names(sz);
    for(size_t i= 0;i<sz;i++)
      {
	mat_names[i]= boost::python::extract<std::string>(matNames[i]);
      }
    this->setMaterial(mat_names);
  }

//! @brief send back order of strainResultant in vector form
int XC::MembranePlateFiberSectionBase::getOrder(void) const
  { return order; }


//! @brief Returns the labels of the DOFs for which the element
//! adds (assembles) stiffness.
const XC::ResponseId &XC::MembranePlateFiberSectionBase::getResponseType(void) const 
  { return RespShellMat; }



//! @brief Swap history variables.
int XC::MembranePlateFiberSectionBase::commitState(void) 
  { return theFibers.commitState(); }

//! @brief Revert to last committed state.
int XC::MembranePlateFiberSectionBase::revertToLastCommit(void)
  { return theFibers.revertToLastCommit(); }

//! @brief Revert to start.
int XC::MembranePlateFiberSectionBase::revertToStart(void)
  { return theFibers.revertToStart(); }


//! @brief Return initial deformation.
const XC::Vector &XC::MembranePlateFiberSectionBase::getInitialSectionDeformation(void) const
  {
    return initialStrain;
  }

//! @brief Set initial values for deformation.
int XC::MembranePlateFiberSectionBase::setInitialSectionDeformation(const Vector &initialStrain_from_element)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return 0;
  }

//! @brief Zero initial deformation.
void XC::MembranePlateFiberSectionBase::zeroInitialSectionDeformation(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet." << std::endl;
  }

//! @brief Returns section deformation.
const XC::Vector &XC::MembranePlateFiberSectionBase::getSectionDeformation(void) const
  {
    static Vector retval;
    retval= strainResultant-initialStrain;
    return retval;
  }

//! @brief Return the Von Mises stress at each fiber.
XC::Vector XC::MembranePlateFiberSectionBase::getVonMisesStressAtFibers(void) const
  {
    const size_t sz= theFibers.size();
    Vector retval(sz);
    for(size_t i= 0;i<sz; i++)
      retval[i]= theFibers[i]->getVonMisesStress();
    return retval;
  }

//! @brief Return the minimum Von Mises stress at fibers.
double XC::MembranePlateFiberSectionBase::getMinVonMisesStress(void) const
  {
    const Vector tmp= getVonMisesStressAtFibers();
    double retval= tmp[0];
    const size_t sz= theFibers.size();
    for(size_t i= 1;i<sz; i++)
      retval= std::min(retval, tmp[i]);
    return retval;
  }

//! @brief Return the maximum Von Mises stress at fibers.
double XC::MembranePlateFiberSectionBase::getMaxVonMisesStress(void) const
  {
    const Vector tmp= getVonMisesStressAtFibers();
    double retval= tmp[0];
    const size_t sz= theFibers.size();
    for(size_t i= 1;i<sz; i++)
      retval= std::max(retval, tmp[i]);
    return retval;
  }
  
//! @brief Return the maximum Von Mises stress at fibers.
double XC::MembranePlateFiberSectionBase::getAvgVonMisesStress(void) const
  {
    const Vector tmp= getVonMisesStressAtFibers();
    double retval= 0.0;
    const size_t sz= theFibers.size();
    for(size_t i= 0;i<sz; i++)
      retval+= tmp[i];
    retval/=sz;
    return retval;
  }

//! @brief Return values of internal forces, deformations...
//! @param cod: name of the requested value.
//! @param silent: if true don't complain about non-existen property.
XC::Matrix XC::MembranePlateFiberSectionBase::getValues(const std::string &cod, bool silent) const
  {
    Matrix retval;
    if(cod == "max_von_mises_stress")
      {
	retval.resize(1,1);
	retval(0,0)= getMaxVonMisesStress();
      }
    else if(cod == "min_von_mises_stress")
      {
	retval.resize(1,1);
	retval(0,0)= getMinVonMisesStress();
      }
    else if((cod == "avg_von_mises_stress") || (cod == "mean_von_mises_stress"))
      {
	retval.resize(1,1);
	retval(0,0)= getAvgVonMisesStress();
      }
    else if((cod == "von_mises_stresses") || (cod == "Von_Mises_stresses"))
      {
	retval.resize(theFibers.size(),1);
	const Vector vm= getVonMisesStressAtFibers();
	retval.putCol(0,vm);
      }
    else
      retval= PlateBase::getValues(cod, silent);
    return retval;
  }

//! @brief Print out data
void  XC::MembranePlateFiberSectionBase::Print( std::ostream &s, int flag ) const
  {
    s << "MembranePlateFiberSectionBase: \n ";
    s <<  "  Thickness h = "        <<  h  <<  std::endl;
    const size_t sz= theFibers.size();
    for(size_t i = 0; i < sz; i++)
      { theFibers[i]->Print( s, flag ) ; }
  }

//! @brief Send object members through the communicator argument.
int XC::MembranePlateFiberSectionBase::sendData(Communicator &comm)
  {
    int res= PlateBase::sendData(comm);
    res+= comm.sendMovable(theFibers,getDbTagData(),CommMetaData(7));
    res+= comm.sendVector(strainResultant,getDbTagData(),CommMetaData(8));
    res+= comm.sendVector(initialStrain,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::MembranePlateFiberSectionBase::recvData(const Communicator &comm)
  {
    int res= PlateBase::recvData(comm);
    res+= comm.receiveMovable(theFibers,getDbTagData(),CommMetaData(7));
    res+= comm.receiveVector(strainResultant,getDbTagData(),CommMetaData(8));
    res+= comm.receiveVector(initialStrain,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Send object itself through the communicator argument.
int XC::MembranePlateFiberSectionBase::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(10);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";
    return res;
  }


//! @brief Receive object itself through the communicator argument.
int XC::MembranePlateFiberSectionBase::recvSelf(const Communicator &comm)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }
 
XC::Response *XC::MembranePlateFiberSectionBase::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    Response *theResponse= nullptr;
    const int argc= argv.size();
    if(argc > 2 && (argv[0]=="fiber"))
      {
        const int passarg = 2;
        const size_t key = atoi(argv[1]);    
        const size_t sz= theFibers.size();
	if(key > 0 && key <= sz)
	  {
	    //info.tag("FiberOutput");
	    //info.attr("number", key);
	    //const double *sg= (integrationType == 0) ? sgLobatto : sgGauss;
	    //const double *wg= (integrationType == 0) ? wgLobatto : wgGauss;      
	    //info.attr("zLoc", 0.5 * h * sg[key - 1]);
	    //info.attr("thickness", 0.5 * h * wg[key - 1]);
            // Slice arguments.
	    const std::vector<std::string> argv2= std::vector<std::string>(argv.begin()+passarg, argv.end());
	    theResponse= theFibers[key-1]->setResponse(argv2, info);
	    //info.endTag();
	  }
      }
    if(!theResponse)
      theResponse= PlateBase::setResponse(argv, info);
    return theResponse;
  }

int XC::MembranePlateFiberSectionBase::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // if the user explicitly wants to update a material in this section...
    const int argc= argv.size();
    if(argc > 1)
      {
        // case 1: fiber value (all fibers)
        // case 2: fiber id value (one specific fiber)
        if((argv[0]=="fiber") || (argv[0]=="Fiber"))
	  {
            // test case 2 (one specific fiber) ...
            if(argc > 2)
	      {
                const size_t pointNum = atoi(argv[1]);
		const size_t sz= theFibers.size();
                if(pointNum > 0 && pointNum <= sz)
		  {
		    // Slice arguments.
		    const std::vector<std::string> argv2= std::vector<std::string>(argv.begin()+2, argv.end());
                    return theFibers[pointNum - 1]->setParameter(argv2, param);
                  }
              }
            // ... otherwise case 1 (all fibers), if the argv[1] is not a valid id
            int mixed_result = -1;
            const size_t sz= theFibers.size();
            for(size_t i = 0; i < sz; ++i)
	      {
		// Slice arguments.
		const std::vector<std::string> argv1= std::vector<std::string>(argv.begin()+1, argv.end());
                if(theFibers[i]->setParameter(argv1, param) == 0)
                    mixed_result = 0; // if at least one fiber handles the param, make it successful
              }
            return mixed_result;
	  }
      }
    // if we are here, the first keyword is not "fiber", so we can check for parameters
    // specific to this section (if any) or forward the request to all fibers.
    if(argc > 0)
      {
        // we don't have parameters for this section, so we directly forward it to all fibers.
        // placeholder for future implementations: if we will have parameters for this class, check them here
        // before forwarding to all fibers
        int mixed_result = -1;
        const size_t sz= theFibers.size();
        for(size_t i = 0; i < sz; ++i)
	  {
            if (theFibers[i]->setParameter(argv, param) == 0)
                mixed_result = 0; // if at least one fiber handles the param, make it successful
          }
        return mixed_result;
      }
    // fallback to base class implementation
    return PlateBase::setParameter(argv, param);
  }


