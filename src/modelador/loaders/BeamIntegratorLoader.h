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
//BeamIntegratorLoader.h

#ifndef BEAMINTEGRATORLOADER_H
#define BEAMINTEGRATORLOADER_H

#include "Loader.h"
#include <map>

namespace XC {
class BeamIntegration;

//! \ingroup Ldrs
//! 
//! @brief Cargador de BeamIntegrators.
class BeamIntegratorLoader: public Loader
  {
  public:
    typedef std::map<std::string,BeamIntegration *> map_integradores;
    typedef map_integradores::const_iterator const_iterator;
    typedef map_integradores::iterator iterator;
  private:
    map_integradores integradores; //!< Integradores para el problema.
    void libera(void);
    BeamIntegratorLoader(const BeamIntegratorLoader &otro);
    BeamIntegratorLoader &operator=(const BeamIntegratorLoader &otro);
  protected:
    bool procesa_comando(CmdStatus &status);
    friend class ElementLoader;
    void ejecuta_bloque_for_each(CmdStatus &,const std::string &);
  public:
    BeamIntegratorLoader(Modelador *owr);
    const map_integradores &Map(void) const;
    const_iterator begin(void) const;
    const_iterator end(void) const;
    iterator begin(void);
    iterator end(void);
    const_iterator find(const std::string &str) const;
    iterator find(const std::string &str);
    BeamIntegration *find_ptr(const std::string &str);
    const BeamIntegration *find_ptr(const std::string &nmb) const;
    ~BeamIntegratorLoader(void);
    inline void clearAll(void)
      { libera(); }
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC

#endif
