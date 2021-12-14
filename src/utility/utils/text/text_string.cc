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
//text_string.cc

#include "text_string.h"
#include <algorithm>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/find.hpp>
#include <cctype>
#include "utility/utils/misc_utils/matem.h"

//! Ajusta una text string a la derecha dentro de other de
//! tamaño n rellena de characters 'c'.
const std::string &ajusta_dcha(const std::string &s,const size_t &n,const char &fill_char)
  {
    static std::string retval;
    retval= s;
    size_t l= s.length();
    if(l<n)
      retval= (std::string(n-l,fill_char)+s);
    else
      if(l>n)
        retval= s.substr(0,n);
    return retval;
  }

//! Ajusta una text string a la izquierda dentro de otra de
//! tamaño n rellena de characters 'c'.
const std::string &ajusta_izda(const std::string &s,const size_t &n,const char &fill_char)
  {
    static std::string retval;
    retval= s;
    size_t l= s.length();
    if(l<n)
      retval= (s+std::string(n-l,fill_char));
    else
      if(l>n)
        retval= s.substr(0,n);
    return retval;
  }

//! @brief Imprime los códigos ASCII de la text string.
void deletrea_ascii(const std::string &s,std::ostream &os)
  {
    for(std::string::const_iterator i= s.begin();i!=s.end();i++)
      os << int(*i) << ',';
  }

//! @brief Return the number of characters that are equal to C
//! in the string.
size_t num_car(const char *str,const char C)
  { return num_car(std::string(str),C); }

//! @brief Return the number of characters that are equal to C
//! in the string.
size_t num_car(const std::string &str,const char C)
  {
    size_t conta= 0;
    for(std::string::const_iterator i= str.begin();i!=str.end();i++)
      if((*i)==C) conta++;
    return conta;
  }


//! @brief Devuelve el número de characters C que hay a la izquierda de
//! la cadena.
int num_car_i(const char *str,const char C)
  { return num_car_i(std::string(str),C); }

//! @brief Devuelve el número de characters C que hay a la izquierda de
//! la cadena.
size_t num_car_i(const std::string &s,const char C)
  {
    size_t b=0;
    for(std::string::const_iterator i= s.begin();i!=s.end();i++,b++)
      if((*i)!=C) break;
    return b;
  }

//! @brief Devuelve el número de characters C que hay a la derecha de
//! la cadena.
int num_car_d(const char *str,const char C)
  { return num_car_d(std::string(str),C); }

//! @brief Devuelve el número de characters C que hay a la derecha de
//! la cadena.
int num_car_d(const std::string &s,const char C)
  {
    //Contamos los characters C a la derecha
    const size_t long_str= s.length();
    size_t num_car= 0;
    size_t i= long_str-1;
    while((i>0) && (s[i] == C))
      {
        i--;
        num_car++;
      }
    return num_car;
  }

//! @brief Desplaza los characters n posiciones a la izquierda.
char *shift_i(char *str,int n)
  {
    const size_t long_str= strlen(str);
    size_t i;
    for(i=0;(n+i)<=long_str;i++) str[i]= str[n+i];
    str[i+1]= '\0';
    return str;    
  }

//! @brief Desplaza los characters n posiciones a la izquierda.
const std::string &shift_i(const std::string &s,int n)
  { 
    static std::string retval;
    retval= s;
    retval.replace(0,n,"");
    return retval;
  }

//! @brief Quita los characters C a la derecha
char *q_car_d(char *str,const char C)
  {
    if(str)
      {
        const size_t long_str= strlen(str);
        size_t num_car= num_car_d(str);
        if(num_car) str[long_str-num_car+1]= '\0';
      }
    return str;
  }

//! @brief Quita los characters C a la derecha
const std::string &q_car_d(const std::string &s,const char C)
  {
    static std::string retval;
    retval= s;
    if(!retval.empty())
      {
        const size_t long_str= retval.length();
        size_t num_car= num_car_d(s,C);
        if(num_car) retval.replace(long_str-num_car,s.length()-1,"");
      }
    return retval;
  }

//! @brief Elimina los characters C.
const std::string &elimina_car(const std::string &s,const char C)
  {
    static std::string retval;
    retval.clear();
    for(std::string::const_iterator i=s.begin();i!=s.end();i++)
      if(*i!=C) retval+=*i;
    return retval;
  }

