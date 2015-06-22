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

#include <utility/recorder/EnvelopeData.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/actor/actor/MatrixCommMetaData.h"

//! @brief Constructor.
XC::EnvelopeData::EnvelopeData(void)
  :MovableObject(RECORDER_TAGS_EnvelopeData), data(nullptr), currentData(nullptr), first(true) {}

void XC::EnvelopeData::alloc(const size_t &numDbColumns)
  {
    free();
    data = new Matrix(3, numDbColumns);
    currentData = new Vector(numDbColumns);
    if(data == 0 || currentData == 0)
      {
        std::cerr << "EnvelopeData::alloc() - out of memory\n";
        exit(-1);
      }
    else
     data->Zero();
  }

void XC::EnvelopeData::free(void)
  {
    if(data) delete data;
    data= nullptr;
    if(currentData) delete currentData;
    currentData= nullptr;
  }

//! @brief Destructor.
XC::EnvelopeData::~EnvelopeData(void)
  { free(); }

int XC::EnvelopeData::restart(void)
  {
    data->Zero();
    first = true;
    return 0;
  }

//! @brief Envía el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::EnvelopeData::sendData(CommParameters &cp)
  {
    int res= cp.sendMatrixPtr(data,getDbTagData(),MatrixCommMetaData(0,1,2,3));
    res+= cp.sendVectorPtr(currentData,getDbTagData(),ArrayCommMetaData(4,5,6));
    setDbTagDataPos(7,first);
    return res;
  }

//! @brief Recibe el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::EnvelopeData::receiveData(const CommParameters &cp)
  {
    data= cp.receiveMatrixPtr(data,getDbTagData(),MatrixCommMetaData(0,1,2,3));
    currentData= cp.receiveVectorPtr(currentData,getDbTagData(),ArrayCommMetaData(4,5,6));
    first= getDbTagDataPos(7);
    return 0;
  }

  
int XC::EnvelopeData::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(cp);
    if(cp.sendIdData(getDbTagData(),dataTag)< 0)
      {
        std::cerr << "EnvelopeData::sendSelf() "
                  << "- failed to send data\n";
        return -1;
      }
    return res;
  }


int XC::EnvelopeData::recvSelf(const CommParameters &cp)
  {
    inicComm(8);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "EnvelopeElementRecorder::recvSelf() - failed to recv data\n";
    else
      res= receiveData(cp);
    return res;
  }
