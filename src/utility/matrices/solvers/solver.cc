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
//solver.cc

#include "solver.h"

void Solver::inic_p(void)
  { 
    PP= m_sizet(n,1);
    for(size_t i= 1;i<= n;i++) P(i)= i;
  }
int Solver::check_pp(const size_t n_rows) const
  {
    if(PP.getNumberOfRows()!= n_rows || PP.getNumberOfColumns()!=1)
      {
        if(verbosity) std::cerr << "Matrix " << PP
				<< "has invalid dimensions."
			        << std::endl;
        return 0;
      }
    return 1;
  }
