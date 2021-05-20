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
//vtkArcSource.h
// vtkArcSource is a source object that creates a single
// n-sided polyline inscribed in an arc of circle.

#ifndef __vtkArcSource_h
#define __vtkArcSource_h

#include "vtkRegularPolygonSource.h"

class VTK_GRAPHICS_EXPORT vtkArcSource : public vtkRegularPolygonSource 
  {
  protected:
    double InitAng;
    double FinalAng;
    double Px[3]; //Vector unitario en la dirección del x axis.

    void GetNormal(double v[3]);
    bool checkPx(void);
  public:
    // Description:
    // Standard methods for instantiation, obtaining type and printing instance values.
    static vtkArcSource *New();
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Set/Get the initAng of the polyline. By default, the radius is set to 0.5.
    vtkSetMacro(InitAng,double);
    vtkGetMacro(InitAng,double);

    // Description:
    // Set/Get the finalAng of the polyline. By default, the radius is set to 0.5.
    vtkSetMacro(FinalAng,double);
    vtkGetMacro(FinalAng,double);

    void SetPx(double v[3]);
    void SetPx(const double &v0,const double &v1,const double &v2);
  protected:
    vtkArcSource();
    ~vtkArcSource() {}

    void SearchPlaneVector(const double n[3]);
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  private:
    vtkArcSource(const vtkArcSource&);  // Not implemented.
    void operator=(const vtkArcSource&);  // Not implemented.
  };

#endif
