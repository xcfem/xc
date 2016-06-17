//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//ElemWithMaterials.h

#ifndef ElemWithMaterial_h
#define ElemWithMaterial_h

#include <domain/mesh/element/ElementBase.h>

namespace XC {

//! \ingroup Elem
//
//! @brief Elemento con material
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
