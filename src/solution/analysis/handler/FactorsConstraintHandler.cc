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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.5 $
// $Date: 2005/11/29 22:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/FactorsConstraintHandler.cpp,v $

// Written: fmk
// Created: May 1998
// Revision: A
//
// What: "@(#) FactorsConstraintHandler.C, revA"

#include <solution/analysis/handler/FactorsConstraintHandler.h>
#include <utility/matrix/Vector.h>



XC::FactorsConstraintHandler::FactorsConstraintHandler(ModelWrapper *owr,int classTag,const double &sp,const double &mp)
  :ConstraintHandler(owr,classTag), alphaSP(sp), alphaMP(mp) {}

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::FactorsConstraintHandler::sendData(CommParameters &cp)
  {
    int res= ConstraintHandler::sendData(cp);
    res+= cp.sendDoubles(alphaSP,alphaMP,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::FactorsConstraintHandler::recvData(const CommParameters &cp)
  {
    int res= ConstraintHandler::recvData(cp);
    res+= cp.receiveDoubles(alphaSP,alphaMP,getDbTagData(),CommMetaData(2));
    return res;
  }

int XC::FactorsConstraintHandler::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::FactorsConstraintHandler::recvSelf(const CommParameters &cp)  
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

