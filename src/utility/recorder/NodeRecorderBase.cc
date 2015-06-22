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
//NodeRecorderBase.cpp

#include <utility/recorder/NodeRecorderBase.h>
#include <utility/matrix/ID.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

XC::NodeRecorderBase::NodeRecorderBase(int classTag)
  :MeshCompRecorder(classTag), theDofs(nullptr), theNodalTags(nullptr), theNodes(),
   dataFlag(0),numValidNodes(0)
  {}

XC::NodeRecorderBase::NodeRecorderBase(int classTag,const ID &dofs, const ID &nodes, 
                                       Domain &theDom, DataOutputHandler &theOutputHandler,
                                       double dT, bool timeFlag)
  :MeshCompRecorder(classTag,theDom,theOutputHandler,dT,timeFlag),
   theDofs(nullptr), theNodalTags(nullptr), theNodes(), 
   dataFlag(0), numValidNodes(0) {}

//! @brief Lee un objeto XC::NodeRecorderBase desde archivo
bool XC::NodeRecorderBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(NodeRecorderBase) Procesando comando: " << cmd << std::endl;
    return MeshCompRecorder::procesa_comando(status);
  }

XC::NodeRecorderBase::~NodeRecorderBase(void)
  {
    if(theDofs) delete theDofs;
    if(theNodalTags) delete theNodalTags;
  }

//! @brief Envía el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::NodeRecorderBase::sendData(CommParameters &cp)
  {
    int res= MeshCompRecorder::sendData(cp);
    res+= cp.sendIDPtr(theDofs,getDbTagData(),ArrayCommMetaData(6,7,8));
    res+= cp.sendIDPtr(theNodalTags,getDbTagData(),ArrayCommMetaData(9,10,11));
    res+= cp.sendInts(dataFlag,numValidNodes,getDbTagData(),CommMetaData(12));
    return res;
  }

//! @brief Recibe el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::NodeRecorderBase::receiveData(const CommParameters &cp)
  {
    int res= MeshCompRecorder::receiveData(cp);
    theDofs= cp.receiveIDPtr(theDofs,getDbTagData(),ArrayCommMetaData(6,7,8));
    theNodalTags= cp.receiveIDPtr(theNodalTags,getDbTagData(),ArrayCommMetaData(9,10,11));
    res+= cp.receiveInts(dataFlag,numValidNodes,getDbTagData(),CommMetaData(12));
    return res;
  }
