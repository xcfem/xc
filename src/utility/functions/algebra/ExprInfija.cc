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
//ExprInfija.cc

#include "ExprInfija.h"
#include <cctype>
#include <cmath>
#include <cassert>
#include "Lexico.h"
#include "NamedOperand.h"
#include "Variable.h"
#include "OpBinario.h"
#include <cstdio>

//! @brief Operador de asignación.
ExprInfija &ExprInfija::operator=(const ExprInfija &other)
  {
    Expresion::operator =(other);
    op_bin_en_espera= other.op_bin_en_espera;
    return *this;
  }

//! @brief Operador de asignación.
ExprInfija &ExprInfija::operator=(const std::string &str)
  {
    Expresion::operator=(str);
    //op_bin_en_espera.erase(op_bin_en_espera.begin(),op_bin_en_espera.end());
    assert(op_bin_en_espera.empty());
    return *this;
  }

//! @brief Inserta la señal que se pasa como parámetro.
void ExprInfija::InsertaPalabra(const std::streampos &pos,Segnal *t)
  {
    const std::string &nmb_palabra= t->getName();
    if(Inicial())
      if(t->GetClase() == op_binario)
        {
          if(dynamic_cast<OpBinario *>(t)->GetPosicion()!=prefija) //Posición ilegal.
            {
              std::cerr << "ExprInfija::InsertaPalabra; error, pos: " << pos
                        << " operador binario: '" << nmb_palabra << "' en posicion ilegal." << std::endl;
              err_traduc= true;
            }
          else //Operador binario prefijo.
            op_bin_en_espera.push(nmb_palabra);
        }
      else
        PonerSegnal(t);
    else
      if(t->GetClase() != op_binario)
        {
          std::cerr << "ExprInfija::InsertaPalabra; error, pos: " << pos
                    << " se esperaba un operador binario, se obtuvo: '" << nmb_palabra << "'." << std::endl;
          err_traduc= true;
        }
      else
        PonerSegnal(t);
  }

//! @brief Inserta la palabra que se pasa como parámetro.
Segnal *ExprInfija::InsertaNuevaPalabra(const std::streampos &pos,const std::string &palabra)
  {
    NamedOperand *t= NULL;
    if(!Inicial())
      {
        std::cerr << "ExprInfija::InsertaNuevaPalabra; error, pos: " << pos
                  << " a un operando sigue inmediatamente un )"
                  << " u otro operando" << std::endl;
        err_traduc= true;
      }
    else
      {
        t= NuevaVar(palabra);
        PonerSegnal(t);
      }
    return t;
  }

//! @brief Inserta la palabra que se pasa como parámetro.
Segnal *ExprInfija::InsertaNeg(const std::streampos &pos,char op)
  {
    Segnal *t= NULL;
    if(op == '+')
      ;
    else
      if(op == '-')
         {
           t= CalcularDireccion("neg");
           PonerSegnal(t);
         }
      else
        {
          std::cerr << "ExprInfija::InsertaNeg; error, posicion: " << pos 
                    << " operador binario en posicion ilegal." << std::endl;
          err_traduc= true;
        }
    return t;
  }

//! @brief Inserta el operador unario contenido en la señal que se pasa como parámetro.
bool ExprInfija::InsertaOpUnario(const std::streampos &pos,Segnal *t)
  {
    if(!Inicial())
      return false;
    if(t->GetClase() == parent_dcho)
      {
        std::cerr << "ExprInfija::InsertaOpUnario; error, posición: " << pos
                  << " posición ilegal para cerrar parentesis" << std::endl;
        err_traduc= true;
      }
    else if(t->GetClase() == op_binario)    //Un operador binario es ilegal en esta
      t= InsertaNeg(pos,t->getName()[0]); //posición, debe ser unario.
    else
      PonerSegnal(t);
    return true;
  }

//! @brief Inserta el operador binario contenido en la señal que se pasa como parámetro.
void ExprInfija::InsertaOpBinario(const std::streampos &pos,Segnal *t)
  {
    if( (t->GetClase() == parent_dcho) || (t->GetClase() == op_binario) )
      PonerSegnal(t);
    else
      {
        std::cerr << "ExprInfija::InsertaOpBinario; error, posicion: " << pos
                  << " se esperaba operador binario o )." 
                  << " se obtuvo: " << t->getName() << std::endl;
        err_traduc= true;
      }
  }

void ExprInfija::CompruebaParentesis( const std::streampos &pos, int &cta, const Segnal *t)
  {
    if(t->GetClase() == parent_izdo)
      cta++;
    else
      if(t->GetClase() == parent_dcho)
        {
          cta--;
          if(cta < 0)
            {
              std::cerr << "ExprInfija::CompruebaParentesis; error, posición: " << pos
                        << " Hay más paréntesis derechos"
                        << " que izquierdos." << std::endl;
              err_traduc= true;
            }
        }
  }
