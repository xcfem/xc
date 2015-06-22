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
//MapModelWrapper.h

#ifndef MAPSOLUMODEL_H
#define MAPSOLUMODEL_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "ModelWrapper.h"

namespace XC {

class ProcSoluControl;
class ModelWrapper;
class Domain;

//!  \ingroup Solu
//! 
//! @brief Contenedor de objetos ModelWrapper para el control del 
//! procedimiento de solución del problema de elementos finitos.
class MapModelWrapper: public EntCmd
  {
  public:
    typedef std::map<std::string,ModelWrapper> map_solu_model;
    typedef map_solu_model::iterator iterator;
    typedef map_solu_model::const_iterator const_iterator;

  private:
    map_solu_model solu_models; //!< Métodos para la solución.
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    MapModelWrapper(ProcSoluControl *owr);

    inline iterator begin()
      { return solu_models.begin(); }
    inline iterator end()
      { return solu_models.end(); }
    inline const_iterator begin() const
      { return solu_models.begin(); }
    inline const_iterator end() const
      { return solu_models.end(); }

    inline bool empty(void) const
      { return solu_models.empty(); }
    inline size_t size(void) const
      { return solu_models.size(); }

    bool existeModelWrapper(const std::string &) const;
    ModelWrapper &creaModelWrapper(const std::string &);
    const ModelWrapper *getModelWrapper(const std::string &) const;
    ModelWrapper *getModelWrapper(const std::string &);

    void clearAll(void);
  };

} // fin namespace XC

#endif
