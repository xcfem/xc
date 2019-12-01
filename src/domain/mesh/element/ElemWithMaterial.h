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

#include <domain/mesh/element/ElementBase.h>

namespace XC {

//! @ingroup Elem
//
//! @brief Element with material
template <int NNODOS,class PhysProp>
class ElemWithMaterial : public ElementBase<NNODOS>
  {
  protected:
    PhysProp physicalProperties; //!< pointers to the material objects and physical properties.

    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    ElemWithMaterial(int tag, int classTag);
    ElemWithMaterial(int tag, int classTag,const PhysProp &);

    // public methods to set the state of the element
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    virtual void zeroInitialGeneralizedStrains(void);	
    
    inline PhysProp &getPhysicalProperties(void)
      { return physicalProperties; }
    inline const PhysProp &getPhysicalProperties(void) const
      { return physicalProperties; }
    void setPhysicalProperties(const PhysProp &);
    inline virtual std::set<std::string> getMaterialNames(void) const
      { return physicalProperties.getMaterialNames(); }
    
    virtual const Matrix &getExtrapolationMatrix(void) const;
    Matrix getExtrapolatedValues(const Matrix &) const;
  };

template <int NNODOS,class PhysProp>
ElemWithMaterial<NNODOS,PhysProp>::ElemWithMaterial(int tag, int classTag)
  : ElementBase<NNODOS>(tag,classTag) {}

template <int NNODOS,class PhysProp>
ElemWithMaterial<NNODOS,PhysProp>::ElemWithMaterial(int tag, int classTag,const PhysProp &physProp)
  : ElementBase<NNODOS>(tag,classTag), physicalProperties(physProp) {}

template <int NNODOS,class PhysProp>
int ElemWithMaterial<NNODOS, PhysProp>::commitState(void)
  {
    int retVal = 0;

    if((retVal= ElementBase<NNODOS>::commitState()) != 0)
      {
        std::cerr << "ElemWithMaterial::commitState () - failed in base class";
        return (-1);
      }
    retVal+= physicalProperties.commitState();
    return retVal;
  }

template <int NNODOS,class PhysProp>
int ElemWithMaterial<NNODOS, PhysProp>::revertToLastCommit(void)
  {
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


//! @brief Send members through the channel being passed as parameter.
template <int NNODOS,class PhysProp>
  int ElemWithMaterial<NNODOS, PhysProp>::sendData(CommParameters &cp)
  {
    int res= ElementBase<NNODOS>::sendData(cp);
    res+= cp.sendMovable(physicalProperties,this->getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
template <int NNODOS,class PhysProp>
int ElemWithMaterial<NNODOS, PhysProp>::recvData(const CommParameters &cp)
  {
    int res= ElementBase<NNODOS>::recvData(cp);
    res+= cp.receiveMovable(physicalProperties,this->getDbTagData(),CommMetaData(7));
    return res;
  }

} //end of XC namespace
#endif
