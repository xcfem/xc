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
// @ rkaul@stanford.edu
// @ ggd@stanford.edu

#include "YieldSurfaceSection2d.h"

#ifndef YS_Section2D02_h
#define YS_Section2D02_h

namespace XC {
//! \ingroup MATSCCYS
//
//! @brief Sección transversal con superficie de fluencia
//! en un problema bidimensional (tres esfuerzos por sección).
class YS_Section2D02: public YieldSurfaceSection2d
  {
  private:
    double E, A, I;
    double maxPlstkRot, peakPlstkRot, iFactor;
  protected:
    void getSectionStiffness(Matrix &Ks);
  public:
    YS_Section2D02(int tag,double E, double A, double I,double theta_p_max,YieldSurface_BC *ptrys, bool use_kr=true);
    YS_Section2D02(int tag);
    YS_Section2D02(void);

    const Matrix &getInitialTangent(void) const;  
    SectionForceDeformation *getCopy(void) const;
    void Print (std::ostream &s, int flag =0);
    int commitState (void);
  };
} // fin namespace XC

#endif
