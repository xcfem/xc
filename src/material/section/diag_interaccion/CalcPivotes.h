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
//CalcPivotes.h
//Calcula los pivotes para un plano de flexión determinado.


#ifndef CALCPIVOTES_H
#define CALCPIVOTES_H

#include "xc_utils/src/geom/sis_ref/Ref3d3d.h"

namespace XC {

class DefAgotPivotes;
class DqFibras;
class StoFibras;
class Fiber;

//! @ingroup MATSCCDiagInt
//
//! @brief Calcula la posición de los pivotes en la sección
//! para un plano de flexión determinado.
class CalcPivotes: public Ref3d3d
  {
    const DefAgotPivotes &agot_pivotes; //!< Deformaciones de agotamiento en pivotes.
    const StoFibras &fibras;//!< Fibras de la sección.
    const DqFibras &fibrasC; //!< Fibras de hormigón (u otro material comprimido).
    const DqFibras &fibrasS; //!< Fibras de acero (u otro material a tracción). 
  public:
    CalcPivotes(const DefAgotPivotes &ap,const StoFibras &fs,const DqFibras &fsC,const DqFibras &fsS,const double &theta);
    const Fiber *getFiberCMinY(void) const;
    const Fiber *getFiberSMinY(void) const;
    const Fiber *getFiberCMaxY(void) const;
    Pos3d GetPuntoD(void) const;
    Pos3d GetPivoteA(void) const;
    Pos3d GetPivoteB(void) const;
    Pos3d GetPivoteC(void) const;
  };

} // fin namespace XC

#endif
