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
//DistributedLinSOE.h
                                                                        
#ifndef DistributedLinSOE_h
#define DistributedLinSOE_h


#include "utility/actor/actor/DistributedObj.h"
#include "xc_utils/src/nucleo/EntCmd.h"
#include <vector>

namespace XC {

class ID;
class Graph;
class CommParameters;

//! @ingroup SOE
//
//! @brief Clase base para los sistemas de ecuaciones
//! distribuidos con matriz en banda
class DistributedLinSOE: public DistributedObj
  {
  protected:
    std::vector<ID> localCol;

    int sendGraph(Graph &theGraph,ID &datos);
    int getSubGraphs(Graph &);
    int sendSizeData(const ID &datos);
    void calcLocalMap(const int &size);
    int getSendID(CommParameters &cp);
    int send(CommParameters &);
    int receive(const CommParameters &);

    DistributedLinSOE(void);
  public:
    int setChannels(const ChannelQueue &theChannels);
  };
} // fin namespace XC


#endif

