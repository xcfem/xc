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
//Spot.h
//Objeto punto de la representación de secciones

#ifndef SPOT_H
#define SPOT_H

#include "EntGeomSection.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"

class BND2d;

namespace XC {
class Eje;

//! @ingroup MATSCCEntGeom
//
//! @brief Punto en un espacio bidimensional.
class Spot: public EntGeomSection
  {
  private:
    Pos2d p; //!< Posición del punto.
    std::set<const Eje *> ejes_pt; //!< Ejes uno de cuyos extremos es este punto:
    friend class Eje;
    void inserta_linea(Eje *l);
    void borra_linea(Eje *l);
  protected:

    virtual void actualiza_topologia(void);

    friend class GeomSection;
    //! @brief Constructor.
    Spot(const Pos2d &pto= Pos2d());
    //! @brief Constructor.
    Spot(GeomSection *m)
      : EntGeomSection("",m) {}
    //! @brief Constructor.
    Spot(const std::string &nombre= "",GeomSection *m= NULL)
      : EntGeomSection(nombre,m) {}

  public:
    //! @brief Devuelve la dimensión del objeto.
    inline virtual unsigned short int GetDimension(void) const
      { return 0; }
    BND2d Bnd(void) const;

    //! @brief Devuelve la posición del objeto.
    const Pos2d &GetPos(void) const
      { return p; }
    //! @brief Devuelve la posición del objeto.
    Pos2d &GetPos(void)
      { return p; }
    inline void setPos(const Pos2d &pos)
      { p= pos; }
    Vector2d VectorPos(void) const;

    //! @brief Devuelve la lista de líneas que tocan al punto.
    const std::set<const Eje *> &EjesTocan(void) const
      { return ejes_pt; }
    bool Toca(const Eje &l) const;
    inline size_t nLines(void) const
      { return ejes_pt.size(); }

    double DistanciaA(const Pos2d &pt) const;


  };

} //end of XC namespace

#endif
