// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//TablaLiterales.h

#ifndef TABLALITERALES_H
#define TABLALITERALES_H

#include "Operando.h"
#include <map>

class TablaLiterales
  {
  private:
    class Literal : public Operando
      {
        //static TablaLiterales *ptr_tabla;
        //Literal &operator=(const Literal &otro);
      public:
	explicit Literal(const double &d= 0.0);
        inline virtual bool Evaluable(void) const
          { return true; }
        inline virtual bool EqualTo(const double &d) const
          { return (valor == d); }
        virtual Rama Diferencia(const Variable &,const Rama &) const;
        virtual int GetPrioridad(void) const
          { return 8; }
        std::ostream &Imprime(std::ostream &os) const;

        friend int operator <(const Literal &c1,const Literal &c2)
          { return (c1.valor < c2.valor); }
        friend int operator ==(const Literal &c1,const Literal &c2)
          { return (c1.valor == c2.valor); }
        friend int operator !=(const Literal &c1,const Literal &c2)
          { return (c1.valor != c2.valor); }
        friend int operator >(const Literal &c1,const Literal &c2)
          { return (c1.valor > c2.valor); }

      };
    typedef std::map<Literal,size_t> tabla_literales;
    typedef tabla_literales::iterator iterator;
    typedef tabla_literales::const_iterator const_iterator;
    typedef tabla_literales::value_type value_type;
    tabla_literales literales;

    const Literal *Nuevo(const Literal &l);
    void NuevaRef(const Literal &l);
    void Borra(const Literal &l);
  public:
    TablaLiterales(void);
    inline const Operando *NuevoDbl(const double &d)
      { return Nuevo(Literal(d)); }
    void NuevaRef(const double &);
    void NuevaRef(const Segnal *);
    void Borra(const double &d);
    void BorraSiLiteral(const Segnal *s);
    inline size_t size(void) const
      { return literales.size(); }
 };

#endif
