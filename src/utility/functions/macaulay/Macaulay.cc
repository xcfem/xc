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
//Macaulay.cc

#include "Macaulay.h"
#include "utility/utils/misc_utils/matem.h"

double Macaulay::Eval(const double &x) const
  {
    double retval= 0.0;
    if (x>=a) retval= coef * pow(x-a,n);
    return retval;
  }

Macaulay Macaulay::Primitiva(void) const
  {
    Macaulay retval(*this);
    retval.n++;
    retval.coef*= (1.0/(double)retval.n);
    return retval;
  }

Macaulay &Macaulay::operator*=(const Macaulay &m)
  {
    if(this->Vars != m.Vars)
      {
        std::cerr << "Error en Macaulay *=" << std::endl;
        abort();
      }
    this->n+= m.n;
    this->a= std::max(this->a,m.a);
    this->coef*= m.coef;
    return *this;
  }
Macaulay Macaulay::Diferencial(void) const
  {
    if(n < 1)
      {
        std::cerr << "La diferencial no existe." << std::endl;
        abort();
      }     
    Macaulay retval(*this);
    retval.coef*= n;
    retval.n--;
    return retval;
  }

double Macaulay::Integral(const double &a, const double &b) const
  {
    Macaulay m= this->Primitiva();
    return m(b)-m(a);
  }
double Macaulay::Derivada(const double &x) const
  {
    Macaulay m= this->Diferencial();
    return m(x);
  }

Macaulay operator *(const Macaulay &m1,const Macaulay &m2)
  {
    Macaulay retval(m1);
    retval *= m2;
    return retval;
  }

bool operator ==(const Macaulay &p1,const Macaulay &p2)
  {
    if ((const Function &) p1 != (const Function &) p2) return 0;
    if (p1.n != p2.n) return 0;
    if (p1.a != p2.a) return 0;
    if (p1.coef != p2.coef) return 0;
    return 1;
  }

std::ostream &operator <<(std::ostream &stream,const Macaulay &p)
  {
    stream << p.coef << "*<" << p.GetNmbVar(1) 
           << ',' << p.a << '>' << p.n;
    return stream;
  }

double dot(const Macaulay &m1,const Macaulay &m2, const double &a, const double &b)
  {
    Macaulay m= m1*m2;
    return m.Integral(a,b);
  }
