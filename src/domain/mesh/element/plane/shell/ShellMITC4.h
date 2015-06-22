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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.9 $
// $Date: 2003/02/25 23:33:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/plane/shell/ShellMITC4.h,v $

// Ed "C++" Love
//
// B-bar four node shell element with membrane and drill
//
#ifndef ShellMITC4_h
#define ShellMITC4_h

#include "ShellMITC4Base.h"

namespace XC {

class ShellLinearCrdTransf3d;

//! \ingroup ElemPlanos
//
//! @brief Elemento shell MIT C4.
class ShellMITC4 : public ShellMITC4Base
  {
    static ShellLinearCrdTransf3d linear_trf;
  protected:
    DbTagData &getDbTagData(void) const;

  public:
    ShellMITC4(void);
    ShellMITC4(int tag,const SectionForceDeformation *ptr_mat); 
    ShellMITC4(int tag, int node1, int node2, int node3, int node4, const SectionForceDeformation &);
    Element *getCopy(void) const;


    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
      
  }; 

} // fin namespace XC

#endif
