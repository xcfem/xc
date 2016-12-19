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
//EqualDOF.cpp

#include <domain/constraints/EqualDOF.h>
#include "utility/matrix/ID.h"
#include "utility/matrix/Matrix.h"
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>

XC::EqualDOF::EqualDOF(int tag)
  : MFreedom_Constraint(tag) {}

XC::EqualDOF::EqualDOF(int tag,const int &masterNode,const int &slaveNode,const ID &dofs)
  : MFreedom_Constraint(tag,masterNode,slaveNode,0)
  { setDofs(dofs); }

void XC::EqualDOF::setDofs(const ID &dofs)
  {
     set_retained_dofs(dofs);
     set_constrained_dofs(retainDOF);
  }

void XC::EqualDOF::setup_matrix(void)
  {
    // The number of DOF to be coupled
    const int numDOF= retainDOF.Size();

    // The constraint matrix ... U_c = C_cr * U_r
    Matrix Ccr(numDOF, numDOF);
    Ccr.Zero();

    for(int j=0;j<numDOF;j++) 
      Ccr(j,j) = 1.0;
    set_constraint(Ccr);
  }

void XC::EqualDOF::setup(Domain *theDomain)
  {  setup_matrix(); }



