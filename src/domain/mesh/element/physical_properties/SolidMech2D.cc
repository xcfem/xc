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
//NDMaterialPhysicalProperties.cc

#include "SolidMech2D.h"

//! @brief Constructor.
XC::SolidMech2D::SolidMech2D(const size_t &nMat,const NDMaterial *ptr_mat, const double &t, const double &r)
  :NDMaterialPhysicalProperties(nMat,ptr_mat), thickness(t), rho(r) {}

//! @brief Constructor.
XC::SolidMech2D::SolidMech2D(const size_t &nMat, NDMaterial &theMat,const std::string &type, const double &t, const double &r)
  : NDMaterialPhysicalProperties(nMat,nullptr), thickness(t), rho(r)
  {
    if(check_material_type(type))
      theMaterial.setMaterial(&theMat,type);
  }

//! @brief Returns densities for each position.
XC::Vector XC::SolidMech2D::getRhoi(void) const
  { return NDMaterialPhysicalProperties::getRhoi(rho); }

#define DBTAGS_SIZE 3
//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::SolidMech2D::getDbTagData(void) const
  {
    static DbTagData retval(DBTAGS_SIZE); 
    return retval;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::SolidMech2D::sendData(CommParameters &cp)
  {
    int res= NDMaterialPhysicalProperties::sendData(cp);
    res+= cp.sendDoubles(thickness,rho,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::SolidMech2D::recvData(const CommParameters &cp)
  {
    int res= NDMaterialPhysicalProperties::recvData(cp);
    res+= cp.receiveDoubles(thickness,rho,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object.
int XC::SolidMech2D::sendSelf(CommParameters &cp)
  {
    inicComm(DBTAGS_SIZE);

    int res= this->sendData(cp);

    const int dataTag= getDbTag();
    res += cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "SolidMech2D::sendSelf -- failed to send ID data\n";
    return res;
  }

//! @brief Receives object
int XC::SolidMech2D::recvSelf(const CommParameters &cp)
  {
    inicComm(DBTAGS_SIZE);

    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "SolidMech2D::recvSelf -- failed to receive ID data\n";
    else
      res+= this->recvData(cp);
    return res;
  }
