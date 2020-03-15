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
//PhysicalProperties.h

#ifndef PhysicalProperties_h
#define PhysicalProperties_h

#include "utility/actor/actor/MovableObject.h"
#include "material/MaterialVector.h"

namespace XC {

//! @ingroup FEMisc
//!
//! @defgroup PhysicalProperties Classes for dealing with element's physical properties (material,...).
//
//! @ingroup PhysProp
//
//! @brief Base class for element's physical properties.
template <class MAT>
class PhysicalProperties: public CommandEntity, public MovableObject
  {
  public:
    typedef MaterialVector<MAT> material_vector;
  protected:
    material_vector theMaterial; //!< pointers to the material objects

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    PhysicalProperties(const size_t &nMat= 0,const MAT *matModel= nullptr);
    void setMaterial(const MAT *);
    void setMaterial(const MAT *,const std::string &);
    void setMaterial(size_t i,const MAT *);

    // public methods to set the state of the properties
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    inline size_t size(void) const
      { return theMaterial.size(); } 
    inline material_vector &getMaterialsVector(void)
      { return theMaterial; }
    inline const material_vector &getMaterialsVector(void) const
      { return theMaterial; }
    inline std::set<std::string> getMaterialNames(void) const
      { return theMaterial.getNames(); }
    inline boost::python::list getMaterialNamesPy(void) const
      { return theMaterial.getNamesPy(); }

    inline MAT *operator[](const size_t &i)
      { return theMaterial[i]; }
    inline const MAT *operator[](const size_t &i) const
      { return theMaterial[i]; }

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    //! @brief Return the generalized stresses at material points.
    inline Matrix getGeneralizedStresses(void) const
      { return theMaterial.getGeneralizedStresses(); }
    //! @brief Return the generalized strains at material points.
    inline Matrix getGeneralizedStrains(void) const
      { return theMaterial.getGeneralizedStrains(); }

    virtual void Print(std::ostream &s, int) const;
  };

 template <class MAT>
PhysicalProperties<MAT>::PhysicalProperties(const size_t &nMat,const MAT *matModel)
  : MovableObject(0), theMaterial(nMat, matModel) {}

template <class MAT>
int PhysicalProperties<MAT>::commitState(void)
  { return theMaterial.commitState(); }

template <class MAT>
int PhysicalProperties<MAT>::revertToLastCommit(void)
  { return theMaterial.revertToLastCommit(); }

template <class MAT>
int PhysicalProperties<MAT>::revertToStart(void)
  { return theMaterial.revertToStart(); }

template <class MAT>
void PhysicalProperties<MAT>::setMaterial(const MAT *matModel)
  { theMaterial.setMaterial(matModel); }

template <class MAT>
void PhysicalProperties<MAT>::setMaterial(const MAT *matModel, const std::string &type)
  { theMaterial.setMaterial(matModel,type); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
template <class MAT>
DbTagData &PhysicalProperties<MAT>::getDbTagData(void) const
  {
    static DbTagData retval(2); 
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
template <class MAT>
int PhysicalProperties<MAT>::sendData(CommParameters &cp)
  {
    int res= cp.sendMovable(theMaterial,this->getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
template <class MAT>
int PhysicalProperties<MAT>::recvData(const CommParameters &cp)
  {
    int res= cp.receiveMovable(theMaterial,this->getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Sends object.
template <class MAT>
int PhysicalProperties<MAT>::sendSelf(CommParameters &cp)
  {
    inicComm(2);

    int res= this->sendData(cp);

    const int dataTag= getDbTag();
    res += cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send ID data\n";
    return res;
  }

//! @brief Receives object
template <class MAT>
int PhysicalProperties<MAT>::recvSelf(const CommParameters &cp)
  {
    inicComm(2);

    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data\n";
    else
      res+= this->recvData(cp);
    return res;
  }

//! @brief Sends object.
template <class MAT>
void PhysicalProperties<MAT>::Print(std::ostream &, int) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented.\n";
  }  
 
} //end of XC namespace
#endif
