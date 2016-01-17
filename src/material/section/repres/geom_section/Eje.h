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
//Eje.h

#ifndef EJE_H
#define EJE_H

#include "EntGeomSection.h"


class BND2d;

namespace XC {
class Spot;

//! @ingroup MATSCCEntGeom
//
//! @brief Clase base para los objetos unidimensionales empleados para representar una sección.
class Eje: public EntGeomSection
  {
    friend class CmbEje;
  protected:
    size_t ndiv; //!< Número de divisiones



    friend class GeomSection;
    Eje(GeomSection *sr,const size_t &nd= 4);
    Eje(const std::string &nombre= "",GeomSection *m= NULL,const size_t &nd= 4);
  public:
    ~Eje(void);
    //! @brief Devuelve la dimensión del objeto.
    inline virtual unsigned short int GetDimension(void) const
      { return 1; }

    virtual Spot *P1(void);
    virtual const Spot *P1(void) const;
    virtual Spot *P2(void);
    virtual const Spot *P2(void) const;
    //! @brief Devuelve el número de vértices.
    virtual size_t NumVertices(void) const= 0;

    virtual const Spot *GetVertice(const size_t &i) const= 0;
    virtual Spot *GetVertice(const size_t &i)= 0;
    virtual void SetVertice(const size_t &,Spot *)= 0;

    virtual void SetNDiv(const size_t &nd)
      { ndiv= nd; }
    virtual size_t NDiv(void) const
      { return ndiv; }
    void actualiza_topologia(void);
 
    virtual BND2d Bnd(void) const= 0;


  };

std::set<const Eje *> GetLineasTocan(const Spot &p);

} //end of XC namespace
#endif
