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


#ifndef FedeasBondMaterial_h
#define FedeasBondMaterial_h

#include <material/uniaxial/FedeasMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief FedeasBondMaterial wraps the FEDEAS
//! 1d material subroutine Bond_1.
class FedeasBondMaterial: public FedeasMaterial
  {
  protected:
    FedeasBondMaterial(int tag, int classTag, int numHV, int numData);
    FedeasBondMaterial(int tag,int classTag, int numHV, int numData,
		double u1p, double q1p, double u2p, double u3p, double q3p,
		double u1n, double q1n, double u2n, double u3n, double q3n,
		double s0, double bb);
    FedeasBondMaterial(int tag, int classTag, int numHV, int numData, const Vector &data);
  public:
    double getInitialTangent(void) const;
  };
} // end of XC namespace


#endif

