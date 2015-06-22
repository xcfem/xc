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
//DqMeshRegiones.h

#ifndef DqMeshRegion_h 
#define DqMeshRegion_h 

#include <deque>
#include <iostream>

namespace XC {

class MeshRegion;

//! @brief Vector de celdas.
class DqMeshRegion: protected std::deque<MeshRegion *>
  {
  public:
    typedef std::deque<MeshRegion *> dq_Reg;
    typedef dq_Reg::reference reference;
    typedef dq_Reg::const_reference const_reference;
    typedef dq_Reg::iterator iterator;
    typedef dq_Reg::const_iterator const_iterator;
  private:
    void libera(void);
    inline reference operator[](const size_t i)
      { return dq_Reg::operator[](i); }
  public:

    DqMeshRegion(void);
    DqMeshRegion(const DqMeshRegion  &otro);
    DqMeshRegion &operator=(const DqMeshRegion  &otro);
    ~DqMeshRegion(void);

    inline size_t size(void) const
      { return dq_Reg::size(); }

    void resize(const size_t sz);

    void add(const MeshRegion &c);
    inline const_reference operator[](const size_t i) const
      { return dq_Reg::operator[](i); }

    inline void clearAll(void)
      { libera(); }

    MeshRegion *getRegion(int tag);

    void Print(std::ostream &s) const;
  };

} // fin namespace XC


#endif
