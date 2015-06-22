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
//FedeasConcrMaterial.h

#ifndef FedeasConcrMaterial_h
#define FedeasConcrMaterial_h

#include <material/uniaxial/FedeasMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief FedeasConcrMaterial wraps the FEDEAS
//! 1d material subroutine Concr_1.
class FedeasConcrMaterial: public FedeasMaterial
  {
  public:
    FedeasConcrMaterial(int tag, int classTag, int numHV, int numData, double fc, double ec, double fu, double eu);
    FedeasConcrMaterial(int tag, int classTag, int numHV, int numData, const Vector &data);
    FedeasConcrMaterial(int tag, int classTag, int numHV, int numData);

    double getInitialTangent(void) const;
  };
} // fin namespace XC


#endif

