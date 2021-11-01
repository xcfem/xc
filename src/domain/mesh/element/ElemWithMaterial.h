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
//ElemWithMaterials.h

#ifndef ElemWithMaterial_h
#define ElemWithMaterial_h

#include "ElementBase.h"
#include "domain/mesh/element/utils/Information.h"
#include "utility/recorder/response/ElementResponse.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"

namespace XC {

//! @ingroup Elem
//
//! @brief Element with material
template <int NNODOS, class PhysProp>
class ElemWithMaterial: public ElementBase<NNODOS>
  {
  public:
    typedef typename PhysProp::material_type material_type;
  protected:
    PhysProp physicalProperties; //!< pointers to the material objects and physical properties.

    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    ElemWithMaterial(int tag, int classTag);
    ElemWithMaterial(int tag, int classTag,const PhysProp &);

    // public methods to set the state of the element
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    virtual void zeroInitialGeneralizedStrains(void);
    
    void setMaterial(const std::string &);
    inline PhysProp &getPhysicalProperties(void)
      { return physicalProperties; }
    inline const PhysProp &getPhysicalProperties(void) const
      { return physicalProperties; }
    void setPhysicalProperties(const PhysProp &);
    inline virtual std::set<std::string> getMaterialNames(void) const
      { return physicalProperties.getMaterialNames(); }
    
    virtual const Matrix &getExtrapolationMatrix(void) const;
    Matrix getExtrapolatedValues(const Matrix &) const;
    
