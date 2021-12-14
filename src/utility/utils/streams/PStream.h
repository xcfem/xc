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
//PStream.h

#ifndef PSTREAM_H
#define PSTREAM_H

#include <iostream.h>
#include <map.h>
#include <String.h>

template <class T>
class PStream : public _IO_istream_withassign
  {
  public:
    typedef map<String,T *,less<String> > mapa_claves;
  private:
    mapa_claves mapa;
  public:
    PStream(void) : _IO_istream_withassign() {}
    PStream &operator=(istream &is)
      {
	_IO_istream_withassign::operator=(is);
        return *this;
      }
    void Registra(const String &str,const T *p)
      { mapa[str]= p->getCopy(); }
    T *ClaseDato(void)
      {
        String str;
	(_IO_istream_withassign &)(*this) >> str;
        mapa_claves::const_iterator i= mapa.find(str);
        if(i == mapa.end())
          {
            cerr << "La clase " << str << " no est'a registrada." << endl;
            return NULL;
          }
        return (*i).second;
      }
    void Save(ostream &os) const
      {
        mapa_claves::const_iterator i;
        for(i= mapa.begin();i!=mapa.end();i++)
          os << ' ' << (*i).first;
      }
  };

#endif

