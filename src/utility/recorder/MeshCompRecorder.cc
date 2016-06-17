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
//MeshCompRecorder.cpp

#include <utility/recorder/MeshCompRecorder.h>
#include <utility/handler/DataOutputHandler.h>

XC::MeshCompRecorder::MeshCompRecorder(int classTag)
  :HandlerRecorder(classTag), deltaT(0.0), nextTimeStampToRecord(0.0)
  {}

XC::MeshCompRecorder::MeshCompRecorder(int classTag,Domain &theDom, 
                                       DataOutputHandler &theOutputHandler,
                                       double dT, bool timeFlag)
  :HandlerRecorder(classTag,theDom,theOutputHandler,timeFlag),
   deltaT(dT), nextTimeStampToRecord(0.0) {}

//! @brief Envia el objeto mediante el comunicador
//! being passed as parameter.
int XC::MeshCompRecorder::sendData(CommParameters &cp)
  {
    int res= HandlerRecorder::sendData(cp);
    res+= cp.sendDoubles(deltaT,nextTimeStampToRecord,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives object through the channeldefinido en el parámetro.
int XC::MeshCompRecorder::receiveData(const CommParameters &cp)
  {
    int res= HandlerRecorder::receiveData(cp);
    res+= cp.receiveDoubles(deltaT,nextTimeStampToRecord,getDbTagData(),CommMetaData(5));
    return res;
  }
