//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  software es libre: usted puede redistribuirlo y/o modificarlo
//  bajo los términos de la Licencia Pública General GNU publicada
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.
//  GNU General Public License for more details.
//  una información más detallada.
//
// You should have received a copy of the GNU General Public License
// junto a este programa.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------

// Written: lcpt
// Created: 03/2015
// Revision: A

#include "Lagrange_FE.h"
#include <solution/analysis/model/dof_grp/DOF_Group.h>

//! @brief Constructor
XC::Lagrange_FE::Lagrange_FE(DOF_Group &theDofGrp)
  : theDofGroup(&theDofGrp)
  {}

int XC::Lagrange_FE::getLagrangeDOFGroupTag(void) const
  {
    int retval= -1;
    if(theDofGroup)
      retval= theDofGroup->getTag();
    return retval;
  }
