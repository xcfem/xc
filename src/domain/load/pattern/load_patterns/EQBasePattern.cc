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
//EQBasePattern.cpp

#include <domain/load/pattern/load_patterns/EQBasePattern.h>
#include <domain/load/groundMotion/GroundMotion.h>


XC::EQBasePattern::EQBasePattern(int tag, int _classTag)
  :LoadPattern(tag, _classTag), theMotions() {}


int XC::EQBasePattern::addMotion(GroundMotion &theMotion)
  {
    theMotions.addMotion(theMotion); 
    return 0;
  }

void XC::EQBasePattern::alloc(const size_t &sz)
  { theMotions= DqGroundMotions(sz); }

void XC::EQBasePattern::Print(std::ostream &s, int flag)
  { std::cerr << "XC::EQBasePattern::Print() - not yet implemented\n"; }

//! @brief Send members through the channel being passed as parameter.
int XC::EQBasePattern::sendData(CommParameters &cp)
  {
    int res= EQBasePattern::sendData(cp);
    res+= cp.sendMovable(theMotions,getDbTagData(),CommMetaData(16));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::EQBasePattern::recvData(const CommParameters &cp)
  {
    int res= EQBasePattern::recvData(cp);
    res+= cp.receiveMovable(theMotions,getDbTagData(),CommMetaData(16));
    return res;
  }
