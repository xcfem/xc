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
//CorotShellMITC4.h

#ifndef CorotShellMITC4_h
#define CorotShellMITC4_h

#include "ShellMITC4Base.h"

namespace XC {

class ShellCorotCrdTransf3d;

//! \ingroup ElemPlanos
//
//! @brief Elemento shell MIT C4.
class CorotShellMITC4 : public ShellMITC4Base
  {
    static ShellCorotCrdTransf3d corot_trf;
  protected:
    DbTagData &getDbTagData(void) const;

    bool procesa_comando(CmdStatus &status);
  public:
    CorotShellMITC4(void);
    CorotShellMITC4(int tag,const SectionForceDeformation *ptr_mat); 
    CorotShellMITC4(int tag, int node1, int node2, int node3, int node4, SectionForceDeformation &theMaterial);
    Element *getCopy(void) const;

    int update(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
      
    any_const_ptr GetProp(const std::string &cod) const;
  }; 

} // fin namespace XC

#endif
