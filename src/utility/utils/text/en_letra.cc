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
//en_letra.cc
//Utilidades para conversión entre números y letras.

#include "en_letra.h"
#include "boost/lexical_cast.hpp"
#include <cmath>

std::string num2str(const size_t &n)
  //Devuelve la cadena de caracteres que representa al número.
  {
    return boost::lexical_cast<std::string>(n);
  }

//! @brief Devuelve una lista con los digitos del entero.
const dq_sint &el_decomp(const unsigned long &i)
  {
    static dq_sint retval;
    retval.clear();
    unsigned long val= i;
    do
      {
        retval.push_front(val % 10);
        val /= 10;
      }
    while(val != 0);
    return retval;
  }

const std::string &ldbl2str_ip(const long double &d)
  {
    long double intpart;
    std::modf(d,&intpart);
    static std::string retval;
    retval= boost::lexical_cast<std::string>(intpart); //num2str(intpart,0);
    return retval;
  }

const std::string &ldbl2str_fp(const long double &d,int precision)
  {
    long double intpart,fracpart;
    fracpart= std::modf(d,&intpart);
    static std::string retval;
    retval=num2str(fracpart,precision);
    size_t inicio= retval.find('.');
    if(inicio>retval.length())
      retval= "";
    else
      retval= retval.substr(inicio+1,retval.length());
    size_t sz= retval.size();
    for(int i=sz;i<precision;i++) retval+='0';
    return retval;
  }

const dq_sint &el_decomp(const std::string &str)
  {
    static dq_sint retval;
    retval.clear();
    if(str.length()<1) return retval;
    char c;
    size_t i= 0;
    do
      {
        c= str[i]; i++;
        short int val= (short int) c - (short int)('0');
        retval.push_back(val);
      }
    while((c!='.') && i<str.length());
    return retval;
  }

const std::string &el_unidades(const unsigned short int &i,const bool &genero)
     //Convierte a letra las unidades.     
     //0 genero masculino.
     //1 genero femenino.
  {
    static std::string retval;
    retval.clear();
    switch(i)
      {
      case 0:
        break;
      case 1:
        retval= "un";
        if(genero)
          retval+= 'a';
        else
          retval+= 'o';
        break;
      case 2:
        retval= "dos"; break;
      case 3:
        retval= "tres"; break;
      case 4:
        retval= "cuatro"; break;
      case 5:
        retval= "cinco"; break;
      case 6:
        retval= "seis"; break;
      case 7:
        retval= "siete"; break;
      case 8:
        retval= "ocho"; break;
      case 9:
        retval= "nueve"; break;
      default:
        break;
      }
    return retval;
  }

//! @brief Convierte a letra las unidades.     
const std::string &el_decenas( const unsigned short int &dec,
                   const unsigned short int &uni,
                   const bool &genero)
  {
    const std::string unidades= el_unidades(uni,genero);
    static std::string retval;
    retval.clear();
    switch(dec)
      {
      case 0:
        retval= unidades;
        break;
      case 1:
        switch(uni)
          {
          case 0:
            retval= "diez";
            break;
          case 1:
            retval= "once";
            break;
          case 2:
            retval= "doce";
            break;
          case 3:
            retval= "trece";
            break;
          case 4:
            retval= "catorce";
            break;
          case 5:
            retval= "quince";
            break;
          default:
            retval= ("dieci" + unidades);
            break;
          }
      case 2:
        if(uni==0)
          retval= "veinte";
        else
          retval= "veinti" + unidades;
        break;
      case 3:
        if(uni==0)
          retval= "treinta";
        else
          retval= "treinta y " + unidades;
        break;
      case 4:
        if(uni==0)
          retval= "cuarenta";
        else
          retval= "cuarenta y " + unidades;
        break;
      case 5:
        if(uni==0)
          retval= "cincuenta";
        else
          retval= "cincuenta y " + unidades;
        break;
      case 6:
        if(uni==0)
          retval= "sesenta";
        else
          retval= "sesenta y " + unidades;
        break;
      case 7:
        if(uni==0)
          retval= "setenta";
        else
          retval= "setenta y " + unidades;
        break;
      case 8:
        if(uni==0)
          retval= "ochenta";
        else
          retval= "ochenta y " + unidades;
        break;
      case 9:
        if(uni==0)
          retval= "noventa";
        else
          retval= "noventa y " + unidades;
        break;
      }
    return retval;
  }

