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
//NDAdaptorMaterial.cc

#include <material/nD/NDAdaptorMaterial.h>
#include "utility/matrix/ID.h"
#include <material/nD/TipoMaterialND.h>


XC::NDAdaptorMaterial::NDAdaptorMaterial(int classTag, int strain_size)
  : NDMaterial(0, classTag), Tstrain22(0.0), Cstrain22(0.0), theMaterial(nullptr), strain(strain_size)
  {}

XC::NDAdaptorMaterial::NDAdaptorMaterial(int classTag, int tag, int strain_size)
  : NDMaterial(tag, classTag), Tstrain22(0.0), Cstrain22(0.0), theMaterial(nullptr), strain(strain_size)
  {}

XC::NDAdaptorMaterial::NDAdaptorMaterial(int classTag, int tag, XC::NDMaterial &theMat, int strain_size)
: XC::NDMaterial(tag, classTag), Tstrain22(0.0), Cstrain22(0.0), theMaterial(nullptr), strain(strain_size)

  {
    // Get a copy of the material
    theMaterial = theMat.getCopy(strTipoThreeDimensional);
  
    if(!theMaterial)
      {
        std::cerr << "NDAdaptorMaterial::NDAdaptorMaterial -- failed to get copy of material\n";
        exit(-1);
     }
  }

XC::NDAdaptorMaterial::~NDAdaptorMaterial(void) 
  { if(theMaterial) delete theMaterial; } 


int XC::NDAdaptorMaterial::commitState(void)
  {
    Cstrain22 = Tstrain22;
    return theMaterial->commitState();
  }

int XC::NDAdaptorMaterial::revertToLastCommit(void)
  {
    Tstrain22 = Cstrain22;
    return theMaterial->revertToLastCommit();
  }

int XC::NDAdaptorMaterial::revertToStart()
  {
    Tstrain22 = 0.0;
    Cstrain22 = 0.0;
    strain.Zero();
    return theMaterial->revertToStart();
  }

double XC::NDAdaptorMaterial::getRho(void) const
  { return theMaterial->getRho(); }

const XC::Vector& XC::NDAdaptorMaterial::getStrain(void)
  { return strain; }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::NDAdaptorMaterial::sendData(CommParameters &cp)
  {
    int res= NDMaterial::sendData(cp);
    res+= cp.sendDoubles(Tstrain22,Cstrain22,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(strain,getDbTagData(),CommMetaData(2));
    res+= cp.sendBrokedPtr(theMaterial,getDbTagData(),BrokedPtrCommMetaData(3,4,5));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::NDAdaptorMaterial::recvData(const CommParameters &cp)
  {
    int res= NDMaterial::recvData(cp);
    res+= cp.receiveDoubles(Tstrain22,Cstrain22,getDbTagData(),CommMetaData(1));
    res+= cp.receiveVector(strain,getDbTagData(),CommMetaData(2));
    if(theMaterial) delete theMaterial;
    theMaterial= cp.getBrokedMaterial(theMaterial,getDbTagData(),BrokedPtrCommMetaData(3,4,5));
    return res;
  }

void XC::NDAdaptorMaterial::Print(std::ostream &s, int flag)
  {
	s << "NDAdaptorMaterial, tag: " << this->getTag() << std::endl;
	s << "\tWrapped material: "<< theMaterial->getTag() << std::endl;

	theMaterial->Print(s, flag);
  }

