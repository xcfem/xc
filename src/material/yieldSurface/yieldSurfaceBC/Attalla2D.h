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
// Attalla2D.h: interface for the Attalla2D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ATTALLA2D_H
#define ATTALLA_H

#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC2D.h"

namespace XC {
//! \ingroup MATYS
//! @brief Superficie de fluencia de tipo Attalla 2D.
class Attalla2D : public YieldSurface_BC2D
  {
  protected:
    double a1, a2, a3, a4, a5, a6;
    int driftAlgo;
  public:
    Attalla2D(int tag, double xmax, double ymax, YS_Evolution &model,
              double a01=0.19,  double a02=0.54, double a03=-1.4,
              double a04=-1.64, double a05=2.21, double a06=2.10);

    virtual YieldSurface_BC *getCopy(void);
    virtual void Print(std::ostream &s, int flag =0);

//protected:
//  For the following 2 methods, x, y already non-dimensionalized
    virtual void getGradient(double &gx, double &gy, double x, double y);
    virtual double getSurfaceDrift(double x, double y);
    virtual void setExtent();
	virtual void	customizeInterpolate(double &xi, double &yi, double &xj, double &yj);
  };
} // fin namespace XC

#endif
