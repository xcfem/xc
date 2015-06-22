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
//DistributedObj.cc

#include "DistributedObj.h"
#include "CommParameters.h"

XC::DistributedObj::DistributedObj(void)
  : processID(0) {}

XC::DistributedObj::DistributedObj(const ChannelQueue &theC)
  : processID(0), theChannels(theC) {}

bool XC::DistributedObj::buscaCanal(const CommParameters &cp,int &sendID)
  {
    // check if already using this object
    bool found = false;
    const int numChannels= theChannels.size();
    for(int i=0; i<numChannels; i++)
      if(theChannels[i] == cp.getChannel())
        {
          sendID= i+1;
	  found = true;
        }
    return found;
  }

int XC::DistributedObj::setProcessID(int dTag) 
  {
    processID = dTag;
    return 0;
  }

int XC::DistributedObj::setChannels(const ChannelQueue &theC)
  {
    theChannels= theC;
    return 0;
  }
