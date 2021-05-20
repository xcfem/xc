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
//vtkDoubleHeadedArrowSource.cc

#include "vtkDoubleHeadedArrowSource.h"

#include "vtkAppendPolyData.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

vtkCxxRevisionMacro(vtkDoubleHeadedArrowSource, "$Revision: 1.7 $")
vtkStandardNewMacro(vtkDoubleHeadedArrowSource)

vtkDoubleHeadedArrowSource::vtkDoubleHeadedArrowSource(void)
  {
    this->TipResolution = 6;
    this->TipRadius = 0.1;
    this->TipLength = 0.35;
    this->ShaftResolution = 6;
    this->ShaftRadius = 0.03;

    this->SetNumberOfInputPorts(0);
  }

int vtkDoubleHeadedArrowSource::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector)
  {
    // get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the ouptut
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    int piece, numPieces, ghostLevel;
    vtkCylinderSource *cyl = vtkCylinderSource::New();
    vtkTransform *trans0 = vtkTransform::New();
    vtkTransformFilter *tf0 = vtkTransformFilter::New();
    vtkConeSource *cone1 = vtkConeSource::New();
    vtkConeSource *cone2 = vtkConeSource::New();
    vtkTransform *trans1 = vtkTransform::New();
    vtkTransform *trans2 = vtkTransform::New();
    vtkTransformFilter *tf1 = vtkTransformFilter::New();
    vtkTransformFilter *tf2 = vtkTransformFilter::New();
    vtkAppendPolyData *append = vtkAppendPolyData::New();

    piece = output->GetUpdatePiece();
    numPieces = output->GetUpdateNumberOfPieces();
    ghostLevel = output->GetUpdateGhostLevel();

    cyl->SetResolution(this->ShaftResolution);
    cyl->SetRadius(this->ShaftRadius);
    cyl->SetHeight(1.0 - this->TipLength);
    cyl->SetCenter(0, (1.0-this->TipLength)*0.5, 0.0);
    cyl->CappingOn();

    trans0->RotateZ(-90.0);
    tf0->SetTransform(trans0);
    tf0->SetInput(cyl->GetOutput());

    cone1->SetResolution(this->TipResolution);
    cone1->SetHeight(this->TipLength);
    cone1->SetRadius(this->TipRadius);

    trans1->Translate(1.0-this->TipLength*0.5, 0.0, 0.0);
    tf1->SetTransform(trans1);
    tf1->SetInput(cone1->GetOutput());


    cone2->SetResolution(this->TipResolution);
    cone2->SetHeight(this->TipLength);
    cone2->SetRadius(this->TipRadius);

    trans2->Translate(1.0-this->TipLength*1.15, 0.0, 0.0);
    tf2->SetTransform(trans2);
    tf2->SetInput(cone2->GetOutput());

    append->AddInput(tf0->GetPolyDataOutput());
    append->AddInput(tf1->GetPolyDataOutput());
    append->AddInput(tf2->GetPolyDataOutput());

    if(output->GetUpdatePiece() == 0 && numPieces > 0)
      {
        append->Update();
        output->ShallowCopy(append->GetOutput());
      }
    output->SetUpdatePiece(piece);
    output->SetUpdateNumberOfPieces(numPieces);
    output->SetUpdateGhostLevel(ghostLevel);

    cone2->Delete();
    cone1->Delete();
    trans0->Delete();
    tf0->Delete();
    cyl->Delete();
    trans1->Delete();
    tf1->Delete();
    trans2->Delete();
    tf2->Delete();
    append->Delete();
    return 1;
  }

void vtkDoubleHeadedArrowSource::PrintSelf(ostream& os, vtkIndent indent)
  {
    this->Superclass::PrintSelf(os,indent);

    os << indent << "TipResolution: " << this->TipResolution << "\n";
    os << indent << "TipRadius: " << this->TipRadius << "\n";
    os << indent << "TipLength: " << this->TipLength << "\n";

    os << indent << "ShaftResolution: " << this->ShaftResolution << "\n";
    os << indent << "ShaftRadius: " << this->ShaftRadius << "\n";
  }
