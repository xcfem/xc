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
//RigidBase.h

#ifndef RigidBase_h
#define RigidBase_h

#include "MP_Constraint.h"

namespace XC {
class Domain;
class Node;

//! @ingroup CContMP
//
//! @brief Clase base para las condiciones de contorno de tipo
//! «movimiento de sólido rígido».
class RigidBase: public MP_Constraint
  {
  protected:
    Node *nodeR; //!< Pointer to retained node.
    Node *nodeC; //!< Pointer to constrained node.
  public:
    RigidBase(int tag);
    RigidBase(int tag,const int &, const int &, int classTag);
    ~RigidBase(void);
    void setDomain(Domain *theDomain);
  };
} // end of XC namespace

#endif
