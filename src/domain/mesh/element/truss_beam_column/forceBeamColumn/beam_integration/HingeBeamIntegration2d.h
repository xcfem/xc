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
//HingeBeamIntegration2d.h

#ifndef HingeBeamIntegration2d_h
#define HingeBeamIntegration2d_h

#include "PlasticLengthsBeamIntegration.h"

namespace XC {

class FVector;

//! \ingroup BeamInteg
//
//! @brief ??.
class HingeBeamIntegration2d: public PlasticLengthsBeamIntegration
  {
  protected:
    double E;
    double A;
    double I;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    HingeBeamIntegration2d(int classTag,double E, double A, double I,double lpI, double lpJ);
    HingeBeamIntegration2d(int classTag);
  
    void addElasticDeformations(ElementalLoad *theLoad, double loadFactor,double L,FVector &v0);
    void Print(std::ostream &s, int flag = 0);
  };
} // end of XC namespace

#endif
