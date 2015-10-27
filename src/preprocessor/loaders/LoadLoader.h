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
//LoadLoader.h

#ifndef LOADLOADER_H
#define LOADLOADER_H

#include "Loader.h"
#include "domain/load/pattern/MapLoadPatterns.h"
#include "domain/load/pattern/GrupoCombinaciones.h"

namespace XC {
class GroundMotion;
class Combinacion;

//!  \ingroup Ldrs
//! 
//! @brief Lee casos de carga desde archivo.
//! Se encarga del mantenimiento de la lista de timeseries.
class LoadLoader: public Loader
  {
    typedef std::map<std::string,GroundMotion *> map_ground_motions;
    map_ground_motions ground_motions; //!< Movimientos del terreno.

    MapLoadPatterns lpatterns; //!< Contenedor de hipótesis simples.
    int tag_lp; //!< Identificador por defecto para hipótesis simples o combinaciones.

    GrupoCombinaciones combinaciones; //!< Contenedor de combinaciones.

    template <class GM>
    void load_ground_motion(const std::string &,CmdStatus &);
    bool procesa_ground_motion(const std::string &,CmdStatus &);
  protected:
    friend class Combinacion;
    friend class Domain;
 
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    LoadLoader(Preprocessor *owr);
    ~LoadLoader(void);

    void addToDomain(const std::string &);
    void removeFromDomain(const std::string &);
    void removeAllFromDomain(void);

    inline int &getTagLP(void)
      { return tag_lp; }
    inline const int &getTagLP(void) const
      { return tag_lp; }
    inline void setTagLP(const int &tlp)
      { tag_lp= tlp; } 
    MapLoadPatterns &getLoadPatterns(void)
      { return lpatterns; }
    const MapLoadPatterns &getLoadPatterns(void) const
      { return lpatterns; }
    GrupoCombinaciones &getCombinaciones(void)
      { return combinaciones; }
    const GrupoCombinaciones &getCombinaciones(void) const
      { return combinaciones; }

    void clearAll(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };



//! @brief Carga un objeto GroundMotions desde archivo
template <class GM>
void XC::LoadLoader::load_ground_motion(const std::string &cmd,CmdStatus &status)
  {
    std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
    if(fnc_indices.size()<1)
      std::cerr << "sintaxis " << cmd << ": " << cmd << "[codigo] " << std::endl;
    GM *gm= new GM();
    if(gm) gm->LeeCmd(status);
    const std::string cod_gm= convert_to_string(fnc_indices[0]); //Código del ground motion.
    ground_motions[cod_gm]= gm;
  }



} // fin namespace XC

#endif
