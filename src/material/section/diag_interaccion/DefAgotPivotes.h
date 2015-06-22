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
//DefAgotPivotes

#ifndef DEFAGOTPIVOTES_H
#define DEFAGOTPIVOTES_H

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {

//! @ingroup MATSCCDiagInt
//
//! @brief Definición de las deformaciones de agotamiento en pivotes
//! de acuerdo con la norma empleada (ver por ejemplo diagrama de pivotes
//! en la EHE.
class DefAgotPivotes: public EntCmd
  {
    double eps_agot_A; //!< Deformación de agotamiento en el pivote A.
    double eps_agot_B; //!< Deformación de agotamiento en el pivote B.
    double eps_agot_C; //!< Deformación de agotamiento en el pivote C.
  protected:
    bool procesa_comando(CmdStatus &status);    
  public:
    DefAgotPivotes(const double &eA= 0.01,const double &eB= -0.0035,const double &eC= -0.002);

    const double &getDefAgotPivoteA(void) const;
    const double &getDefAgotPivoteB(void) const;
    const double &getDefAgotPivoteC(void) const;
    void setDefAgotPivoteA(const double &);
    void setDefAgotPivoteB(const double &);
    void setDefAgotPivoteC(const double &);
    const double getIncEpsAB(const double &ndiv= 25) const;
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC

#endif
