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
//CorotShellMITC4.cpp

#include "CorotShellMITC4.h"
#include "ShellCorotCrdTransf3d.h"

XC::ShellCorotCrdTransf3d XC::CorotShellMITC4::corot_trf;

//! @brief Constructor
XC::CorotShellMITC4::CorotShellMITC4(void)
  : ShellMITC4Base(ELE_TAG_CorotShellMITC4,&corot_trf) {}

//! @brief Constructor
XC::CorotShellMITC4::CorotShellMITC4(int tag,const SectionForceDeformation *ptr_mat)
  : ShellMITC4Base(tag,ELE_TAG_CorotShellMITC4,ptr_mat,&corot_trf) {}

//! @brief Constructor
XC::CorotShellMITC4::CorotShellMITC4(int tag,int node1,int node2,int node3,int node4,SectionForceDeformation &theMaterial )
  : ShellMITC4Base(tag,ELE_TAG_CorotShellMITC4,&theMaterial,&corot_trf) {}

//! @brief Constructor virtual.
XC::Element* XC::CorotShellMITC4::getCopy(void) const
  { return new CorotShellMITC4(*this); }

//! @brief Actualiza los valores de las variables de estado.
int XC::CorotShellMITC4::update(void)
  {
    ShellMITC4Base::update();
    const ShellCorotCrdTransf3d *corot_transf= dynamic_cast<const ShellCorotCrdTransf3d *>(theCoordTransf);
    //local nodal coordinates in plane of shell
    for(int i= 0;i<4;i++)
      {
        const Vector &coorI= theNodes[i]->getCrds();
        xl[0][i]= coorI^corot_transf->G1trial();
        xl[1][i]= coorI^corot_transf->G2trial();
      }
    return 0;
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::CorotShellMITC4::getDbTagData(void) const
  {
    static DbTagData retval(16);
    return retval;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::CorotShellMITC4::sendSelf(CommParameters &cp)
  {
    inicComm(16);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "CorotShellMITC4::sendSelf() - failed to send ID data\n";
    return res;
  }

//! @brief Recibe el objeto por el canal que se pasa como parámetro.
int XC::CorotShellMITC4::recvSelf(const CommParameters &cp)
  {
    inicComm(16);

    const int dataTag= getDbTag();
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "CorotShellMITC4::sendSelf() - failed to receive ID data\n";
    else
      res+= recvData(cp);
    return res;
  }
