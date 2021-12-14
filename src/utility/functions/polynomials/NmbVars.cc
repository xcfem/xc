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
//NmbVars.cc

#include "NmbVars.h"

void NmbVars::PutStr(const std::string &str)
  {
    short unsigned int i,mx= std::min(this->size(),str.size());
    for(i=0;i<mx;i++) (*this).at(i)= str[i];
  }
NmbVars NmbVars::Combina(const NmbVars &n) const
//Devuelve la union de los conjuntos de variables
//de this y de n.
  {
    short unsigned int i,sz;
    std::set<char,std::less<char> > c(this->begin(),this->end());
    sz= n.size();
    for (i=0;i<sz;i++) c.insert(n[i]);
    NmbVars comb(c.size());
    i= 0;
    for(std::set<char,std::less<char> >::iterator j=c.begin();j!=c.end();j++)
      comb[i++]= *j;
    return comb;
  }
  
vZ_sui NmbVars::Indices(const NmbVars &n) const
//Devuelve los indices de las variables de n en this.
//Ejemplo:
// *this contiene [x,y,z,t,b,a]
// n contiene: [b,z,y,a]
// el mEtodo devuelve [4,2,1,5]
  {
    short unsigned int i,sz= n.size();
    int indice;
    vZ_sui v(sz);
    for (i=0;i<sz;i++)
      {
        indice = Busca(n[i]);
        if (indice == -1)
          {
	    std::cerr << "La variable: " << n[i] << " no figura en: " << *this;
            abort();
          }
        v.at(i)= indice;
      }
    return v;
  }