//! @brief Sustituye en la propia cadena el carácter v
//! por el n.
void replace_in_situ(std::string &s,char v,char n)
  {
    std::string::iterator i;
    for(i=s.begin();i!=s.end(); i++)
      {
        if((*i)==v)
          (*i)= n;
      }
  }

//! @brief Devuelve la cadena que resulta de sustituir el carácter v
//! por el n en la que se le pasa como parámetro.
const std::string &replace(const std::string &s,char v,char n)
  {
    static std::string retval;
    retval.clear();
    std::string::const_iterator i;
    for(i=s.begin();i!=s.end(); i++)
      {
        if((*i)!=v)
          retval+= *i;
        else
          retval+= n;
      }
    return retval;
  }

//! @brief Devuelve la cadena que resulta de copiar la que se le 
//! pasa como parámetro hasta encontrar el carácter delim tantas veces
//! como cont.
const std::string &copia_hasta(const std::string &s,char delim,size_t cont)
  {
    size_t c= 0;
    static std::string retval;
    retval.clear();
    std::string::const_iterator i;
    for(i=s.begin();i!=s.end(); i++)
      {
        if((*i)!=delim)
          retval+= *i;
        else
          {
            c++;
            if(c==cont) break;
          }
      }
    return retval;
  }

//! @brief Devuelve la cadena que resulta de copiar la que se le pasa
//! como parámetro desde la posición siguiente a la de la primera ocurrencia
//! del carácter delim.
const std::string &copia_desde(const std::string &s,char delim)
  {
    static std::string retval;
    retval.clear();
    std::string::const_iterator i;
    for(i=s.begin();i!=s.end(); i++)
      if((*i)==delim) break;
    if(i!=s.end())
      {
        i++;
        for(std::string::const_iterator j= i;j!=s.end();j++)
        retval+=*j;
      }
    return retval;
  }

//! @brief Devuelve la cadena que resulta de copiar
//! la que se le pasa como parámetro hasta
//! encontrar alguno de los characters de delim.
const std::string &copia_hasta(const std::string &s,const std::string &delim)
  {
    static std::string retval;
    retval.clear();
    std::string::const_iterator i;
    for(i=s.begin();i!=s.end(); i++)
      {
        if(delim.find(*i)> delim.length())
          retval+= *i;
        else
          break;
      }
    return retval;
  }

//! @brief Devuelve los n últimos characters.
const std::string &get_car_d(const std::string &s,const size_t &n)
  {
    static std::string retval;
    retval.clear();
    const size_t long_str= s.length();
    const size_t lim= long_str-1;
    size_t i= lim-n;
    while((i<lim))
      {
        i++;
        retval+= s[i];
      }
    return retval;
  }

//! @brief Devuelve verdadero si la cadena representa un entero.
int isInteger(const std::string &s)
  {
    int  i;

    const int len= s.length() ;
    if(!len) return false;

    char c= s[0] ;
    if(c=='+' || c=='-')
      i=1;
    else
      if(isdigit(c))
	i=0 ;
      else
	return false;
    for(;i<len;i++)
      if(!isdigit(s[i]))
	return false;
    return true ;
  }

//! Devueleve verdadero si s representa un entero en coma flotante.
//! s puede tener signo (opcional), una parte entera, un punto y una parte decimal
//! (ambas opcionales), y un exponente (opcional). El exponente
//! debe llevar 'e' or 'E', el signo (opcional) y números.
int isNumber(const std::string &s)
  {
    const size_t len= s.length();
    if(!len) return false;

    char c= s[0]; // look for a sign
    size_t i=0;
    if(c=='+' || c=='-')
     i=1;
    else
      if(isdigit(c) || (c=='.')) //O dígito o punto decimal.
	i=0;
      else
	return false;

    for(;i<len;i++)              // process integer part
      if(!isdigit(c=s[i]))
        {
	  if(c=='.')
	    goto decimal;
	  else if(c=='e' || c=='E')
	    goto exponent;
	  else
	    return false;
        }
    return true;

  decimal :
    for(i=i+1;i<len;i++)
      if(!isdigit(c=s[i]))
        {
	  if(c=='e' || c=='E')
	    goto exponent;
	  else
	    return false;
        }
    return true;

  exponent :
    c = s[i+1];
    if(c=='+'|| c=='-') i++;
    if(i == len-1) return false;
    for(i=i+1; i<len; i++)
      if(!isdigit(s[i])) return false;
    return true;
  }

