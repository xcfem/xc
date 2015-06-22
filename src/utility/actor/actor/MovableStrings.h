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
//MovableStrings
                                                                        
                                                                        
#ifndef MovableStrings_h
#define MovableStrings_h

#include "MovableContainer.h"
#include <deque>

namespace XC {

class MovableStrings: public MovableContainer<std::deque<std::string> >
  {
  protected:
    
  public:
    explicit MovableStrings(std::deque<std::string> &);
    int sendItem(const std::string &s,CommParameters &,DbTagData &, const CommMetaData &);
    int receiveItem(std::string &s,const CommParameters &,DbTagData &, const CommMetaData &);

    const std::deque<std::string> &getStrings(void) const
      { return contenedor; }
  };

} // fin namespace XC

#endif

