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
//vtkShStrArray.h

#ifndef vtkShStrArray_h
#define vtkShStrArray_h

#include "vtkDoubleArray.h"

const size_t bytes_str= sizeof(double);

class VTK_COMMON_EXPORT vtkShStrArray : public vtkDoubleArray
  {
private:
  //ETX
  vtkShStrArray(const vtkShStrArray&) {}  // Not implemented.
  void operator=(const vtkShStrArray&) {}  // Not implemented.

  union str_dbl
    {
      char c[bytes_str];
      double d;
      str_dbl(const double &dd= 0.0);
      str_dbl(const std::string &);
      str_dbl(const char &);
      std::string get_string(void) const;
    };

  // Description:
  // Set the tuple value at the ith location in the array.
  void SetTupleValue(vtkIdType i, const double *) {} //Not implemented.

  // Description:
  // Insert (memory allocation performed) the tuple into the ith location
  // in the array.
  void InsertTupleValue(vtkIdType i, const double* ) {} //Not implemented.

  // Description:
  // This method lets the user specify data to be held by the array.  The
  // array argument is a pointer to the data.  size is the size of
  // the array supplied by the user.  Set save to 1 to keep the class
  // from deleting the array when it cleans up or reallocates memory.
  // The class uses the actual array provided; it does not copy the data
  // from the suppled array.
  void SetArray(double* array, vtkIdType size, int save)
    { vtkDoubleArray::SetArray(array, size, save); }

public:
  static vtkShStrArray *New();
  vtkTypeRevisionMacro(vtkShStrArray,vtkDoubleArray);

  static std::string dbl_to_str(const double &d);


  void PrintSelf(ostream& os, vtkIndent indent);



  // Description:
  // Get the data at a particular index.
  std::string GetValue(vtkIdType id);

  // Description:
  // Set the data at a particular index. Does not do range checking. Make sure
  // you use the method SetNumberOfValues() before inserting data.
  void SetValue(vtkIdType id, const std::string &value);

  // Description:
  // Insert data at a specified position in the array.
  void InsertValue(vtkIdType id,const std::string &);

  // Description:
  // Insert data at the end of the array. Return its location in the array.
  vtkIdType InsertNextValue(const std::string &);


protected:
  vtkShStrArray(vtkIdType numComp=1);
  ~vtkShStrArray(void);

};

#endif