//! @brief Devuelve el número de llaves characters de apertura '{[«' menos el de llaves
//! cierre '}]»' en la cadena que se pasa como parámetro'}' 
int get_balance_characters(const std::string &s,const char &abre,const char &cierra)
  {
    int retval= 0;
    for(std::string::const_iterator i= s.begin();i!=s.end();i++)
      {
        if(*i == abre) retval++;
        if(*i == cierra) retval--;
      }
    return retval;
  }


// //! Function taken from Kernighan & Ritchie, p64. Implemen-
// //! ted for portability (the PC function itoa(int,char*,int) is not ANSI
// //! C). Returns s, the std::string conversion of n.
// const std::string &itos(int n)
//   {
//     int  i,j,sign;
//     char c;

//     if(n == 1) return "1";
//     if(n == 2) return "2";
//     static std::string s;
//     s.clear();

//     if((sign=n)<0) n = -n;
//     i = 0;
//     do
//       s[i++] = n % 10 + '0';
//     while ((n/=10) > 0);
//     if(sign < 0)
//       s[i++] = '-';
//     s[i] = '\0';

//     // reverse 's'
//     j=strlen(s.c_str()) - 1;
//     for(i=0; i<j; i++,j--)
//       {
//         c    = s[i];
//         s[i] = s[j];
//         s[j] = c;
//       }
//     return s;
//   }

//! Devuelve verdadero si la text string representa un número entero.
bool es_entero(const std::string &str)
  {
    if(str.length()<1) return false;
    for(std::string::const_iterator i=str.begin();i!=str.end();i++)
      {
        if(isalpha(*i)) return false;
        if(ispunct(*i)) return false;
        if(isspace(*i)) return false;
      }
    return true;
  }

//! Devuelve verdadero si la text string representa un número real.
bool es_real(const std::string &str)
  {
    if(str.length()<1) return false;
    for(std::string::const_iterator i=str.begin();i!=str.end();i++)
      {
        const char c= *i;
        if(isalpha(c))
          {
            if((c=='E') || (c=='e'))
              continue;
            else
              return false;
          }
        if(ispunct(c))
          {
            if(c=='.') continue;
            if(c=='+') continue;
            if(c=='-') continue;
            return false;
          }
        if(isspace(c)) return false;
      }
    return true;
  }

struct ToLower
  {
    char operator() (char c) const  { return std::tolower(c); }
  };

struct ToUpper
  {
    char operator() (char c) const  { return std::toupper(c); }
  };


//! @brief Devuelve la text string transformada a mayúsculas.
const std::string &tolower(const std::string &str)
  { return a_minusculas(str); }

//! @brief Devuelve la text string transformada a minúsculas.
const std::string &toupper(const std::string &str)
  { return a_mayusculas(str); }

//! @brief Devuelve la text string transformada a minúsculas.
const std::string &a_minusculas(const std::string &str)
  {
    static std::string retval;
    retval= str;
    transform(retval.begin(),retval.end(),retval.begin(),ToLower());
    return retval;
  }

//! @brief Devuelve la text string transformada a mayúsculas.
const std::string &a_mayusculas(const std::string &str)
  {
    static std::string retval;
    retval= str;
    transform(retval.begin(),retval.end(),retval.begin(),ToUpper());
    return retval;
  }

//! @brief Devuelve un objeto ios_base::openMode a partir de la text string que se pasa como parámetro.
std::ios_base::openmode string_to_openmode(const std::string &str)
  {
    if(str == "app")
      return std::ios_base::app;
    else if(str == "ate")
      return std::ios_base::ate;
    else if(str == "binary")
      return std::ios_base::binary;
    else if(str == "in")
      return std::ios_base::in;
    else if(str == "out")
      return std::ios_base::out;
    else if(str == "trunc")
      return std::ios_base::trunc;
    else
      {
        std::cerr << "string_to_openmode; no se reconoce el modo: " << str << std::endl;
        return std::ios_base::out;
      }
  }

//Parsing.

//! @brief Separa una cadena en subcadenas separadas por uno de los characters
//! que se pasan en el parámetro sep.
//!
//! @param str: text string a dividir.
//! @param sp: Characters que separan campos.
const std::deque<std::string> &separa_escaped_cadena(const std::string &str,const std::string &sp)
  {
    typedef boost::tokenizer<boost::escaped_list_separator<char> >  tokenizer;
    static std::deque<std::string> retval;
    retval.clear();
    if(!str.empty())
      {
        boost::escaped_list_separator<char> sep("\\",sp.c_str(),"\"");
        tokenizer tokens(str, sep);
        for(tokenizer::const_iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter)
          retval.push_back(*tok_iter);
      }
    return retval;
  }

