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
//MPBase_FE.h
                                                                        
                                                                        
#ifndef MPBase_FE_h
#define MPBase_FE_h

#include "MPSPBaseFE.h"

namespace XC {
class Node;
class MFreedom_ConstraintBase;
class DOF_Group;

//! @ingroup AnalisisFE
//
//! @brief ??
class MPBase_FE: public MPSPBaseFE
  {
  protected:
    Node *theConstrainedNode;

    MPBase_FE(int tag, int numDOF_Group, int ndof, const double &alpha= 1.0);
    int determineConstrainedDOFsIDs(const MFreedom_ConstraintBase &,const int &);
    DOF_Group *determineConstrainedNodeDofGrpPtr(void);
  };
} // end of XC namespace

#endif


