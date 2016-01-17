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
//ZeroLengthMaterials.cpp

#include "ZeroLengthMaterials.h"
#include "domain/mesh/element/zeroLength/ZeroLength.h"
#include "material/uniaxial/UniaxialMaterial.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/loaders/MaterialLoader.h"
#include "domain/mesh/node/Node.h"


//!  Constructor:
XC::ZeroLengthMaterials::ZeroLengthMaterials(ZeroLength *owner,UniaxialMaterial &theMat, int direction)
  :DqUniaxialMaterial(owner,theMat,1), direcciones()
  {
    direcciones.push_back(direction);
    checkDirection();
  }

//!  Constructor:
XC::ZeroLengthMaterials::ZeroLengthMaterials(ZeroLength *owner,UniaxialMaterial *theMat, int direction)
  :DqUniaxialMaterial(owner,theMat,1), direcciones()
  {
    direcciones.push_back(direction);
    checkDirection();
  }

//! @brief  Construct container with multiple unidirectional materials
XC::ZeroLengthMaterials::ZeroLengthMaterials(ZeroLength *owner,const DqUniaxialMaterial &theMat,const ID &direction)
  :DqUniaxialMaterial(theMat), direcciones(theMat.size())
  {
    set_owner(owner);
    // initialize uniaxial materials and directions and check for valid values
    for(size_t i= 0;i<theMat.size();i++)
      direcciones[i]= direction(i);
    checkDirection();
  }

//   Constructor:
//   invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//   to be invoked upon
XC::ZeroLengthMaterials::ZeroLengthMaterials(ZeroLength *owner)
  :DqUniaxialMaterial(owner), direcciones() {}


void XC::ZeroLengthMaterials::clear(void)
  {
    DqUniaxialMaterial::clear();
    direcciones.clear();
  }

XC::MaterialLoader *XC::ZeroLengthMaterials::get_material_loader(void)
  {
    MaterialLoader *retval= nullptr;
    EntCmd *owr= Owner();
    if(owr)
      {
        ZeroLength *elem= dynamic_cast<ZeroLength *>(owr);
        if(elem)
          {
            Preprocessor *preprocessor= elem->GetPreprocessor();
            if(preprocessor)
              retval= &preprocessor->getMaterialLoader();
            else
              std::cerr << "ZeroLengthMaterials::get_material_loader; el puntero al preprocesador es nulo." << std::endl;
          }
        else
          std::cerr << "ZeroLengthMaterials::get_material_loader; el propietario no es un elemento." << std::endl;
      }
    else
      std::cerr << "ZeroLengthMaterials::get_material_loader; pointer to owner not found." << std::endl;
    return retval;
  }

void XC::ZeroLengthMaterials::push_back(const int &dir,const UniaxialMaterial *t)
  {
    if(!t)
      std::cerr << "XC::DqUniaxialMaterial::push_back; se pasó un puntero a material nulo." << std::endl;
    else
      {
        UniaxialMaterial *tmp= nullptr;
        tmp= t->getCopy();
        if(tmp)
          {
            DqUniaxialMaterial::push_back(tmp);
            direcciones.push_back(dir);
          }
        else
          std::cerr << "DqUniaxialMaterial::push_back; no se pudo crear un objeto UniaxialMaterial" << std::endl;
      }
  }

void XC::ZeroLengthMaterials::push_front(const int &dir,const UniaxialMaterial *t)
  {
    if(!t)
      std::cerr << "XC::DqUniaxialMaterial::push_front; se pasó un puntero a material nulo." << std::endl;
    else
      {
        UniaxialMaterial *tmp= nullptr;
        tmp= t->getCopy();
        if(tmp)
          {
            DqUniaxialMaterial::push_front(tmp);
            direcciones.push_front(dir);
          }
        else
          std::cerr << "DqUniaxialMaterial::push_front; no se pudo crear un objeto UniaxialMaterial" << std::endl;
      }
  }

int XC::ZeroLengthMaterials::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3); 
    const size_t sz= size();

    int res= sendData(cp);
    ID direction(sz);
    for(size_t i= 0;i<sz;i++)
      direction[i]= direcciones[i];
    res+= cp.sendID(direction,getDbTagData(),CommMetaData(2));

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ZeroLengthMaterials::sendSelf -- failed to send.\n";
    return res;
  }

int XC::ZeroLengthMaterials::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ZeroLengthMaterials::recvSelf -- failed to receive.\n";
    else
      {
        res+= recvData(cp);
        ID direction;
        res+= cp.receiveID(direction,getDbTagData(),CommMetaData(2));
        const size_t sz= direction.Size();
        direcciones.resize(sz);
        for(size_t i= 0;i<sz;i++)
          direcciones[i]= direction(i);
      }
    return res;
  }


void XC::ZeroLengthMaterials::Print(std::ostream &s, int flag)
  {
    for(size_t j = 0; j < size(); j++)
      {
        s << "\tMaterial1d, tag: " << (*this)[j]->getTag()
          << ", dir: " << direcciones[j] << std::endl;
        s << *((*this)[j]);
      }
  }

// Private methods


// Check that direction is in the range of 0 to 5
void XC::ZeroLengthMaterials::checkDirection(void)
  {
    for(size_t i=0; i<direcciones.size(); i++)
      if(direcciones[i] < 0 || direcciones[i] > 5 )
        {
          std::cerr << "WARNING XC::ZeroLengthMaterials::checkDirection - incorrect direction " << direcciones[i] << " is set to 0\n";
          direcciones[i]= 0;
        }
  }