std::streampos ExprInfija::EncontrarPalabra(const std::streampos &pos,std::istream &is)
  {
    if(is.eof()) return pos;
    Variable s;
    is >> s;
    Segnal *t= NULL;
    const std::string &nmb= s.getName();
    if((t= CalcularDireccion(nmb)))
      InsertaPalabra(pos,t);
    else
      t= InsertaNuevaPalabra(pos,nmb);
    return is.tellg();
  }

std::streampos ExprInfija::EncontrarNumero(const std::streampos &pos,std::istream &is)
  {
    if(is.eof()) return pos;
    if(!Inicial())
      {
        std::cerr << "ExprInfija::CompruebaParentesis; error, constante en posicion ilegal. pos=" << pos << std::endl;
        err_traduc= true;
        return pos;
      }
    else
      {
        double x;
        is >> x;
        PonerDouble(x);
        return is.tellg();
      }
  }

//! @brief Devuelve verdadero si el carácter corresponde a un símbolo.
inline bool es_simbolo(const char &c)
  { return (ispunct(c) && (c!='.')); }

//! @brief Devuelve verdadero si los dos caracteres corresponden a un símbolo.
inline bool son_simbolo(const char &c1,const char &c2)
  { return ( ((c1=='<') || (c1=='>') || (c1=='!') || (c1=='=')) && (c2=='=') ); }


//! @brief Inserta el operador correspondiente a un símbolo (+,1,*,>,<,>=,...).
std::streampos ExprInfija::EncontrarSimbolo(const std::streampos &pos,std::istream &is,int &cta_parent)
  {
    std::streampos retval(pos);
    if(is.eof()) return pos;
    std::string name;
    //Leemos el primer carácter del símbolo.
    char c; is >> c; retval= is.tellg();
    if(es_simbolo(c)) name+= c;
    // Probamos si el símbolo es de dos caracteres.
    c= is.peek();
    if(son_simbolo(name[0],c)) //Parece símbolo.
      {
        is >> c; //Leemos el carácter del stream.
        name+= c;
        retval= is.tellg();
      }
    Segnal *t= CalcularDireccion(name);
    if(!t)
      {
        if(name==",") //La coma separa los argumentos de un operador binario prefijo (max,min,...)
          {
            assert(!op_bin_en_espera.empty()); //No debe estar vacío.
            t= CalcularDireccion(op_bin_en_espera.top());
            InsertaOpBinario(pos,t);
            op_bin_en_espera.pop();
          }
        else
          {
            std::cerr << "ExprInfija::EncontrarSimbolo; el símbolo: '" << name << "' es desconocido." << std::endl;
            err_traduc= true;
            return retval;
          }
      }
    else
      if(!InsertaOpUnario(pos,t)) InsertaOpBinario(pos,t);
    CompruebaParentesis(pos,cta_parent,t);
    return retval;
  }

void print_entrada(std::ostream &os,std::istream &is)
  {
    is.seekg(0, std::ios::beg); //Al principio.
    is.clear();
    while(!is.eof())
      os << is.get();
  }

//! @brief Convierte la cadena de caracteres contenida en el istream en una expresión.
void ExprInfija::Traduce(std::istream &is)
  {
    err_traduc= false;
    if(is.eof()) return;
    borra_lista();
    int cta_parent= 0;
    std::streampos pos= 0;
    std::string carac_leidos; //Caracteres leídos.
    char c=' ';
    while(true)
      {
        c= is.peek();
        if(c==EOF) break;
        carac_leidos+= c;
        if(isspace(c))
          {
            c= is.get(); //Pasamos por alto los blancos,tabuladores,retornos de carro,avances de línea o de página.
            pos= is.tellg();
          }
        else
          {
            if(isalpha(c))
              pos= EncontrarPalabra(pos,is);
            else if(isdigit(c) || (c == '.'))
              pos= EncontrarNumero(pos,is);
            else
              pos= EncontrarSimbolo(pos,is,cta_parent);
            if(err_traduc) break;
          }
        if(is.eof()) break;
      }
    if(err_traduc)
      {
        std::cerr << "ExprInfija::Traduce; error al leer caracteres: '"
                  << carac_leidos << "'" << std::endl;
      }
    //En este punto, posicion ha llegado al final del stream
    if(cta_parent != 0)
      {
        std::cerr << "ExprInfija::Traduce; error, paréntesis en expresión: '"
                  << carac_leidos << "'" << std::endl;
        err_traduc= true;
      }
    if(Inicial())
      {
        std::cerr << "ExprInfija::Traduce; error, la expresión de entrada: '"
                  << carac_leidos << "'" << std::endl;
        err_traduc= true;
      }
    if(!op_bin_en_espera.empty())
      {
        std::cerr << "ExprInfija::Traduce; error, quedaron operadores binarios"
                  << " prefijos sin leer en la expresión de entrada: '"
                  << carac_leidos << "'" << std::endl;
        err_traduc= true;
      }
  }
