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
//Pivotes.h
//Calcula los pivotes para un plano de flexión determinado.


#ifndef PIVOTES_H
#define PIVOTES_H

#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"

namespace XC {

class CalcPivotes;

//! @ingroup MATSCCDiagInt
//
//! @brief Conjunto de pivotes calculado para
//! un plano de flexión determinado.
class Pivotes
  {
    Pos3d A; //!< Pivote A (deformación máxima en el acero a tracción).
    Pos3d B; //!< Pivote B (deformación mínima en el hormigón comprimido).
    Pos3d C; //!< Pivote C (ver diagrama de pivotes en EHE).
    Pos3d D; //!< Deformación nula en el hormigón más traccionado.
    bool ok; //!< True if positions ok (see checkPositions).

  public:
    Pivotes(const Pos3d &a,const Pos3d &b,const Pos3d &c,const Pos3d &d);
    Pivotes(const CalcPivotes &cp);
    inline const Pos3d &getPuntoD(void) const
      { return D; }
    inline const Pos3d &getPivoteA(void) const
      { return A; }
    inline const Pos3d &getPivoteB(void) const
      { return B; }
    inline const Pos3d &getPivoteC(void) const
      { return C; }
    inline Pos3d getPuntoA(const double &epsilon)
      { return Pos3d(epsilon,A.y(),A.z()); }
    inline Pos3d getPuntoB(const double &epsilon)
      { return Pos3d(epsilon,B.y(),B.z()); }
    inline Pos3d getPuntoD(const double &epsilon)
      { return Pos3d(epsilon,D.y(),D.z()); }
    inline double getEpsilonA(void) const
      { return A.x(); }
    inline double getEpsilonB(void) const
      { return B.x(); }
    inline double getEpsilonC(void) const
      { return C.x(); }
    inline double getEpsilonD(void) const
      { return B.x(); }
    inline Pos2d getPosPivoteA(void) const
      { return Pos2d(A.y(),A.z()); }
    inline Pos2d getPosPivoteB(void) const
      { return Pos2d(B.y(),B.z()); }
    inline Pos2d getPosPivoteC(void) const
      { return Pos2d(C.y(),C.z()); }
    inline Pos2d getPosPuntoD(void) const
      { return Pos2d(D.y(),D.z()); }
    inline bool Ok(void) const
      { return ok; }
    bool checkPositions(void) const;
    void print(std::ostream &) const;
  };

inline std::ostream &operator<<(std::ostream &os, const Pivotes &p)
  {
    p.print(os);
    return os;
  }

} // end of XC namespace

#endif
