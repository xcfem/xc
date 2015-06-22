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
//DistributedBandLinSOE.h
                                                                        
#ifndef DistributedBandLinSOE_h
#define DistributedBandLinSOE_h


#include "solution/system_of_eqn/linearSOE/DistributedLinSOE.h"
#include "utility/matrix/Vector.h"

namespace XC {

class Graph;
class CommParameters;
class LinearSOEData;
class BandGenLinSOE;
class FactoredSOEBase;
class ID;
class Vector;

//! @ingroup SOE
//
//! @brief Clase base para los sistemas de ecuaciones
//! distribuidos con matriz en banda
class DistributedBandLinSOE: public DistributedLinSOE
  {
  protected:
    mutable std::vector<double> workArea;
    std::vector<double> myB;
    Vector myVectB;

    void setupMyB(const std::size_t &,const std::size_t &);
    int addB(const int &size,const Vector &, const ID &,const double &);
    int setB(const int &size,const Vector &, const double &);
    void zeroB(void);
    int sendBArecvBX(LinearSOEData &,const bool &,double *,const int &,ID &);
    int recvBAsendBX(FactoredSOEBase &) const;
    int sendResultsBack(FactoredSOEBase &,ID &);
    int recvXA(FactoredSOEBase &,const bool &,double *A,const int &,ID &);

    DistributedBandLinSOE(void);
  };
} // fin namespace XC


#endif

