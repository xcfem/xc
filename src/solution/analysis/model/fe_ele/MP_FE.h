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
//MP_FE.h
                                                                        
                                                                        
#ifndef MP_FE_h
#define MP_FE_h

#include "MPBase_FE.h"

namespace XC {
class Node;
class MP_Constraint;

//! @ingroup AnalisisFE
//
//! @brief ??
class MP_FE: public MPBase_FE
  {
  protected:
    MP_Constraint *theMP;
    Node *theRetainedNode;    

    MP_FE(int tag, int numDOF_Group, int ndof, MP_Constraint &theMP,const double &alpha= 1.0);
    int determineConstrainedDOFsIDs(const int &);
    int determineRetainedDOFsIDs(const int &);
    DOF_Group *determineRetainedNodeDofGrpPtr(void);
  };
} // end of XC namespace

#endif


