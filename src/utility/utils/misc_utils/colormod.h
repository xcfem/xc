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
//colormod.h
// Base on the post of Joel Sjögren in this stackoverflow discussion:
// https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal

#include <ostream>

namespace Color {
enum Code
  {
    FG_RED      = 31,
    FG_GREEN    = 32,
    FG_YELLOW   = 33,
    FG_BLUE     = 34,
    FG_DEFAULT  = 39,
    BG_RED      = 41,
    BG_GREEN    = 42,
    BG_BLUE     = 44,
    BG_DEFAULT  = 49
  };
  
class Modifier
  {
    Code code;
  public:
    Modifier(Code pCode) : code(pCode) {}
    friend std::ostream& operator<<(std::ostream& os, const Modifier& mod)
      { return os << "\033[" << mod.code << "m"; }
  };
  
static const Modifier red(Color::FG_RED);
static const Modifier green(Color::FG_GREEN);
static const Modifier blue(Color::FG_BLUE);
static const Modifier yellow(Color::FG_YELLOW);
static const Modifier def(Color::FG_DEFAULT);

} // end Color namespace.
