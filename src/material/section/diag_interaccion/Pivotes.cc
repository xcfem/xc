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
//Pivotes.cc

#include "Pivotes.h"
#include "CalcPivotes.h"


bool XC::Pivotes::checkPositions(void) const
  {
    bool retval= true;
    const double dMin= 1e-4;
    const Pos2d pA= GetPosPivoteA();
    const Pos2d pB= GetPosPivoteB();
    const Pos2d pC= GetPosPivoteC();
    const Pos2d pD= GetPosPuntoD();
    const double dAB= dist(pA,pB);
    if(dAB<dMin)
      {
        std::cerr << "Pivots A= " << pA << " and B= " 
                  << pB << " too close d= " << dAB << std::endl;
        retval= false;
      }
    const double dAC= dist(pA,pC);
    if(dAC<dMin)
      {
        std::cerr << "Pivots A= " << pA << " and C= " 
                  << pC << " too close d= " << dAC << std::endl;
        retval= false;
      }
    // const double dAD= dist(pA,pD); 

    const double dBC= dist(pB,pC);
    if(dBC<dMin)
      {
        std::cerr << "Pivots B= " << pB << " and C= " 
                  << pC << " too close d= " << dBC << std::endl;
        retval= false;
      }
    const double dBD= dist(pB,pD);
    if(dBD<dMin)
      {
        std::cerr << "Pivot B= " << pB << " and point D= " 
                  << pD << " too close d= " << dBD << std::endl;
        retval= false;
      }
    const double dCD= dist(pC,pD);
    if(dCD<dMin)
      {
        std::cerr << "Pivot C= " << pC << " and point D= " 
                  << pD << " too close d= " << dCD << std::endl;
        retval= false;
      }
    return retval;
  }

//! @brief Constructor.
XC::Pivotes::Pivotes(const Pos3d &a,const Pos3d &b,const Pos3d &c,const Pos3d &d)
  : A(a), B(b), C(c), D(d) 
  { ok= checkPositions(); }

//! @brief Constructor.
XC::Pivotes::Pivotes(const CalcPivotes &cp)
  : A(cp.calcPositionPivotA()), B(cp.calcPositionPivotB()), C(cp.calcPositionPivotC()), D(cp.GetPuntoD())
  { ok= checkPositions(); }


