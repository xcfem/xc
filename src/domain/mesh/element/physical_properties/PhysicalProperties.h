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
//PhysicalProperties.h

#ifndef PhysicalProperties_h
#define PhysicalProperties_h

#include "utility/actor/actor/MovableObject.h"
#include "material/MaterialVector.h"

namespace XC {

//! \ingroup Elem
//
//! @brief Elemento con material
template <class MAT>
class PhysicalProperties: public EntCmd, public MovableObject
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
    void setMaterial(const MAT *,const std::string &tipo);
    void setMaterial(size_t i,const MAT *nuevo_mat);

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

    inline MAT *operator[](const size_t &i)
      { return theMaterial[i]; }
    inline const MAT *operator[](const size_t &i) const
      { return theMaterial[i]; }

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    virtual void Print(std::ostream &s, int);
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
void PhysicalProperties<MAT>::setMaterial(const MAT *matModel, const std::string &tipo)
  { theMaterial.setMaterial(matModel,tipo); }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
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
      std::cerr << "PhysicalProperties::sendSelf -- failed to send ID data\n";
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
      std::cerr << "PhysicalProperties::recvSelf -- failed to receive ID data\n";
    else
      res+= this->recvData(cp);
    return res;
  }

//! @brief Sends object.
template <class MAT>
void PhysicalProperties<MAT>::Print(std::ostream &, int)
  {
    std::cerr << "PhysicalProperties::Print -- not implemented\n";
  }  

} //end of XC namespace
#endif
