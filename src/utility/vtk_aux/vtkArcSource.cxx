//----------------------------------------------------------------------------
//  biblioteca vtk_aux; utilidades construidas sobre VTK (<http://www.vtk.org>)
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
//vtkArcSource.cxx

#include "vtkArcSource.h"

#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"

//vtkCxxRevisionMacro(vtkArcSource, "$Revision: 1.2 $")
vtkStandardNewMacro(vtkArcSource)

vtkArcSource::vtkArcSource(void)
  : InitAng(0.0), FinalAng(2.0*vtkMath::DoublePi())
  {
    GeneratePolygon= 0;
    Px[0]= 1.0; Px[1]= 0.0; Px[2]= 0.0;
  }

void vtkArcSource::GetNormal(double n[3])
  {
    // Make sure the polygon normal is a unit vector
    n[0] = this->Normal[0];
    n[1] = this->Normal[1];
    n[2] = this->Normal[2];
    if( vtkMath::Normalize(n) == 0.0 )
      {
        n[0] = 0.0;
        n[1] = 0.0;
        n[2] = 1.0;
      }
  }

void vtkArcSource::SetPx(double v[3])
  { SetPx(v[0],v[1],v[2]); }

void vtkArcSource::SetPx(const double &v0,const double &v1,const double &v2)
  {
    Px[0]= v0; Px[1]= v1; Px[2]= v2;
    if(!checkPx())
      std::cerr << "vtkArcSource::SetPx; error al asignar el vector." << std::endl;
  }

bool vtkArcSource::checkPx(void)
  {
    bool retval= false;
    double n[3];
    GetNormal(n);
    if(vtkMath::Normalize(Px) >1.0E-3)
      {
        double py[3];
        vtkMath::Cross(Px,n,py); //created two orthogonal axes in the polygon plane, Px & py
        if(vtkMath::Normalize(py) > 1.0E-3)
          retval= true;
      }
    return retval;
  }

void vtkArcSource::SearchPlaneVector(const double n[3])
  {
    if(!checkPx())
      {
        double axis[3];
        // Cross with unit axis vectors and eventually find vector in the polygon plane
        bool foundPlaneVector= false;
        axis[0] = 1.0;
        axis[1] = 0.0;
        axis[2] = 0.0;
        vtkMath::Cross(n,axis,Px);
        if( vtkMath::Normalize(Px) > 1.0E-3 )
          { foundPlaneVector= true; }
        if(!foundPlaneVector)
          {
            axis[0] = 0.0;
            axis[1] = 1.0;
            axis[2] = 0.0;
            vtkMath::Cross(n,axis,Px);
            if(vtkMath::Normalize(Px) > 1.0E-3 )
              {foundPlaneVector= true;}
          }
        if(!foundPlaneVector)
          {
            axis[0] = 0.0;
            axis[1] = 0.0;
            axis[2] = 1.0;
            vtkMath::Cross(n,axis,Px);
            vtkMath::Normalize(Px);
          }
      }
  }

int vtkArcSource::RequestData( vtkInformation *vtkNotUsed(request),vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector)
  {
    // Get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // Get the output
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    // We only produce one piece
    if(outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()) > 0)
      { return 1; }

    double x[3], r[3];
    int i, j, numPts=this->NumberOfSides;
    vtkPoints *newPoints; 
    vtkCellArray *newLine;
  
    // Prepare to produce the output; create the connectivity array(s)
    newPoints = vtkPoints::New();
    newPoints->Allocate(numPts);

    if(this->GeneratePolyline )
      {
        newLine = vtkCellArray::New();
        newLine->Allocate(newLine->EstimateSize(1,numPts));
        newLine->InsertNextCell(numPts+1);
        for(i=0;i<=numPts;i++)
          { newLine->InsertCellPoint(i); }
        //newLine->InsertCellPoint(0); //close the polyline
        output->SetLines(newLine);
        newLine->Delete();
      }
  
    if(this->GeneratePolygon )
      {
	std::cerr << __FUNCTION__
	          << "polygon generation not implemented." << std::endl;
//    vtkCellArray *newPoly;
//         newPoly = vtkCellArray::New();
//         newPoly->Allocate(newPoly->EstimateSize(1,numPts));
//         newPoly->InsertNextCell(numPts);
//         for(i=0; i<numPts; i++)
//           { newPoly->InsertCellPoint(i); }
//         output->SetPolys(newPoly);
//         newPoly->Delete();
      }
  
    // Produce a unit vector in the plane of the polygon (i.e., perpendicular
    // to the normal)
    double n[3], py[3];

    GetNormal(n);
    SearchPlaneVector(n);

    vtkMath::Cross(Px,n,py); //created two orthogonal axes in the polygon plane, Px & py

    // Now run around normal vector to produce polygon points.
    const double angBarrido= FinalAng-InitAng;
    const double incTheta= angBarrido/numPts;
    double theta= InitAng;
    for(j=0; j<numPts+1; j++)
      {
        for(i=0; i<3; i++)
          {
            r[i] = Px[i]*cos(theta) + py[i]*sin(theta);
            x[i] = this->Center[i] + this->Radius * r[i];
          }
        newPoints->InsertNextPoint(x);
        theta+= incTheta;
      }
    output->SetPoints(newPoints);
    newPoints->Delete();
    return 1;
  }

void vtkArcSource::PrintSelf(ostream& os, vtkIndent indent)
  {
    this->Superclass::PrintSelf(os,indent);
    os << indent << "Initial angle: " << this->InitAng << "\n";
    os << indent << "Final angle: " << this->FinalAng << "\n";
  }