const std::string &el_centenas( const unsigned short int &cen,
                    const unsigned short int &dec,
                    const unsigned short int &uni,
                    const bool &genero)
     //Convierte a letra las unidades.
  {
    std::string decenas= el_decenas(dec,uni,genero);
    std::string plural="os";
    if(genero) plural= "as";
    static std::string retval;
    retval.clear();
    switch(cen)
      {
      case 0:
        retval= decenas;
        break;
      case 1:
        if(dec==0 && uni==0)
          retval= "cien";
        else
          retval= "ciento " + decenas;
        break;
      case 2:
        retval= "doscient" + plural + ' ' + decenas;
        break;
      case 3:
        retval= "trescient" + plural + ' ' + decenas;
        break;
      case 4:
        retval= "cuatrocient" + plural + ' ' + decenas;
        break;
      case 5:
        retval= "quinient" + plural + ' ' + decenas;
        break;
      case 6:
        retval= "seiscient" + plural + ' ' + decenas;
        break;
      case 7:
        retval= "setecient" + plural + ' ' + decenas;
        break;
      case 8:
        retval= "ochocient" + plural + ' ' + decenas;
        break;
      case 9:
        retval= "novecient" + plural + ' ' + decenas;
        break;
      }
    return retval;
  }

class terna: public dq_sint
  {
  public:
    const std::string &en_letra(const bool &genero) const
      {
        static const std::string nula="";
        switch(size())
          {
          case 0:
            return nula;
          case 1:
            return el_unidades((*this)[0],genero);
          case 2:
            return el_decenas((*this)[0],(*this)[1],genero);
          case 3:
            return el_centenas((*this)[0],(*this)[1],(*this)[2],genero);
          }
        return nula;
      }
    unsigned int valor(void) const
      {
        switch(size())
          {
          case 0:
            return 0;
          case 1:
            return (*this)[0];
          case 2:
            return (*this)[0]*10+(*this)[1];
          case 3:
            return (*this)[0]*100+(*this)[1]*10+(*this)[2];
          }
        return 0;
      }
    const std::string en_humano(void) const
      {
         static std::string retval;
         retval.clear();
         for(const_iterator i=begin();i!=end();i++)
           retval+= char(*i+48);
         return retval;
      }
    friend std::ostream &operator<<(std::ostream &os,const terna &t)
      {
        for(const_iterator i=t.begin();i!=t.end();i++)
          os << *i;
        os << ": " << t.en_letra(true);
        return os;
      }
  };



