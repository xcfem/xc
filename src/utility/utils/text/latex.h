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
//latex.h
//Secuencias para programación en LaTEX.

#ifndef LATEX_H
#define LATEX_H

#include <iostream>
#include <string>
#include <iomanip>
#include "utility/utils/streams/omanip.h"

//Caracteres especiales
std::string ascii2latex(const std::string &s);
//Devuelve el código equivalente el LaTex
//para los caracteres con significado especial


//Manipuladores para latex.

//Caracteres
const std::string ltx_porciento= "\\%";
const std::string ltx_ldots= "\\ldots";
inline std::ostream &ltx_symbol(std::ostream &os,const unsigned short int &s)
  { return os << "\\symbol{" << s << '}'; }
inline omanip<const unsigned short int &> ltx_symbol(const unsigned short int &s)
  { return omanip<const unsigned short int &>(&ltx_symbol,s); }

//Tipos de letra
const std::string ltx_tiny= "\\scriptsize";
const std::string ltx_scriptsize= "\\scriptsize";
const std::string ltx_small= "\\small";
const std::string ltx_normalsize= "\\normalsize";
const std::string ltx_large= "\\large";
inline std::ostream &ltx_textbf(std::ostream &os,const std::string &str)
  { return os << "\\textbf{" << str << '}'; }
inline std::ostream &ltx_emph(std::ostream &os,const std::string &str)
  { return os << "\\emph{" << str << '}'; }

inline omanip<const std::string &> ltx_textbf(const std::string &str)
  { return omanip<const std::string &>(&ltx_textbf,str); }
inline omanip<const std::string &> ltx_emph(const std::string &str)
  { return omanip<const std::string &>(&ltx_emph,str); }

//Entornos
inline std::ostream &ltx_begin(std::ostream &os,const std::string &str)
  { return os << "\\begin{" << str << '}'; }
inline std::ostream &ltx_end(std::ostream &os,const std::string &str)
  { return os << "\\end{" << str << '}'; }

inline omanip<const std::string &> ltx_begin(const std::string &str)
  { return omanip<const std::string &>(&ltx_begin,str); }
inline omanip<const std::string &> ltx_end(const std::string &str)
  { return omanip<const std::string &>(&ltx_end,str); }

//Varios
const std::string ltx_newpage= "\\newpage";
inline std::ostream &ltx_input(std::ostream &os,const std::string &str)
  { return os << "\\input{" << str << '}'; }

inline omanip<const std::string &> ltx_input(const std::string &str)
  { return omanip<const std::string &>(&ltx_input,str); }


//Estructura
const std::string ltx_parttoc= "\\parttoc";

inline std::ostream &ltx_part(std::ostream &os,const std::string &str)
  { return os << "\\part{" << str << '}'; }
inline std::ostream &ltx_star_part(std::ostream &os,const std::string &str)
  { return os << "\\part*{" << str << '}'; }
inline std::ostream &ltx_chapter(std::ostream &os,const std::string &str)
  { return os << "\\chapter{" << str << '}'; }
inline std::ostream &ltx_star_chapter(std::ostream &os,const std::string &str)
  { return os << "\\chapter*{" << str << '}'; }
inline std::ostream &ltx_section(std::ostream &os,const std::string &str)
  { return os << "\\section{" << str << '}'; }
inline std::ostream &ltx_subsection(std::ostream &os,const std::string &str)
  { return os << "\\subsection{" << str << '}'; }
inline std::ostream &ltx_subsubsection(std::ostream &os,const std::string &str)
  { return os << "\\subsubsection{" << str << '}'; }
inline std::ostream &ltx_paragraph(std::ostream &os,const std::string &str)
  { return os << "\\paragraph{" << str << '}'; }

inline omanip<const std::string &> ltx_part(const std::string &str)
  { return omanip<const std::string &>(&ltx_part,str); }
inline omanip<const std::string &> ltx_star_part(const std::string &str)
  { return omanip<const std::string &>(&ltx_star_part,str); }
inline omanip<const std::string &> ltx_chapter(const std::string &str)
  { return omanip<const std::string &>(&ltx_chapter,str); }
inline omanip<const std::string &> ltx_star_chapter(const std::string &str)
  { return omanip<const std::string &>(&ltx_star_chapter,str); }
inline omanip<const std::string &> ltx_section(const std::string &str)
  { return omanip<const std::string &>(&ltx_section,str); }
inline omanip<const std::string &> ltx_subsection(const std::string &str)
  { return omanip<const std::string &>(&ltx_subsection,str); }
inline omanip<const std::string &> ltx_subsubsection(const std::string &str)
  { return omanip<const std::string &>(&ltx_subsubsection,str); }
inline omanip<const std::string &> ltx_paragraph(const std::string &str)
  { return omanip<const std::string &>(&ltx_paragraph,str); }

//Listas
const std::string ltx_beg_itemize= "\\begin{itemize}";
const std::string ltx_item= "\\item ";
const std::string ltx_end_itemize= "\\end{itemize}";

//Tablas
const std::string ltx_ampsnd= " & ";
const std::string ltx_fin_reg= " \\\\";
const std::string ltx_hline= "\\hline";
const std::string ltx_endhead= "\\endhead";
const std::string ltx_endfoot= "\\endfoot";
const std::string ltx_endlastfoot= "\\endlastfoot";
inline std::ostream &ltx_cline(std::ostream &os,const std::string &str)
  { return os << "\\cline{" << str << "}"; }
inline omanip<const std::string &> ltx_cline(const std::string &str)
  { return omanip<const std::string &>(&ltx_cline,str); }

inline std::string ltx_datos_multicolumn(const std::string &num_campos,const std::string &just,const std::string &text)
  { return ("{" + num_campos + "}{" + just + "}{" + text + "}"); }
inline std::ostream &ltx_multicolumn(std::ostream &os,const std::string &str)
  { return os << "\\multicolumn" << str; }

inline omanip<const std::string &> ltx_multicolumn(const std::string &str)
  { return omanip<const std::string &>(&ltx_multicolumn,str); }





#endif




