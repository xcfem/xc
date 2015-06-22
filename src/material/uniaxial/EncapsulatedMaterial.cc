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

#include "EncapsulatedMaterial.h"
#include <utility/matrix/ID.h>
#include "utility/matrix/Vector.h"


void XC::EncapsulatedMaterial::libera(void)
  {
    if(theMaterial) delete theMaterial;
    theMaterial= nullptr;
  }

void XC::EncapsulatedMaterial::copia(const UniaxialMaterial *otro)
  {
    libera();
    if(otro)
      theMaterial= otro->getCopy();
  }

XC::EncapsulatedMaterial::EncapsulatedMaterial(int tag, int classTag, UniaxialMaterial &material)
  :UniaxialMaterial(tag,classTag), theMaterial(nullptr)
  {
    copia(&material);
    if(!theMaterial)
      {
        std::cerr <<  "EncapsulatedMaterial::EncapsulatedMaterial -- failed to get copy of material\n";
        exit(-1);
      }
  }

XC::EncapsulatedMaterial::EncapsulatedMaterial(int tag, int classTag)
  :UniaxialMaterial(tag,classTag), theMaterial(nullptr) {}

XC::EncapsulatedMaterial::~EncapsulatedMaterial(void)
  { libera(); }

XC::EncapsulatedMaterial::EncapsulatedMaterial(const EncapsulatedMaterial &otro)
  :UniaxialMaterial(otro), theMaterial(nullptr)
  { copia(otro.theMaterial); }

XC::EncapsulatedMaterial &XC::EncapsulatedMaterial::operator=(const EncapsulatedMaterial &otro)
  {
    UniaxialMaterial::operator=(otro);
    copia(otro.theMaterial);
    return *this;
  }

double XC::EncapsulatedMaterial::getStrain(void) const
  { return theMaterial->getStrain(); }

double XC::EncapsulatedMaterial::getStrainRate(void) const
  { return theMaterial->getStrainRate(); }

int XC::EncapsulatedMaterial::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    int res= sendMaterialPtr(theMaterial,getDbTagData(),cp,BrokedPtrCommMetaData(1,2,3));
    return res;
  }

int XC::EncapsulatedMaterial::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    theMaterial= dynamic_cast<UniaxialMaterial *>(receiveMaterialPtr(theMaterial,getDbTagData(),cp,BrokedPtrCommMetaData(1,2,3)));
    
    return res;
  }
