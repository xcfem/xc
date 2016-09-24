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
//PlateBase.h

#ifndef PlateBase_h
#define PlateBase_h

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include "material/section/SectionForceDeformation.h"


namespace XC {

//! \ingroup MATSCC
//!  @defgroup MATPLAC Materiales para elementos bidimensionales (placas,...).
//
//! @ingroup MATPLAC
//
//! @brief Base class for bidimensional membrane/plate/shell materials.
class PlateBase: public SectionForceDeformation
  {
  protected:
    double h; // plate thickness

  public: 

    PlateBase(int tag,int classTag);
    PlateBase(int classTag);
    PlateBase(int tag,int classTag, double h);

    double getStrain(const double &y,const double &z) const;

    inline double getH(void) const
      { return h; }
    void setH(const double &d)
      { h= d; }

  }; //end of PlateBase declarations

} // end of XC namespace

#endif