//! @brief Separa una cadena en subcadenas separadas por uno de los characters
//! que se pasan en el parámetro sep.
//!
//! @param str: text string a dividir.
//! @param sp: Characters que separan campos.
const std::deque<std::string> &separa_cadena(const std::string &str,const std::string &sp)
  {
    typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
    static std::deque<std::string> retval;
    retval.clear();
    boost::char_separator<char> sep(sp.c_str());
    tokenizer tokens(str, sep);
    for(tokenizer::const_iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter)
      retval.push_back(*tok_iter);
    return retval;
  }


//! @brief Separa una cadena en subcadenas separadas por uno de los characters
//! que se pasan en el parámetro sep. Además comprueba que el número de argumentos coindide
//! con el valor sz que se pasa como parámetro.
//!
//! @param str: text string a dividir.
//! @param sp: Characters que separan campos.
//! @param sz: Number of expected fieldsr (if zero don't check it).
const std::deque<std::string> &separa_cadena(const std::string &str,const std::string &sp,const size_t &sz)
  {
    static std::deque<std::string> retval;
    retval= separa_cadena(str,sp);
    const size_t tam= retval.size();
    if(sz>0)
      {
        if(tam < sz)
          std::cerr << "separa_cadena: ¡Ojo!, la cadena: '" << str << "' tiene menos de " << sz << " campos." << std::endl;
        if(tam > sz)
          std::cerr << "separa_cadena: ¡Ojo!, la cadena: '" << str << "' tiene más de " << sz << " campos." << std::endl;
      }
    return retval;
  }


//! @brief Separa una cadena en dos subcadenas separadas por la primera
//! ocurrencia del carácter que se pasa como parámetro.
const std::deque<std::string> &split_at_first(const std::string &str,const char &c)
  {
    const size_t pos= str.find_first_of(c);
    static std::deque<std::string> retval;
    retval.clear();
    if(pos == std::string::npos) //no se encotró el carácter separador.
      retval.push_back(str);
    else
      {
        retval.push_back(str.substr(0,pos));
	retval.push_back(str.substr(pos+1));
      }
    return retval;
  }

//! @brief Separa los campos de una text string CSV (comma separated values).
//!
//! @param str: text string a dividir.
const std::deque<std::string> &separa_csv(const std::string &str)
  {
    typedef boost::tokenizer<boost::escaped_list_separator<char> >  tokenizer;
    static std::deque<std::string> retval;
    retval.clear();
    tokenizer tok(str);
    for(tokenizer::const_iterator beg=tok.begin(); beg!=tok.end();++beg)
      retval.push_back(*beg);
    return retval;
  }

//! @brief Devuelve los sumandos de la text string (A +B + C + ...) que se pasa como parámetro.
const std::deque<std::string> &separa_sumandos(const std::string &str)
  {
    static std::deque<std::string> retval;
    retval.clear();
    if(has_char(str,'+')) //Tiene sumandos.
      { retval= separa_cadena(str,"+"); }
    else
      if(!str.empty()) retval.push_back(str);
    for(std::deque<std::string>::iterator i= retval.begin();i!=retval.end();i++)
      (*i)= q_blancos(*i); 
    return retval;
  }


//! Receive a text string of type: function_name(arg1,arg2,arg3,...)
//!
//! and return the following text strings;
//! - retval[0]= function_name
//! - retval[1]= arg1,arg2,arg3,...
std::deque<std::string> getfuncargs(const std::string &str)
  {
    static std::deque<std::string> retval;
    retval= separa_cadena(str,"(",2);
    if(retval.size()>1)
      boost::algorithm::trim_right_if(retval[1],boost::algorithm::is_any_of(") ")); //Quitamos paréntesis derecho y espacios.
    if(retval.size()>2)
      std::cerr << "Error when splitting function and arguments." << std::endl;
    return retval;
  }

//! Receive a text string like:
//
//! function_name(arg1,arg2,arg3,...)
//!
//! and return the text strings:
//! - retval[0]= function_name
//! - retval[1]= arg1
//! - retval[2]= arg2
//! - retval[3]= arg3
//! - ...
//!
const std::deque<std::string> &getargs(const std::string &str)
  {
    std::deque<std::string> tmp= getfuncargs(str);
    static std::deque<std::string> retval;
    retval.clear();
    if(tmp.empty()) return retval;
    retval.push_back(tmp[0]);
    if(tmp.size()<2) return retval;
    typedef boost::tokenizer<boost::escaped_list_separator<char> > tknzr;
    tknzr tok(tmp[1]);
    for(tknzr::iterator beg=tok.begin(); beg!=tok.end();++beg)
      retval.push_back(*beg);
    return retval;
  }

