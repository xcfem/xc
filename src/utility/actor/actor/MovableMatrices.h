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
//MovableMatrices
                                                                        
                                                                        
#ifndef MovableMatrices_h
#define MovableMatrices_h

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Matrix.h"
#include <vector>

namespace XC {

class MovableMatrices: public MovableObject
  {
  protected:
    std::vector<Matrix> &vectores;
  public:
    explicit MovableMatrices(std::vector<Matrix> &);

    void setMatrices(const std::vector<Matrix> &);
    const std::vector<Matrix> &getMatrices(void) const
      { return vectores; }

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };

} // fin namespace XC

#endif

