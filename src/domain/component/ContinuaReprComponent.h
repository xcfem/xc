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
//ContinuaReprComponent.h                                                         
                                                                        
#ifndef ContinuaReprComponent_h
#define ContinuaReprComponent_h

#include "domain/component/DomainComponent.h"

namespace XC {

//! @brief Clase base para componentes que sirven
//! para representar el contínuo.
class ContinuaReprComponent: public DomainComponent
  {
    bool dead; //!< Verdadero si el elemento está desactivado.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors
    ContinuaReprComponent(int classTag);
    ContinuaReprComponent(int tag, int classTag);
    virtual const bool isDead(void) const
      { return dead; }
    virtual const bool isAlive(void) const
      { return !dead; }
    virtual void kill(void)
      { dead= true; }
    virtual void alive(void)
      { dead= false; }
    any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC

#endif