//! Receive a text string like:
//
//! name[v1,v2,v3,...]
//!
//! and return the text strings:
//! - retval[0]= name
//! - retval[1]= v1,v2,v3,...
//!
const std::deque<std::string> &getnmbindices(const std::string &str)
  {
    static std::deque<std::string> retval;
    retval= separa_cadena(str,"[",2);
    if(retval.size()>1)
      boost::algorithm::trim_right_if(retval[1],boost::algorithm::is_any_of("] ")); //Quitamos paréntesis derecho y espacios.
    if(retval.size()>2)
      std::cerr << "Error splitting name and indexes." << std::endl;
    return retval;
  }

//!  Receive a text string like:
//
//! name[v1,v2,v3,...]
//!
//! and return the text strings:
//! - retval[0]= name
//! - retval[1]= v1
//! - retval[2]= v2
//! - retval[3]= v3
//! - ...
const std::deque<std::string> &getindices(const std::string &str)
  {
    std::deque<std::string> tmp= getnmbindices(str);
    static std::deque<std::string> retval;
    retval.clear();
    if(tmp.empty()) return retval;
    retval.push_back(tmp[0]);
    if(tmp.size()<2) return retval;
    typedef boost::tokenizer<boost::escaped_list_separator<char> > tknzr;
    tknzr tok(tmp[1]);
    for(tknzr::iterator beg=tok.begin(); beg!=tok.end();++beg)
      retval.push_back(*beg);
    return retval;
  }

//! @brief Return the number of quotation marks (without escape)
//! in the argument string.
size_t cuenta_comillas(const std::string &str)
  {
    size_t retval= 0;
    const size_t sz= str.size();
    char c;
    bool escape= false;
    for(size_t i= 0;i<sz;i++)
      {
        c= str[i];
        if(c=='\\') //Escape.
          {
            escape= true;
            continue;
          }
        if(c=='\"')
          {
            if(!escape)
              retval++;
	  }
        escape= false;
      }
    return retval;
  }

//! @brief Devuelve verdadero si la cadena es correcta.
bool check_comillas(const std::string &str)
  {
    bool retval= par(cuenta_comillas(str));
    if(!retval)
      std::cerr << "check_comillas; error sintáctico en la cadena: '" << str
                << "' no se han cerrado comillas." << std::endl;
    return retval;
  }

//! @brief Coloca un escape delante de los paréntesis para
//! evitar que el intérprete los tome en cuenta.
const std::string &protege_signos(const std::string &str)
  {
    static std::string retval;
    retval.clear();
    const size_t sz= str.size();
    char c;
    bool escape= false;
    for(size_t i= 0;i<sz;i++)
      {
        c= str[i];
        if(c=='\\') //Escape.
          {
            escape= true;
            continue;
          }
        if(c=='\"' || c=='(' || c==')' ||
           c=='[' || c==']')
          {
            if(!escape)
              retval+= '\\';
	  }
        retval+= c;
        escape= false;
      }
    return retval;
  }
  
int saldo_characters(const std::string &str,const char &abre,const char &cierra)
  {
    int retval= 0;
    const size_t sz= str.size();
    char c;
    bool escape= false;
    for(size_t i= 0;i<sz;i++)
      {
        c= str[i];
        if(c=='\\') //Escape.
          {
            escape= true;
            continue;
          }
        else if(c==abre)
          {
            if(!escape)
              retval++;
	  }
        else if(c==cierra)
          {
            if(!escape)
              retval--;
	  }
        escape= false;
      }
    return retval;
  }

int saldo_parentesis(const std::string &str)
  { return saldo_characters(str,'(',')'); }

int saldo_corchetes(const std::string &str)
  { return saldo_characters(str,'[',']'); }

bool check_parentesis(const std::string &str)
  {
    const int saldo= saldo_parentesis(str);
    bool retval= (saldo==0);
    if(!retval)
      {
        std::cerr << "Error sintáctico en la cadena: '" << str;
        if(saldo>0)
	  std::cerr << "' no se ha(n) cerrado " << saldo << " paréntesis." << std::endl;
        else
	  std::cerr << "' no se ha(n) abierto " << -saldo << " paréntesis." << std::endl;
      }
    return retval;
  }

