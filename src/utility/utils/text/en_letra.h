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
//en_letra.h
//Utilidades para text strings.

#ifndef EN_LETRA_H
#define EN_LETRA_H

#include <string>
#include <iomanip>
#include <deque>
#include <sstream>
#include "utility/utils/streams/omanip.h"
#include "utility/utils/misc_utils/matem.h"
#include "text_string.h"

//! @brief Devuelve la text string que representa al número
//! que se pasa como parámetro siendo 'precision' el número
//! de decimales.
template <class N>
inline std::string num2str(const N &n,int precision)
  {
    std::ostringstream ostr;
    ostr.setf(std::ios::fixed,std::ios::floatfield);
    if(precision>0) ostr << std::setprecision(precision) << std::setw(precision);
    ostr << n;
    return ostr.str();
  }

std::string num2str(const size_t &n);

inline std::string bool2str(const bool &b)
  { return b ? "Si" : "No"; }

typedef std::deque<short int> dq_sint;

const dq_sint &el_decomp(const unsigned long &i);

const std::string &ldbl2str_ip(const long double &d);
const std::string &ldbl2str_fp(const long double &d,int precision= 16);

const dq_sint &el_decomp(const std::string &str);

inline const dq_sint &el_decomp_ip(const long double &d)
  { return el_decomp(ldbl2str_ip(d)); }

inline const dq_sint &el_decomp_fp(const long double &d,int precision= 16)
  { return el_decomp(ldbl2str_fp(d,precision)); }

const std::string &el_unidades(const unsigned short int &i,const bool &genero);
     //Convierte a letra las unidades.     
     //0 genero masculino.
     //1 genero femenino.

const std::string &el_decenas( const unsigned short int &dec,
                   const unsigned short int &uni,
                   const bool &genero);
     //Convierte a letra las decenas.     

const std::string &el_centenas( const unsigned short int &cen,
                    const unsigned short int &dec,
                    const unsigned short int &uni,
                    const bool &genero);
     //Convierte a letra las centenas.     


const std::string &en_letra(const unsigned long &i,const bool &genero);
const std::string &pon_coletilla(const std::string &str, const std::string &coletilla);
const std::string &en_letra(const long double &d,int num_dec,const bool &genero,const std::string &sep_dec= " con ", const std::string &coletilla= " céntimos" );
const std::string &en_humano(const unsigned long &i);
const std::string &en_humano(const long double &d,int num_dec,const char &sep_miles='.',const char &sep_dec= '\'',const bool &con_signo= false);

#endif
