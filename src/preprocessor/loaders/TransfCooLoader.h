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
//TransfCooLoader.h

#ifndef TRANSFCOOLOADER_H
#define TRANSFCOOLOADER_H

#include "Loader.h"
#include <map>

namespace XC {
class Domain;
class CrdTransf;
class LinearCrdTransf2d;
class LinearCrdTransf3d;
class PDeltaCrdTransf2d;
class PDeltaCrdTransf3d;
class CorotCrdTransf2d;
class CorotCrdTransf3d;


//! @ingroup Ldrs
//! 
//! @brief Cargador de transformaciones de coordenadas.
class TransfCooLoader: public Loader
  {
  public:
    typedef std::map<std::string,CrdTransf *> map_transfcoo;
    typedef map_transfcoo::const_iterator const_iterator;
    typedef map_transfcoo::iterator iterator;
  private:
    map_transfcoo transfcoo; //!< Transformaciones de coordenadas para el problema.
    int tag_trf; //!< Tag de la transformación de coordenadas.
  protected:
    void libera(void);
    TransfCooLoader(const TransfCooLoader &otro);
    TransfCooLoader &operator=(const TransfCooLoader &otro);
    bool procesa_comando(CmdStatus &status);
  public:
    TransfCooLoader(Preprocessor *owr);
    const map_transfcoo &Map(void) const;

    LinearCrdTransf2d *newLinearCrdTransf2d(const std::string &);
    LinearCrdTransf3d *newLinearCrdTransf3d(const std::string &);
    PDeltaCrdTransf2d *newPDeltaCrdTransf2d(const std::string &);
    PDeltaCrdTransf3d *newPDeltaCrdTransf3d(const std::string &);
    CorotCrdTransf2d *newCorotCrdTransf2d(const std::string &);
    CorotCrdTransf3d *newCorotCrdTransf3d(const std::string &);

    const_iterator begin(void) const;
    const_iterator end(void) const;
    iterator begin(void);
    iterator end(void);
    const_iterator find(const std::string &str) const;
    iterator find(const std::string &str);
    CrdTransf *find_ptr(const std::string &str);
    ~TransfCooLoader(void);

    void clearAll(void);
  };

} // fin namespace XC

#endif