    int getResponse(int responseID, Information &eleInformation);
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    boost::python::list getValuesAtNodes(const std::string &, bool silent= false) const;
  };

template <int NNODOS,class PhysProp>
ElemWithMaterial<NNODOS,PhysProp>::ElemWithMaterial(int tag, int classTag)
  : ElementBase<NNODOS>(tag,classTag) {}

template <int NNODOS,class PhysProp>
ElemWithMaterial<NNODOS,PhysProp>::ElemWithMaterial(int tag, int classTag,const PhysProp &physProp)
  : ElementBase<NNODOS>(tag,classTag), physicalProperties(physProp) {}

//! @brief Set the element material.
template <int NNODOS,class PhysProp>
void ElemWithMaterial<NNODOS, PhysProp>::setMaterial(const std::string &matName)
  {
    const Material *ptr_mat= this->get_material_ptr(matName);
    if(ptr_mat)
      {
	const material_type *tmp= dynamic_cast<const material_type *>(ptr_mat);
	if(tmp)
	  physicalProperties.setMaterial(tmp);
	else
	  std::cerr << this->getClassName() << "::" << __FUNCTION__ << "; "
		    << "material identified by: '" << matName
		    << "' is not a suitable material.\n";
      }
  }
  
template <int NNODOS,class PhysProp>
int ElemWithMaterial<NNODOS, PhysProp>::commitState(void)
  {
    int retVal= 0;
    if((retVal= ElementBase<NNODOS>::commitState()) != 0)
      {
        std::cerr << this->getClassName() << "::" << __FUNCTION__
	          << "; failed in base class." << std::endl;
        return (-1);
      }
    retVal+= physicalProperties.commitState();
    return retVal;
  }

template <int NNODOS,class PhysProp>
int ElemWithMaterial<NNODOS, PhysProp>::revertToLastCommit(void)
  {
    // DON'T call Element::revertToLastCommit() because
    // it's a pure virtual method.
    int retval= physicalProperties.revertToLastCommit();
    return retval;
  }

template <int NNODOS,class PhysProp>
int ElemWithMaterial<NNODOS, PhysProp>::revertToStart(void)
  {
    int retval= ElementBase<NNODOS>::revertToStart();
    retval+= physicalProperties.revertToStart();
    return retval;
  }

template <int NNODOS,class PhysProp>
void ElemWithMaterial<NNODOS, PhysProp>::zeroInitialGeneralizedStrains(void)
  {
    physicalProperties.getMaterialsVector().zeroInitialGeneralizedStrains();
  }

template <int NNODOS,class PhysProp>
void ElemWithMaterial<NNODOS, PhysProp>::setPhysicalProperties(const PhysProp &physProp)
  { physicalProperties= physProp; }

//! @brief Return the matrix that extrapolates results at
//! material points to results at nodes
template <int NNODOS,class PhysProp>
const Matrix &ElemWithMaterial<NNODOS, PhysProp>::getExtrapolationMatrix(void) const
  {
    std::cerr << this->getClassName() << "::" << __FUNCTION__
              << "; must be overloaded in derived classes."
              << std::endl;
    static const Matrix retval;
    return retval;
  }

//! @brief Extrapolate from Gauss points to nodes.
template <int NNODOS,class PhysProp>
Matrix ElemWithMaterial<NNODOS, PhysProp>::getExtrapolatedValues(const Matrix &values) const
  { return getExtrapolationMatrix()*values; }
  
//! @brief Return a python list with the values of the argument property
//! at element nodes.
//!
//! When the property requested its located at the integration point this
//! function is responsible of the extrapolation of values from
//! Gauss points to nodes.
//! @tparam NNODOS: number of nodes.
//! @tparam PhysProp: type of material container (physical properties).
//! @param code: identifier of the requested value.
//! @param silent: if true don't complaint about non-existen property.
template <int NNODOS,class PhysProp>
boost::python::list ElemWithMaterial<NNODOS, PhysProp>::getValuesAtNodes(const std::string &code, bool silent) const
  {
    boost::python::list retval;
    const Matrix matValues= physicalProperties.getMaterialsVector().getValues(code, silent);
    if(matValues.noRows()>0)
      {
        const Matrix nodeValues= getExtrapolatedValues(matValues);
    	const size_t nRows= nodeValues.noRows();
    	for(size_t i= 0;i<nRows;i++)
    	  {
    	    Vector valueAtNode= nodeValues.getRow(i);
    	    retval.append(valueAtNode);
    	  }
      }
    else
      retval= ElementBase<NNODOS>::getValuesAtNodes(code, silent); 
    return retval;
  }
  
//! @brief Send members through the communicator argument.
template <int NNODOS,class PhysProp>
int ElemWithMaterial<NNODOS, PhysProp>::sendData(Communicator &comm)
  {
    int res= ElementBase<NNODOS>::sendData(comm);
    res+= comm.sendMovable(physicalProperties,this->getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives members through the communicator argument.
template <int NNODOS,class PhysProp>
int ElemWithMaterial<NNODOS, PhysProp>::recvData(const Communicator &comm)
  {
    int res= ElementBase<NNODOS>::recvData(comm);
    res+= comm.receiveMovable(physicalProperties,this->getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Obtain information from an analysis.
template <int NNODOS,class PhysProp>
int ElemWithMaterial<NNODOS, PhysProp>::getResponse(int responseID, Information &eleInfo)
  {
    int retval= -1;
    if(responseID == 1)
      { retval= eleInfo.setVector(this->getResistingForce()); }
    else if(responseID == 2)
      { retval= eleInfo.setMatrix(this->getTangentStiff()); }
    else if(responseID == 3)
      { retval= physicalProperties.getResponse(responseID, eleInfo);  }
    else if(responseID == 4)
      { retval= physicalProperties.getResponse(responseID, eleInfo);  }
    return retval;
  }

//! @brief element response.
template <int NNODOS,class PhysProp>
XC::Response *ElemWithMaterial<NNODOS, PhysProp>::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    Response *retval= nullptr;
    if(argv[0] == "force" || argv[0] == "forces")
      retval= new ElementResponse(this, 1, this->getResistingForce());
    else if(argv[0] == "stiff" || argv[0] == "stiffness")
      retval= new ElementResponse(this, 2, this->getTangentStiff());
    else if(argv[0] == "material"  || (argv[0]=="Material") || argv[0] == "integrPoint")
      {
        size_t pointNum = atoi(argv[1]);
        if(pointNum > 0 && pointNum <= this->physicalProperties.size())
          retval= this->setMaterialResponse(this->physicalProperties[pointNum-1],argv,2,eleInfo);
        else
          retval= nullptr;
      }
    else if(argv[0] == "stress" || argv[0] == "stresses")
      { retval= new ElementResponse(this, 3, this->getResistingForce()); }
    else if(argv[0] == "strain" || argv[0] == "strains")
      { retval= new ElementResponse(this, 3, this->getResistingForce()); }
    else // otherwise response quantity is unknown for the quad class
      retval= nullptr;
    return retval;
  }

} //end of XC namespace
#endif
