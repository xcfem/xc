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
//CentralDifferenceBase.h
                                                                        
#ifndef CentralDifferenceBase_h
#define CentralDifferenceBase_h

#include "solution/analysis/integrator/TransientIntegrator.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup TransientIntegrator
//
//! @brief Approximates velocity and acceleration by centered finite differences of displacement.
class CentralDifferenceBase : public TransientIntegrator
  {
  protected:
    int updateCount;    // method should only have one update per step
    Vector Udot;       // vel response quantity at time t-1/2 delta t
    double deltaT;

    CentralDifferenceBase(SoluMethod *,int classTag);
  public:
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);

    int newStep(double deltaT);
  };
} // fin namespace XC

#endif

