//----------------------------------------------------------------------------
//  biblioteca vtk_aux; utilidades construidas sobre VTK (<http://www.vtk.org>)
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
//vtkShStrArray.cxx

#include "vtkShStrArray.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkShStrArray, "$Revision: 0.1 $")
vtkStandardNewMacro(vtkShStrArray)

vtkShStrArray::str_dbl::str_dbl(const double &dd)
  :d(dd) {}

vtkShStrArray::str_dbl::str_dbl(const std::string &src)
  : d(0.0)
  {
    const size_t sz= std::min(src.length(),bytes_str-1);
    size_t i= 0;
    for(;i<sz;i++)
      c[i]= src[i];
    c[i]= '\0';
  }

vtkShStrArray::str_dbl::str_dbl(const char &car)
  : d(0.0)
  {
    c[0]= car;
    c[1]= '\0';
  }

std::string vtkShStrArray::str_dbl::get_string(void) const
  { return std::string(c); }

//----------------------------------------------------------------------------
vtkShStrArray::vtkShStrArray(vtkIdType numComp)
  : vtkDoubleArray(numComp)
  {}

//----------------------------------------------------------------------------
vtkShStrArray::~vtkShStrArray(void)
  {}


std::string vtkShStrArray::dbl_to_str(const double &d)
  {
    str_dbl tmp(d);
    return tmp.get_string();
  }

// Description:
// Get the data at a particular index.
std::string vtkShStrArray::GetValue(vtkIdType id)
  { return dbl_to_str(vtkDoubleArray::GetValue(id)); }

  // Description:
  // Set the data at a particular index. Does not do range checking. Make sure
  // you use the method SetNumberOfValues() before inserting data.
void vtkShStrArray::SetValue(vtkIdType id, const std::string &value)
  {
    str_dbl tmp(value);
    vtkDoubleArray::SetValue(id, tmp.d);
  }

// Description:
// Insert data at a specified position in the array.
void vtkShStrArray::InsertValue(vtkIdType id,const std::string &f)
  {
    str_dbl tmp(f);
    vtkDoubleArray::InsertValue(id, tmp.d);
  }

// Description:
// Insert data at the end of the array. Return its location in the array.
vtkIdType vtkShStrArray::InsertNextValue(const std::string &f)
  {
    str_dbl tmp(f);
    return vtkDoubleArray::InsertNextValue(tmp.d);
  }

//----------------------------------------------------------------------------
void vtkShStrArray::PrintSelf(ostream& os, vtkIndent indent)
  { vtkDoubleArray::PrintSelf(os,indent); }
