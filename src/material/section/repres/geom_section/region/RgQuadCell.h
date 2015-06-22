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
//RgQuadCell.h

#ifndef RgQuadCell_h 
#define RgQuadCell_h 


#include <iostream>
#include "material/section/repres/geom_section/region/RegionSecc.h" 

class Rejilla2d;

namespace XC {

//! @ingroup MATSCCRegiones
//
//! @brief Región de una sección que crea celdas de tipo
//! cuadrilátero.
//
//         nDivIJ= 4
// L +---+---+---+---+ K
//   |   |   |   |   |
//   +---+---+---+---+ nDivKL= 2
//   |   |   |   |   |
// I +---+---+---+---+ J
class RgQuadCell: public RegionSecc
  {
    mutable Rejilla2d *rejilla; //!< Puntos de la malla.
  protected:
    int nDivIJ, nDivJK; //!< Número de divisiones.
    void libera(void) const;
    const Rejilla2d &alloc(const Rejilla2d &) const;
    bool procesa_comando(CmdStatus &status);
  public:
    RgQuadCell(Material *);
    RgQuadCell(Material *, int numSubdivIJ, int numSubdivJK);
    RgQuadCell(const RgQuadCell &otro);
    RgQuadCell &operator=(const RgQuadCell &otro);
    virtual ~RgQuadCell(void);

    void setDiscretization(int numSubdivIJ, int numSubdivJK);

    inline void setNDivIJ(const int &n)
      { nDivIJ= n; }
    inline int getNDivIJ(void) const
      { return nDivIJ; }
    inline void setNDivJK(const int &n)
      { nDivJK= n; }
    inline int getNDivJK(void) const
      { return nDivJK; }
    //inquiring functions
    inline const int &nDivCirc(void) const
      { return nDivIJ; }
    inline int &nDivCirc(void)
      { return nDivIJ; }
    inline const int &nDivRad(void) const
      { return nDivJK; }
    inline int &nDivRad(void)
      { return nDivJK; }
    int getNumCells(void) const;
    void getDiscretization(int &numSubdivIJ, int &numSubdivJK) const;
    virtual const Rejilla2d &Malla(void) const=0;
    Vector getCdgRejilla(void) const;
    Vector getVertCoords(const size_t &,const size_t &) const;
    Matrix getCellVertCoords(const size_t &,const size_t &) const;
    
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC


#endif

