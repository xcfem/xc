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
//VectorCells.h

#ifndef VectorCells_h 
#define VectorCells_h 

#include <vector>
#include <iostream>

namespace XC {

  class Cell;

//! @ingroup MATSCCCell
//
//! @brief Vector de celdas.
class VectorCells: protected std::vector<Cell *>
  {
  public:
    typedef std::vector<Cell *> v_cell;
    typedef v_cell::reference reference;
    typedef v_cell::const_reference const_reference;
    typedef v_cell::iterator iterator;
    typedef v_cell::const_iterator const_iterator;
  private:
    void libera(void);
    void libera(const size_t i);
    void alloc(const size_t i,const Cell &c);
    inline reference operator[](const size_t i)
      { return v_cell::operator[](i); }
  public:

    VectorCells(const size_t &sz= 0);
    VectorCells(const VectorCells  &otro);
    VectorCells &operator=(const VectorCells  &otro);
    ~VectorCells(void);

    inline size_t size(void) const
      { return v_cell::size(); }

    void resize(const size_t sz);

    void put(const size_t i,const Cell &c);
    inline const_reference operator[](const size_t i) const
      { return v_cell::operator[](i); }

    void Print(std::ostream &s) const;
  };

} // end of XC namespace


#endif
