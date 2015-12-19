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
#ifndef QuadraticCyclic_H
#define QuadraticCyclic_H

#include "CyclicModel.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

namespace XC {
//! \ingroup ElemBarra
//
//! @brief ??.
class QuadraticCyclic : public CyclicModel
  {
  private:
    double a, b, c;
    double weightFactor, facty;
    double qx1, qy1, qx2, qy2, qx3, qy3;
    static Matrix X;
    static Vector Y, A;
  public:
    QuadraticCyclic(int tag, double wt=0.9, double qy=0.33);

    void Print (std::ostream &, int = 0);
    CyclicModel *getCopy();
  protected:
    int createFullCycleTask();
    int createHalfCycleTask();
    double getTaskFactor();
  private:
    int solveQuad(double x1, double y1, double x2,double y2, double x3, double y);
    double getQuadFactor(double x1, double y1, double dx);
    int createTask(void);
  };
} // end of XC namespace

#endif

