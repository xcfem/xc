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

#ifndef UserDefinedBeamIntegrationBase_h
#define UserDefinedBeamIntegrationBase_h

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h>

#include <utility/matrix/Vector.h>

namespace XC {

//! \ingroup BeamInteg
//
//! @brief Integración en la barra definida por el usuario.
class UserDefinedBeamIntegrationBase : public BeamIntegration
  {
  protected:
    Vector pts;
    Vector wts;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    UserDefinedBeamIntegrationBase(int , const Vector &, const Vector &);
    UserDefinedBeamIntegrationBase(int , const Vector &);
    UserDefinedBeamIntegrationBase(int);
    
    void getSectionLocations(int nIP, double L, double *xi) const;
    void getSectionWeights(int nIP, double L, double *wt) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag = 0);  
 };
} // end of XC namespace

#endif