class Ternas: public std::deque<terna>
  {
    typedef std::deque<terna> dq_ternas;

    void el_ternas(const dq_sint &d)
      {
        for(dq_sint::const_reverse_iterator i= d.rbegin();i!=d.rend();i++)
          {
            terna t;
            t.push_front(*i); i++;
            if(i != d.rend())
              {
                t.push_front(*i); i++;
                if(i!= d.rend())
                  t.push_front(*i);
              }
            push_front(t);
            if(i==d.rend()) break;
          }
      }
    static std::string capa_uno(const std::string &str)
      {
        if(str.substr(str.length()-3,str.length()) == "uno")
          return str.substr(0,str.length()-1);
        else
          return str;
      }
    std::string sexteto_en_letra(const size_t &ind,const bool &genero) const
      {
        if((*this)[ind].valor() == 0)
          return (*this)[ind+1].en_letra(genero);
        else
          if((*this)[ind].valor() == 1)
            return " mil " + (*this)[ind+1].en_letra(genero);
          else
            {
              std::string retval = capa_uno((*this)[ind].en_letra(genero));
              retval+= " mil " + (*this)[ind+1].en_letra(genero);;
              return retval;
            }
      }
  public:
    Ternas(const unsigned long &i)
      { el_ternas(el_decomp(i)); }
    Ternas(const long double &d,const bool &i= true,int precision= 16)
      //i: verdadero si han de ser las ternas de la parte entera
      //El signo ha de gestionarse aparte.
      { 
        if(precision==0) return;
        if(i)
          el_ternas(el_decomp_ip(fabs(d)));
        else
          el_ternas(el_decomp_fp(fabs(d),precision));
      }
    const std::string &en_letra(const bool &genero) const
      {
        static std::string retval;
        retval.clear();
        switch(size())
          {
          case 4:
            retval= capa_uno(sexteto_en_letra(0,false));
            retval+= " millones " + sexteto_en_letra(2,genero);
            break;
          case 3:
            if((*this)[0].valor()==1)
              retval= "un millón ";
            else
              {
                retval= capa_uno((*this)[0].en_letra(false));
                retval+= " millones ";
	      }
            retval+= sexteto_en_letra(1,genero);
            break;
          case 2:
            retval= sexteto_en_letra(0,genero);
            break;
          case 1:
            retval= (*this)[0].en_letra(genero);
            break;
          case 0:
            break;
          }
        return retval;
      }
    const std::string &en_humano(const char &sep_miles= '.') const
      //Si el separador de miles es NUL (\0) no se pone
      {
        static std::string retval;
        retval.clear();
        if(size()<1) return retval;
        Ternas::const_iterator i= begin();
        retval= (*i).en_humano(); i++;
        for(;i!=end();i++)
          {
            if(sep_miles!='\0') retval+= '.';
            retval+= (*i).en_humano();
          }
        return retval;
      }
    friend std::ostream &operator<<(std::ostream &os, const Ternas &t)
      {
        for(Ternas::const_iterator i= t.begin();i!=t.end();i++)
          os << *i << std::endl;
        return os;
      }
  };

const std::string &en_letra(const unsigned long &i,const bool &genero)
  {
    Ternas t(i);
    return t.en_letra(genero);
  }

const std::string &pon_coletilla(const std::string &str, const std::string &coletilla)
  {
    static std::string retval;
    retval.clear();
    if((coletilla == " céntimos") && (get_car_d(str,3) =="uno"))
      retval= (q_car_d(str,'o') + coletilla);
    else
      retval= str + coletilla;
    return retval;
  }

const std::string &en_letra(const long double &d,int num_dec,const bool &genero,const std::string &sep_dec, const std::string &coletilla)
  {
    double dd(d);
    if((num_dec==0) && (fp(dd)>=0.5)) dd+=1.0;
    Ternas tip(dd,true),tfp(dd,false,num_dec);
    std::string str_ip= tip.en_letra(genero);
    if(tfp.size() && (fp(dd)>0.0))
      if(str_ip != "")
        return pon_coletilla(str_ip + sep_dec + tfp.en_letra(genero),coletilla);
      else
        return pon_coletilla(tfp.en_letra(genero),coletilla);
    else
      return tip.en_letra(genero);
  }

const std::string &en_humano(const unsigned long &i)
  {
    Ternas t(i);
    return t.en_humano();
  }

const std::string &en_humano(const long double &d,int num_dec,const char &sep_miles,const char &sep_dec,const bool &con_signo)
  {
    double dd(d);
    static std::string retval;
    retval.clear();
    if(dd<0)
      retval+="-";
    else
      if(con_signo) retval+="+";
    if((num_dec==0) && (fp(dd)>=0.5)) dd+=1.0;
    Ternas tip(dd,true),tfp(dd,false,num_dec);
    retval+= tip.en_humano();
    if((tfp.size())) retval+= (sep_dec + tfp.en_humano('\0'));
    return retval;
  }

