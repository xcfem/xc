// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//FixedWidthReader.h

#ifndef FIXEDWIDTHREADER_H
#define FIXEDWIDTHREADER_H

#include <string>
#include <vector>

//! @brief Lee campos de ancho fijo en una cadena de caracteres.
class FixedWidthReader
  {
  public:
    typedef std::vector<size_t> vector_indices;
    typedef vector_indices::iterator indices_iterator;
    typedef vector_indices::const_iterator const_indices_iterator;

    typedef std::vector<std::string> vector_campos;
    typedef vector_campos::iterator iterator;
    typedef vector_campos::const_iterator const_iterator;
  private:
    vector_indices fmt; //Posiciones <inicio,fin> de los campos a leer.
    vector_campos campos; //Valores leídos para cada uno de los campos.
    static const std::string &trata_campo(const std::string &);
  public:
    FixedWidthReader(const size_t &size);
    FixedWidthReader(const std::vector<size_t> &format);

    const size_t numCampos(void) const;
    const std::string &campo(const size_t &i) const;
    std::string &campo(const size_t &i);
    inline iterator campos_begin(void)
      { return campos.begin(); }
    inline const_iterator campos_begin(void) const
      { return campos.begin(); }
    inline iterator campos_end(void)
      { return campos.end(); }
    inline const_iterator campos_end(void) const
      { return campos.end(); }
    const std::string &get_csv_string(void) const;
    const std::string &get_csv_string(const std::string &);
    const std::string &get_indices_string(void) const;

    void set_num_campos(const size_t &nc);
    void set_vector_indices(const vector_indices &);
    void set_field_pair(const size_t &field, const size_t &ini,const size_t &fin);
    void set_string(const std::string &str);
  };

#endif
