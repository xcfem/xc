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
#ifndef Inelastic2DYS02_H
#define Inelastic2DYS02_H

#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/InelasticYS2DGNL.h"

namespace XC {
class CyclicModel;

//! \ingroup ElemBarra
//
//! @brief ??.
class Inelastic2DYS02 : public InelasticYS2DGNL
 {
 private:
   // int cycType;
   // double wT;
   double A;
   double E;
   double Iz;
   double resFactor;
   double alfa, beta;
   double delPmax;
   double delPMaxPos, delPMaxNeg;
   CyclicModel *cModel;
 public:
   Inelastic2DYS02(int tag, double A, double E, double Iz,
 		  int Nd1, int Nd2,
 		  YieldSurface_BC *ysEnd1,  YieldSurface_BC *ysEnd2,
 		  // int cyc_type, double wt,
 		  CyclicModel *cycModel,
 		  double del_p_max,
 		  double Alpha, double Beta, int rf_algo=-1,
 		  bool islinear=false, double rho=0.0);
   Element *getCopy(void) const;
   ~Inelastic2DYS02();
   int commitState(void);
   int update(void);

 protected:
   void getLocalStiff(Matrix &K) const;
 };
} // end of XC namespace

#endif

