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
//SectionMatrices.cc

#include "SectionMatrices.h"
#include "utility/actor/actor/MovableVectors.h"
#include "utility/actor/actor/MovableMatrices.h"
#include "utility/matrix/ID.h"

const size_t XC::SectionMatrices::maxNumSections;


//! @brief alocate section flexibility matrices and section deformation vectors
void XC::SectionMatrices::alloc(const size_t &sz)
  {
    vsSubdivide.resize(sz);
    fsSubdivide.resize(sz);
    SsrSubdivide.resize(sz);
  }

//! @brief libera espacio.
void XC::SectionMatrices::free(void)
  {
    vsSubdivide.clear();
    fsSubdivide.clear();
    SsrSubdivide.clear();
  }

XC::SectionMatrices::SectionMatrices(const size_t &sz)
  : MovableObject(0)
  { alloc(sz); }

//! @brief Redimensiona los objetos.
void XC::SectionMatrices::resize(const size_t &sz)
  { alloc(sz); }

//! @brief Inicializa los elementos cuyo índice se pasa como parámetro.
void XC::SectionMatrices::initialize(const size_t &i,const int &order)
  {
    fsSubdivide[i]= Matrix(order,order);
    vsSubdivide[i]= Vector(order);
    SsrSubdivide[i]= Vector(order);
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::SectionMatrices::sendSelf(CommParameters &cp)
  {
    inicComm(3);

    int res= cp.sendVectors(vsSubdivide,getDbTagData(),CommMetaData(0));
    res+= cp.sendMatrices(fsSubdivide,getDbTagData(),CommMetaData(1));
    res+= cp.sendVectors(SsrSubdivide,getDbTagData(),CommMetaData(2));

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "SectionMatrices::sendSelf -- failed to send ID data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::SectionMatrices::recvSelf(const CommParameters &cp)
  {
    inicComm(3);

    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "SectionMatrices::recvSelf -- failed to receive ID data\n";
    else
      {
        res+= cp.receiveVectors(vsSubdivide,getDbTagData(),CommMetaData(0));
        res+= cp.receiveMatrices(fsSubdivide,getDbTagData(),CommMetaData(1));
        res+= cp.receiveVectors(SsrSubdivide,getDbTagData(),CommMetaData(2));
      }
    return res;
  }
