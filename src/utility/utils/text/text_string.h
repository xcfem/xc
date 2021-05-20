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
//text_string.h
//Utilidades para text strings.

#ifndef TEXT_STRING_H
#define TEXT_STRING_H

#include <string>
#include <iostream>
#include <deque>

const std::string &ajusta_dcha(const std::string &s,const size_t &n,const char &fill_char= ' ');

const std::string &ajusta_izda(const std::string &s,const size_t &n,const char &fill_char= ' ');

void deletrea_ascii(const std::string &s,std::ostream &os);

size_t num_car(const char *str,const char C);
size_t num_car(const std::string &str,const char C);

int num_car_i(const char *str,const char C= ' ');
//! @brief Devuelve el número de blancos a la izquierda.
inline int num_blancos_i(const char *str)
  { return num_car_i(str,' '); }

size_t num_car_i(const std::string &s,const char C= ' ');

inline size_t num_blancos_i(const std::string &s)
  { return num_car_i(s,' '); }

int num_car_d(const char *str,const char C= ' ');
inline int num_blancos_d(const char *str)
  { return num_car_d(str,' '); }
int num_car_d(const std::string &s,const char C= ' ');
inline int num_blancos_d(const std::string &s)
  { return num_car_d(s,' '); }

//! @brief Desplaza los characters n posiciones a la izquierda.
char *shift_i(char *str,int n);
//! @brief Desplaza los characters n posiciones a la izquierda.
const std::string &shift_i(const std::string &s,int n);

//! @brief Quita los characters C a la izquierda de la cadena.
inline char *q_car_i(char *str,const char C= ' ')
  { return shift_i(str,num_car_i(str,C)); }
//! @brief Quita los blancos a la izquierda de la cadena.
inline char *q_blancos_i(char *str)
  { return q_car_i(str,' '); }
//! @brief Quita el retorno de carro a la izquierda de la cadena.
inline char *q_CR_i(char *str)
  { return q_car_i(str,'\n'); }
inline std::string q_car_i(const std::string &s,const char C= ' ')
  { return shift_i(s,num_car_i(s,C)); }
inline std::string q_blancos_i(const std::string &s)
  { return q_car_i(s,' '); }
inline std::string q_CR_i(const std::string &s)
  { return q_car_i(s,'\n'); }

char *q_car_d(char *str,const char C= ' ');
inline char *q_blancos_d(char *str)
  { return q_car_d(str,' '); }
inline char *q_CR_d(char *str)
  { return q_car_d(str,'\n'); }
const std::string &q_car_d(const std::string &s,const char C= ' ');
inline std::string q_blancos_d(const std::string &s)
  { return q_car_d(s,' '); }
inline std::string q_CR_d(const std::string &s)
  { return q_car_d(s,'\n'); }

const std::string &elimina_car(const std::string &s,const char C);

//! @brief Quita los characters C a izquierda y derecha de la cadena.
inline char *q_car(char *str,const char C= ' ')
  { return q_car_d(q_car_i(str,C),C); }

inline char *q_blancos(char *str)
  { return q_car(str,' '); }

//! @brief Quita los characters C a izquierda y derecha de la cadena.
inline std::string q_car(const std::string &s,const char C= ' ')
  { return q_car_d(q_car_i(s,C),C); }

inline std::string q_blancos(const std::string &s)
  { return q_car(s,' '); }



void replace_in_situ(std::string &s,char v,char n);
const std::string &replace(const std::string &s,char v,char n);
const std::string &copia_hasta(const std::string &s,char delim,size_t cont= 1);
//Devuelve la cadena que resulta de copiar
//la que se le pasa como parámetro hasta
//encontrar el carácter delim tantas veces
//como cont.
const std::string &copia_desde(const std::string &s,char delim);
//Devuelve la cadena que resulta de copiar
//la que se le pasa como parámetro desde la posición
//siguiente a la del carácter delim.
const std::string &copia_hasta(const std::string &s,const std::string &delim);
//Devuelve la cadena que resulta de copiar
//la que se le pasa como parámetro hasta
//encontrar el alguno de los characters de delim.

const std::string &get_car_d(const std::string &s,const size_t &n);
//Devuelve los n últimos characters.

//! @brief Devuelve el número de  characters de apertura '{[«' menos el de characters de
//! cierre '}]»' en la cadena que se pasa como parámetro.
int get_balance_characters(const std::string &s,const char &abre,const char &cierra);
//! @brief Devuelve el número de llaves de apertura '{' menos el de llaves de
//! cierre '}' en la cadena que se pasa como parámetro.
inline int get_balance_llaves(const std::string &s)
  { return get_balance_characters(s,'{','}'); }
//! @brief Devuelve el número de corchetes de apertura '[' menos el de corchetes de
//! cierre ']' en la cadena que se pasa como parámetro.
inline int get_balance_corchetes(const std::string &s)
  { return get_balance_characters(s,'[',']'); }

int isInteger(const std::string &s);
int isNumber(const std::string &s);
inline int atoi(const std::string &s)
  { return ::atoi(s.c_str()); }
inline double atof(const std::string &s)
  { return ::atof(s.c_str()); }

inline bool has_char(const std::string &s,const char &c)
  { return (s.find(c,0)!= std::string::npos); }

//const std::string &itos(int n);

bool es_entero(const std::string &str);
//Devuelve verdadero si la text string representa un número entero.

bool es_real(const std::string &str);
//Devuelve verdadero si la text string representa un número real.

inline bool es_numero(const std::string &str)
//Devuelve verdadero si la text string representa un número real o entero.
  { return es_real(str); }

const std::string &tolower(const std::string &);
const std::string &toupper(const std::string &);
const std::string &a_minusculas(const std::string &);
const std::string &a_mayusculas(const std::string &);

std::ios_base::openmode string_to_openmode(const std::string &str);

//Parsing.
size_t cuenta_comillas(const std::string &);
bool check_comillas(const std::string &);
int saldo_parentesis(const std::string &);
bool check_parentesis(const std::string &);
int saldo_corchetes(const std::string &);
bool check_corchetes(const std::string &);
const std::string &get_bloque_parentesis(std::string::const_iterator &iter,const std::string::const_iterator &end);
const std::string &get_bloque_corchetes(std::string::const_iterator &iter,const std::string::const_iterator &end);
const std::string &get_bloque_parentesis(const std::string &str,const size_t &iter,const size_t &end);
const std::string &get_bloque_corchetes(const std::string &str,const size_t &iter,const size_t &end);
const std::string &protege_signos(const std::string &);

const std::deque<std::string> &separa_escaped_cadena(const std::string &,const std::string &);
const std::deque<std::string> &separa_cadena(const std::string &str,const std::string &sp);
const std::deque<std::string> &separa_cadena(const std::string &str,const std::string &sp,const size_t &sz);
const std::deque<std::string> &split_at_first(const std::string &str,const char &c);
const std::deque<std::string> &separa_csv(const std::string &str);
const std::deque<std::string> &separa_sumandos(const std::string &str);
const std::deque<std::string> &getargs(const std::string &str);
const std::deque<std::string> &getnmbindices(const std::string &);
const std::deque<std::string> &getindices(const std::string &);

#endif
