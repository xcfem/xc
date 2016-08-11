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
//RawConcrete.h
                                              
#ifndef RawConcrete_h
#define RawConcrete_h


#include "material/uniaxial/UniaxialMaterial.h"
#include "material/uniaxial/UniaxialStateVars.h"
#include "material/uniaxial/UniaxialHistoryVars.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Clase base para los materiales de tipo hormigón.
class RawConcrete: public UniaxialMaterial
  {
  protected:
    double fpc; //!< Resistencia a compresión
    double epsc0; //!< Deformación cuando se alcanza la resistencia a compresión
    double epscu; //!< Strain at crushing strength

    virtual void setup_parameters(void)= 0;

  public:
    RawConcrete(int tag, int classTag, double fpc, double eco, double ecu);
    RawConcrete(int tag, int classTag);

    void setFpc(const double &);
    double getFpc(void) const;
    void setEpsc0(const double &);
    double getEpsc0(void) const;
    void setEpscu(const double &);
    double getEpscu(void) const;
  };

} // end of XC namespace


#endif


