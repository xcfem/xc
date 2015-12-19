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
#ifndef Inelastic2DYS03_H
#define Inelastic2DYS03_H

#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/InelasticYS2DGNL.h"

namespace XC {
//! \ingroup ElemBarra
//
//! @brief ??.
class Inelastic2DYS03 : public InelasticYS2DGNL
  {
  private:
    double Atens, Acomp, E;
    double IzPos, IzNeg;
    mutable Vector ndisp;
    Vector ndisp_hist;
  public:
    Inelastic2DYS03(int tag, double a_ten, double a_com, double e,
                double iz_pos, double iz_neg, int Nd1, int Nd2,
                   YieldSurface_BC *ysEnd1,  YieldSurface_BC *ysEnd2,
                    int rf_algo=-1, bool islinear=false, double rho=0);
    virtual int commitState();
    Element *getCopy(void) const;

  protected:
    virtual void getLocalStiff(Matrix &K) const;
  };
} // end of XC namespace

#endif

