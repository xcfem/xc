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
//vtkLabeledShStrMapper.h
//Modification of vtkLabeledDataMapper to display short text strings
//(the must fit in the memory space for a double).

#ifndef __vtkLabeledShStrMapper_h
#define __vtkLabeledShStrMapper_h

#include "vtkMapper2D.h"

class vtkDataSet;
class vtkTextMapper;
class vtkTextProperty;

class VTK_RENDERING_EXPORT vtkLabeledShStrMapper : public vtkMapper2D
{
public:
  // Description:
  // Instantiate object with %%-#6.3g label format. By default, point ids
  // are labeled.
  static vtkLabeledShStrMapper *New();

  vtkTypeRevisionMacro(vtkLabeledShStrMapper,vtkMapper2D);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Set/Get the field data array to label. This instance variable is
  // only applicable if field data is labeled.
  vtkSetClampMacro(FieldDataArray,int,0,VTK_LARGE_INTEGER);
  vtkGetMacro(FieldDataArray,int);

  // Description:
  // Set the input dataset to the mapper. This mapper handles any type of data.
  virtual void SetInput(vtkDataSet*);
  vtkDataSet *GetInput();

  // Description:
  // Set/Get the text property.
  virtual void SetLabelTextProperty(vtkTextProperty *p);
  vtkGetObjectMacro(LabelTextProperty,vtkTextProperty);

  // Description:
  // Draw the text to the screen at each input point.
  void RenderOpaqueGeometry(vtkViewport* viewport, vtkActor2D* actor);
  void RenderOverlay(vtkViewport* viewport, vtkActor2D* actor);

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  virtual void ReleaseGraphicsResources(vtkWindow *);


protected:
  vtkLabeledShStrMapper();
  ~vtkLabeledShStrMapper();

  vtkDataSet *Input;
  vtkTextProperty *LabelTextProperty;

  int   FieldDataArray;

  vtkTimeStamp BuildTime;

  inline int &NLabels()
    { return NumberOfLabels; }
  inline int &NLabelsAlloc()
    { return NumberOfLabelsAllocated; }

private:
  int NumberOfLabels;
  int NumberOfLabelsAllocated;
  vtkTextMapper **TextMappers;

  virtual int FillInputPortInformation(int, vtkInformation*);

private:
  vtkLabeledShStrMapper(const vtkLabeledShStrMapper&);  // Not implemented.
  void operator=(const vtkLabeledShStrMapper&);  // Not implemented.

public:
  inline const int &NLabels() const
    { return NumberOfLabels; }
  inline const int &NLabelsAlloc() const
    { return NumberOfLabelsAllocated; }

};

#endif

