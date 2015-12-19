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
//MapSoluMethod.h

#ifndef MAPSOLUMETHOD_H
#define MAPSOLUMETHOD_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/handler/DataOutputHandler.h"
#include "SoluMethod.h"

namespace XC {

class ProcSoluControl;
class ModelWrapper;

//!  \ingroup Solu
//! 
//! @brief Contenedor de objetos SoluMethod para el control del 
//! procedimiento de solución del problema de elementos finitos.
class MapSoluMethod: public EntCmd
  {
  private:
    typedef std::map<std::string,SoluMethod> map_solu_method;
    typedef map_solu_method::iterator iterator;
    typedef map_solu_method::const_iterator const_iterator;

    map_solu_method solu_methods; //!< Métodos para la solución.
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    MapSoluMethod(ProcSoluControl *owr);

    inline iterator begin()
      { return solu_methods.begin(); }
    inline iterator end()
      { return solu_methods.end(); }
    inline const_iterator begin() const
      { return solu_methods.begin(); }
    inline const_iterator end() const
      { return solu_methods.end(); }

    inline bool empty(void) const
      { return solu_methods.empty(); }
    inline size_t size(void) const
      { return solu_methods.size(); }

    bool existeSoluMethod(const std::string &) const;
    SoluMethod &creaSoluMethod(const std::string &,ModelWrapper *);
    const SoluMethod *getSoluMethod(const std::string &) const;
    SoluMethod *getSoluMethod(const std::string &);

    SoluMethod &newSoluMethod(const std::string &,const std::string &);

    void revertToStart(void);
    void clearAll(void);
  };

} // end of XC namespace

#endif
