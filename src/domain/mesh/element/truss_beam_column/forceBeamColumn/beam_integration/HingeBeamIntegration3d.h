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
//HingeBeamIntegration3d.h

#ifndef HingeBeamIntegration3d_h
#define HingeBeamIntegration3d_h

#include "PlasticLengthsBeamIntegration.h"
#include "material/section/repres/ConstantesSecc3d.h"

namespace XC {
class FVector;

//! \ingroup BeamInteg
//
//! @brief ??.
class HingeBeamIntegration3d: public PlasticLengthsBeamIntegration
  {
  protected:
    ConstantesSecc3d ctes_scc; //Características de la seccion E,A,Iy,...

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    HingeBeamIntegration3d(int classTag, double E, double A, double Iz,double Iy, double G, double J,double lpI, double lpJ);
    HingeBeamIntegration3d(int classTag, const ConstantesSecc3d &cts= ConstantesSecc3d(),const double &lpi=0.0,const double &lpj=0.0);
  
    void addElasticDeformations(ElementalLoad *theLoad,double loadFactor,double L,FVector &v0);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag = 0);
  };
} // end of XC namespace

#endif
