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
//VectorReinfBars.h

#ifndef VectorReinfBar_h 
#define VectorReinfBar_h 

#include <vector>
#include <iostream>
#include "material/section/repres/SeccionInerte.h"

class Pos2d;

namespace XC {

class ReinfBar;
class Vector;
class Matrix;

//! @ingroup MATSCCArmaduras
//
//! @brief Vector de barras de armadura.
class VectorReinfBar: public std::vector<ReinfBar *>, public SeccionInerte
  {
  public:
    typedef std::vector<ReinfBar *> v_rfBar;
    typedef v_rfBar::reference reference;
    typedef v_rfBar::const_reference const_reference;
    typedef v_rfBar::iterator iterator;
    typedef v_rfBar::const_iterator const_iterator;
  private:
    void libera(void);
    void libera(const size_t i);
    void alloc(const size_t i,const ReinfBar &c);
    inline reference operator[](const size_t i)
      { return v_rfBar::operator[](i); }
  public:
    VectorReinfBar(const size_t &sz= 0);
    VectorReinfBar(const VectorReinfBar  &otro);
    VectorReinfBar &operator=(const VectorReinfBar  &otro);
    ~VectorReinfBar(void);

    inline size_t size(void) const
      { return v_rfBar::size(); }
    inline size_t empty(void) const
      { return v_rfBar::empty(); }
    inline iterator begin(void)
      { return v_rfBar::begin(); }
    inline iterator end(void)
      { return v_rfBar::end(); }
    inline const_iterator begin(void) const
      { return v_rfBar::begin(); }
    inline const_iterator end(void) const
      { return v_rfBar::end(); }

    double getAreaSeccBruta(void) const;
    Vector getCdgSeccBruta(void) const;
    double getIySeccBruta(void) const;
    double getIzSeccBruta(void) const;
    double getPyzSeccBruta(void) const;

    double getAreaSeccHomogeneizada(const double &E0) const;
    Vector getCdgSeccHomogeneizada(const double &E0) const;
    double getIySeccHomogeneizada(const double &E0) const;
    double getIzSeccHomogeneizada(const double &E0) const;
    double getPyzSeccHomogeneizada(const double &E0) const;

    void resize(const size_t sz);

    void put(const size_t i,const ReinfBar &c);
    inline const_reference operator[](const size_t i) const
      { return v_rfBar::operator[](i); }

    void Print(std::ostream &s) const;

  };

} // end of XC namespace


#endif