bool check_corchetes(const std::string &str)
  {
    const size_t saldo= saldo_corchetes(str);
    bool retval= (saldo==0);
    if(!retval)
      {
        std::cerr << "Error sintáctico en la cadena: '" << str;
        if(saldo>0)
	  std::cerr << "' no se ha(n) cerrado " << saldo << " corchete(s)." << std::endl;
        else
	  std::cerr << "' no se ha(n) abierto " << -saldo << " corchete(s)." << std::endl;
      }
    return retval;
  }

//! @brief Devuelve una text string comprendida entre paréntesis, por.
//! ejemplo si la cadena es 'AAB(C(DEF)J)KL' y el iterador apunta al primer paréntesis (al
//! que está detrás de la B) devolverá 'C(DEF)J'
//! @param iter: Iterador apuntando al paréntesis que abre el bloque.
const std::string &get_bloque_parentesis(std::string::const_iterator &iter,const std::string::const_iterator &end)
  {
    static std::string retval;
    retval.clear();
    size_t cuenta_parent(1);
    if(*iter != '(')
      {
        std::cerr << "El iterador no abre paréntesis." << std::endl;
        return retval;
      }
    retval+= *iter; //Primer paréntesis.
    while(cuenta_parent && (iter!=end))
      {
        iter++;
        const char c= *iter;
        if(c == '(') cuenta_parent++;
        if(c == ')') cuenta_parent--;
        retval+= c;
      }
    return retval;
  }

//! @brief Devuelve una text string comprendida entre paréntesis, por.
//! ejemplo si la cadena es 'AAB(C(DEF)J)KL' y el iterador apunta al primer paréntesis (al
//! que está detrás de la B) devolverá 'C(DEF)J'
//! @param iter: Índice apuntando al paréntesis que abre el bloque.
const std::string &get_bloque_parentesis(const std::string &str,const size_t &iter,const size_t &end)
  {
    static std::string retval;
    retval.clear();
    size_t cuenta_parent(1);
    if(str[iter] != '(')
      {
        std::cerr << "El iterador no abre paréntesis." << std::endl;
        return retval;
      }
    retval+= str[iter]; //Primer paréntesis.
    size_t conta(iter);
    while(cuenta_parent && (conta<end))
      {
        conta++;
        if(str[conta] == '(') cuenta_parent++;
        if(str[conta] == ')') cuenta_parent--;
        retval+= str[conta];
        if(!cuenta_parent) break;
      }
    return retval;
  }

//! @brief Devuelve una text string comprendida entre corchetes, por.
//! ejemplo si la cadena es 'AAB[C[DEF]J]KL' y el iterador apunta al primer corchetes [al
//! que está detrás de la B] devolverá 'C[DEF]J'
//! @param iter: Iterador apuntando al corchete que abre el bloque.
const std::string &get_bloque_corchetes(std::string::const_iterator &iter,const std::string::const_iterator &end)
  {
    static std::string retval;
    retval.clear();
    size_t cuenta_parent(1);
    if(*iter != '[')
      {
        std::cerr << "El iterador no abre corchetes." << std::endl;
        return retval;
      }
    retval+= *iter; //Primer corchete.
    while(cuenta_parent && (iter!=end))
      {
        iter++;
        const char c= *iter;
        if(c == '[') cuenta_parent++;
        if(c == ']') cuenta_parent--;
        retval+= c;
      }
    return retval;
  }

//! @brief Devuelve una text string comprendida entre corchetes, por.
//! ejemplo si la cadena es 'AAB[C[DEF]J]KL' y el iterador apunta al primer corchetes [al
//! que está detrás de la B] devolverá 'C[DEF]J'
//! @param iter: Iterador apuntando al corchete que abre el bloque.
const std::string &get_bloque_corchetes(const std::string &str,const size_t &iter,const size_t &end)
  {
    static std::string retval;
    retval.clear();
    size_t cuenta_parent(1);
    if(str[iter] != '[')
      {
        std::cerr << "El iterador no abre corchetes." << std::endl;
        return retval;
      }
    retval+= str[iter]; //Primer paréntesis.
    size_t conta(iter);
    while(cuenta_parent && (conta<end))
      {
        conta++;
        if(str[conta] == '[') cuenta_parent++;
        if(str[conta] == ']') cuenta_parent--;
        retval+= str[conta];
        if(!cuenta_parent) break;
      }
    return retval;
  }
