//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
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
//MEDVertexInfo.h
//Envoltorio para el objeto MESHING de MED (para exportar archivos a «salome»).

#ifndef MEDVERTEXINFO_H
#define MEDVERTEXINFO_H

#include "MEDBaseInfo.h"
#include "xc_basic/src/med_xc/MEDMEM_Meshing.hxx"

namespace XC {

class Mesh;
//! @ingroup MED
//
//!  @brief Información sobre los vértices (nodos) de la malla.
class MEDVertexInfo: public MEDBaseInfo
  {
    std::vector<double> coordenadas; //!< Coordenadas de los vértices.
    size_t spaceDimension; //!< Dimensión del espacio en el que se define la malla.
    std::string tipoCoordenadas; //!< "CARTESIAN", "CYLINDRICAL" or "SPHERICAL"
    std::vector<std::string> nombresCoordenadas; //!< Nombres de las coordenadas ("X","Y","Z",...).
    std::vector<std::string> nombresUnidades; //!< Nombres de las unidades ("cm","cm","cm",...).

  protected:
    void nuevo_vertice(size_t i,const std::vector<double> &coo);
    bool procesa_comando(CmdStatus &status);
  public:
    MEDVertexInfo(void);
    MEDVertexInfo(const Mesh &);

    //! @brief Devuelve el número de vertices.
    inline size_t getNumVertices(void) const
      { return coordenadas.size()/spaceDimension; }
    const std::vector<double> &getCoordenadas(void) const;
    inline size_t getSpaceDimension(void) const
      { return spaceDimension; }
    inline void setSpaceDimension(const size_t &sz)
      { spaceDimension= sz; }
    inline const std::string &getTipoCoordenadas(void) const
      { return tipoCoordenadas; }
    inline void setTipoCoordenadas(const std::string &str)
      { tipoCoordenadas= str; }
    inline const std::vector<std::string> &getCoordinateNames(void) const
      { return nombresCoordenadas; }
    void setCoordinateNames(const boost::python::list &);
    inline const std::vector<std::string> &getUnitNames(void) const
      { return nombresUnidades; }
    void setUnitNames(const boost::python::list &);
    void newVertex(const size_t &i,const boost::python::list &);

    void to_med(MEDMEM::MESHING &malla) const;
    void clear(void);

    any_const_ptr GetProp(const std::string &) const;
  };
} // fin namespace XC
#endif
