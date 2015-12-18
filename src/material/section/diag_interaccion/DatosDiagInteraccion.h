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
//DatosDiagInteraccion

#ifndef DATOSDIAGINTERACCION_H
#define DATOSDIAGINTERACCION_H


#include "DefAgotPivotes.h"

namespace XC {


//! \@ingroup MATSCCDiagInt
//
//! @brief Parámetros necesarios para la obtención del diagrama de interacción.
class DatosDiagInteraccion: public EntCmd
  {
    double umbral; //!< Distancia mínima entre puntos del diagrama.
    double inc_eps; //!< Tamaño del paso en deformaciones.
    double inc_t; //!< Tamaño del paso en ángulo.
    DefAgotPivotes agot_pivotes; //!< Deformaciones de agotamiento en pivotes.
    std::string nmb_set_hormigon; //!< Nombre del conjunto de fibras de hormigón.
    int tag_hormigon; //!< Tag del material empleado para modelizar el hormigón.
    std::string nmb_set_armadura; //!< Nombre del conjunto de fibras de armadura. 
    int tag_armadura; //!< Tag del material empleado para modelizar la armadura.
  protected:
    bool procesa_comando(CmdStatus &status);    
  public:
    DatosDiagInteraccion(void);
    DatosDiagInteraccion(const double &u,const double &inc_e,const double &inc_t= M_PI/4,const DefAgotPivotes &agot= DefAgotPivotes());

    inline const double &getUmbral(void) const
      { return umbral; }
    inline void setUmbral(const double &v)
      { umbral= v; }
    inline const double &getIncEps(void) const
      { return inc_eps; }
    inline void setIncEps(const double &v)
      { inc_eps= v; }
    inline const double &getIncTheta(void) const
      { return inc_t; }
    inline void setIncTheta(const double &v)
      { inc_t= v; }
    inline const DefAgotPivotes &getDefsAgotPivotes(void) const
      { return agot_pivotes; }
    inline void setDefsAgotPivotes(const DefAgotPivotes &v)
      { agot_pivotes= v; }
    inline const std::string &getNmbSetHormigon(void) const
      { return nmb_set_hormigon; }
    inline void setNmbSetHormigon(const std::string &v)
      { nmb_set_hormigon= v; }
    inline const int &getTagHormigon(void) const
      { return tag_hormigon; }
    inline void setTagHormigon(const int &v)
      { tag_hormigon= v; }
    inline const std::string &getNmbSetArmadura(void) const
      { return nmb_set_armadura; }
    inline void setNmbSetArmadura(const std::string &v)
      { nmb_set_armadura= v; }
    inline const int &getTagArmadura(void) const
      { return tag_armadura; }
    inline void setTagArmadura(const int &v)
      { tag_armadura= v; }

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} // end of XC namespace

#endif
