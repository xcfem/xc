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
//prep.cc

#include "prep.h"
#include "imanip.h"

//! @brief Quita los comentarios de c++ de la entrada y los escribe en la salida.
void quita_comentarios(std::istream &in,std::ostream &out)
  {
    char c;
    int escribe= 1;
    while(!in.eof())
      {
        in.get(c);
        if((c=='/') && escribe)
          {
            c= char(in.peek());
            if(c=='/')
              escribe= 0;
            else
              in.putback('/');               
          }
        if(escribe) out.put(c);
        if((c=='\n') && !escribe)
          escribe= 1;
      }
  }

//! @brief Escribe en os lo mismo que haya en is HASTA a_sust luego escribe en os sust.
int find_and_replace1(std::istream &is,const std::string &a_sust,std::ostream &os,const std::string &sust)
  {
    char c;
    const std::streampos start= is.tellg(); //Punto de comienzo de la busqueda.
    int encontrada= find_start(is,a_sust);
    std::streampos ini= is.tellg(); 
    is.seekg(start);
    if(encontrada)
      {
        encontrada= 1;
        while(is.tellg()<=ini) //El uno es para que no se coma los blancos.
          {
            c= is.get();
            if(is)
              os << c;
            else
              break;
          }
        os << sust;
        is.ignore(a_sust.length());
      }
    else
      vuelca_stream(is,os);
    return encontrada;
  }

//! @brief Escribe en os lo mismo que haya en is sustituyendo a_sust por sust.
int find_and_replace(std::istream &is,const std::string &a_sust,std::ostream &os,const std::string &sust)
  {
    int encontrada= 0;
    while(is)
      encontrada= find_and_replace1(is,a_sust,os,sust);
    vuelca_stream(is,os);
    return encontrada;
  }

//! @brief Devuelve una cadena de caracteres comprendida entre dos caracteres '"'
std::string get_bloque_comillas(std::istream &is, bool &error)
  {
    std::string retval;
    bool backslash= false;
    while(is)
      {
        char c= is.get();
        if(!(is)) break;
        if(c == '\n')
          {
	    std::cerr << "get_bloque_comillas; ERROR, no se cerraron comillas en: '"
                      << retval << "'\n";
            error= true;
            break;
          }
        retval+=c;
        if(c == '"' && !backslash)
          break;
        if(c == '\\')
          backslash= !backslash;
        else
          backslash= false;
      }
    retval= '"' + retval;
    return retval;
  }

//! @brief Devuelve una cadena de caracteres comprendida entre paréntesis '"'
//!
//! Se diferencia de get_bloque_llaves en que carga todo
//! hasta que terminan los parentesis.
std::string get_bloque_parentesis(std::istream &is,bool &error)
  {
    std::string retval;
    size_t cuenta_parentesis(1);  //Ya se leyó el primer paréntesis (
    while(cuenta_parentesis && (is))
      {
        const char c= is.get();
        if(!(is)) break;
        if(c == '"')
          {retval+= get_bloque_comillas(is,error);}
        else
          {
            if(c == '(')
              cuenta_parentesis++;
            else if(c == ')')
              cuenta_parentesis--;
            if(cuenta_parentesis) //El último paréntesis se ignora.
              retval+= c;
          }
      }
    retval= '(' + retval +')';
    if(error)
      std::cerr << "get_bloque_parentesis; ERROR, al interpretar la expresión: '"
                << retval << "'\n";
    return retval;
  }

//! @brief Devuelve un bloque del archivo de entrada.
//!
//! El bloque comienza con '{' y termina con '}'
//!
//! @param ignora_esp: Si es verdadero se ignoran los espacios (tabuladores,...) dentro del bloque.
std::string get_bloque_llaves(std::istream &is,bool &error,const bool &ignora_esp)
  {
    std::string retval;
    size_t cuenta_llaves(1); //Ya se leyó la primera llave {
    while(cuenta_llaves && (is))
      {
        const char c= is.get();
        if(!(is)) break;
        if(c == '{') cuenta_llaves++;
        if(c == '}') cuenta_llaves--;
        if(cuenta_llaves) //La última llave se ignora.
          {
            if(isspace(c) && ignora_esp)
              continue;  //Ignora espacios.
            else
              {
                if(c == '"')
                  { retval+= get_bloque_comillas(is,error); }
                else if(c=='(')
                  {retval+= get_bloque_parentesis(is,error); } 
                else                                  
                  retval+=c;
              }
          }
      }
    if(error)
      std::cerr << "get_bloque_llaves; ERROR, al interpretar la expresión: '"
                << retval << "'\n";
    return retval;
  }
