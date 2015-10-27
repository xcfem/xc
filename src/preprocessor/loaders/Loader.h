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
//Loader.h

#ifndef LOADER_H
#define LOADER_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"
#include <map>

namespace XC {
class Domain;
class Preprocessor;
class Channel;
class FEM_ObjectBroker;

//! \ingroup Preprocessor
//
//! @defgroup Ldrs Definición del modelo.
//
//! \ingroup Ldrs
//! 
//! @brief Objetos encargado de agregar al dominio los nodos,
//! elementos, cargas, etc. leídos desde archivo.
class Loader: public EntCmd,public MovableObject
  {
  protected:
    Preprocessor *modelador; //!< Puntero al modelador.
    bool procesa_comando(CmdStatus &status);
    friend class Preprocessor;    
    void set_modelador(Preprocessor *preprocessor);
  public:
    Loader(Preprocessor *owr);
    ~Loader(void);
    Domain *getDomain(void) const;

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC

#endif
