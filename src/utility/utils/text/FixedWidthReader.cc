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
//FixedWidthReader.cc

#include "FixedWidthReader.h"
#include "text_string.h"
#include <iostream>
#include <sstream>

//! @brief Constructor.
FixedWidthReader::FixedWidthReader(const size_t &sz)
  : fmt(2*sz), campos(sz)
  {
    for(iterator i= campos.begin();i!=campos.end();i++)
      (*i)= "";
  }

//! @brief Constructor.
FixedWidthReader::FixedWidthReader(const std::vector<size_t> &format)
  : fmt(format), campos()
  {
    campos.resize(fmt.size()/2);
    for(iterator i= campos.begin();i!=campos.end();i++)
      (*i)= "";
  }

//! @brief Devuelve el número de campos.
const size_t FixedWidthReader::numCampos(void) const
  { return campos.size(); }
//! @brief Devuelve el valor del campo de índice i.
const std::string &FixedWidthReader::campo(const size_t &i) const
  { return campos[i]; }
//! @brief Devuelve el valor del campo de índice i.
std::string &FixedWidthReader::campo(const size_t &i)
  { return campos[i]; }


const std::string &FixedWidthReader::trata_campo(const std::string &str)
  {
    static std::string retval;
    retval= q_blancos(str);
    if(!es_numero(retval))
      retval= '"' + retval + '"';
    return retval;
  }

//! @brief Devuelve los valores separados por comas.
const std::string &FixedWidthReader::get_csv_string(void) const
  {
    static std::string retval;
    retval.clear();
    if(!campos.empty())
      {
        std::ostringstream os;
        const_iterator i=campos_begin();
        os << trata_campo(*i); i++;
        for(;i!=campos_end();i++)
          os << ',' << trata_campo(*i);
        retval= os.str();
      }
    return retval;
  }

//! @brief Devuelve los valores separados por comas.
const std::string &FixedWidthReader::get_csv_string(const std::string &str)
  {
    set_string(str);
    return get_csv_string();
  }

//! @brief Devuelve los valores separados por comas.
const std::string &FixedWidthReader::get_indices_string(void) const
  {
    static std::string retval;
    retval.clear();
    if(!fmt.empty())
      {
        std::ostringstream os;
        const_indices_iterator i=fmt.begin();
        os << *i; i++;
        for(;i!=fmt.end();i++)
          os << ',' << *i;
        retval= os.str();
      }
    return retval;
  }

//! @brief Asigna los índices de principio y final al campo cuyo índice se
//! pasa como parámetro.
//! @param field: Índice del campo cuyo principio y final se asignan.
//! @param ini: Índice del carácter en el que empieza el campo.
//! @param fin: Índice del carácter en el que termina el campo.
void FixedWidthReader::set_field_pair(const size_t &field, const size_t &ini,const size_t &fin)
  {
    if(field<numCampos())
      {
        if(fin<ini)
	  std::cerr << "FixedWidthReader::set_field_pair; error, el índice del final: " 
		    << fin << " es anterior al del principio: "  << ini << std::endl;
        const size_t j= field*2;
        fmt[j]= ini;
        fmt[j+1]= fin;
      }
    else
      std::cerr << "FixedWidthReader::set_field_pair; error, índice: " 
                << field << " fuera de rango." << std::endl;
  }

//! @brief Analiza la cadena que se pasa como parámetro.
void FixedWidthReader::set_string(const std::string &str)
  {
    const size_t nc= numCampos();
    const size_t imax= (str.size()-1);
    for(size_t i= 0;i<nc;i++)
      {
        const size_t j= i*2;
        const size_t ini= std::min(fmt[j],imax);
        const size_t fin= std::min(fmt[j+1],imax);
        const size_t len= fin-ini+1;
        campos[i]= str.substr(ini,len);
      }
  }

//! @brief Asigna el número de campos que se espera obtener.
void FixedWidthReader::set_num_campos(const size_t &sz)
  {
    fmt.resize(2*sz);
    campos.resize(sz);
    for(iterator i= campos.begin();i!=campos.end();i++)
      (*i)= "";
    for(indices_iterator i= fmt.begin();i!=fmt.end();i++)
      (*i)= 0;
  }

//! @brief Asigna el vector de índices y el número de campos que se espera obtener.
void FixedWidthReader::set_vector_indices(const vector_indices &v)
  {
    const size_t sz= v.size();
    fmt= v;
    campos.resize(sz/2);
    for(iterator i= campos.begin();i!=campos.end();i++)
      (*i)= "";
  }
