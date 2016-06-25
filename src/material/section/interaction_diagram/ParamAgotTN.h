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
//ParamAgotTN

#ifndef PARAMAGOTTN_H
#define PARAMAGOTTN_H

namespace XC {

class DefAgotPivotes;

//! @ingroup MATSCCDiagInt
//
//! @brief Parámetros que definen el agotamiento de una sección 
//! en tensiones normales.
class ParamAgotTN
  {
    const DefAgotPivotes &agot_pivotes; //!< Deformaciones de agotamiento en pivotes.
    double eps_c_max; //!< Deformación máxima en el hormigón (fibra más traccionada).
    double eps_c_min; //!< Deformación mínima en el hormigón (fibra más comprimida).
    double eps_c_pC; //!< Deformación en el hormigón en el pivote C.

  private:
    double fc_tracc(void) const;
    double fc_comp(void) const;
    double fc_pC(void) const;
    double fc_min(void) const;
  public:
    ParamAgotTN(const DefAgotPivotes &ap,const double &emx= 0.0,const double &emn= 0.0,const double &ec= 0.0);
    
    bool Cumple(void) const;
    bool Agotada(void) const;
  };

} // end of XC namespace

#endif
