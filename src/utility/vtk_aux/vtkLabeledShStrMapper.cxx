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
//vtkLabeledShStrMapper.cxx

#include "vtkLabeledShStrMapper.h"

#include "vtkExecutive.h"
#include "vtkInformation.h"
#include "vtkActor2D.h"
#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkShStrArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include <assert.h>

vtkCxxRevisionMacro(vtkLabeledShStrMapper, "$Revision: 1.40 $")
vtkStandardNewMacro(vtkLabeledShStrMapper)

vtkCxxSetObjectMacro(vtkLabeledShStrMapper,LabelTextProperty,vtkTextProperty)

//----------------------------------------------------------------------------
// Creates a new label mapper

vtkLabeledShStrMapper::vtkLabeledShStrMapper()
{
  this->Input = nullptr;

  this->FieldDataArray = 0;
 
  this->NLabels() = 0;
  this->NLabelsAlloc() = 50;

  this->TextMappers = new vtkTextMapper * [this->NLabelsAlloc()];
  for (int i=0; i<this->NLabelsAlloc(); i++)
    {
    this->TextMappers[i] = vtkTextMapper::New();
    }

  this->LabelTextProperty = vtkTextProperty::New();
  this->LabelTextProperty->SetFontSize(12);
  this->LabelTextProperty->SetBold(1);
  this->LabelTextProperty->SetItalic(1);
  this->LabelTextProperty->SetShadow(1);
  this->LabelTextProperty->SetFontFamilyToArial();
}

//----------------------------------------------------------------------------
vtkLabeledShStrMapper::~vtkLabeledShStrMapper()
  {
    if(this->TextMappers != nullptr )
      {
        for (int i=0; i < this->NLabelsAlloc(); i++)
          { this->TextMappers[i]->Delete(); }
        delete [] this->TextMappers;
      }
    this->SetLabelTextProperty(nullptr);
  }

//----------------------------------------------------------------------------
void vtkLabeledShStrMapper::SetInput(vtkDataSet *input)
  {
    if (input)
      { this->SetInputConnection(0, input->GetProducerPort()); }
    else
      { this->SetInputConnection(0, 0); } // Setting a nullptr input removes the connection.
  }

//----------------------------------------------------------------------------
// Specify the input data or filter.
vtkDataSet *vtkLabeledShStrMapper::GetInput()
  { return vtkDataSet::SafeDownCast(this->GetExecutive()->GetInputData(0, 0)); }

//----------------------------------------------------------------------------
// Release any graphics resources that are being consumed by this mapper.
void vtkLabeledShStrMapper::ReleaseGraphicsResources(vtkWindow *win)
  {
    if(this->TextMappers != nullptr )
      {
        for (int i=0; i < this->NLabelsAlloc(); i++)
          { this->TextMappers[i]->ReleaseGraphicsResources(win); }
      }
  }

//----------------------------------------------------------------------------
void vtkLabeledShStrMapper::RenderOverlay(vtkViewport *viewport, 
                                         vtkActor2D *actor)
  {
    int i;
    double x[3];
    vtkDataSet *input=this->GetInput();

    if(!input)
      {
        vtkErrorMacro(<<"Need input data to render labels");
        return;
      }
    for(i=0; i<this->NLabels(); i++)
      {
        input->GetPoint(i,x);
        actor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
        actor->GetPositionCoordinate()->SetValue(x);
        this->TextMappers[i]->RenderOverlay(viewport, actor);
      }
  }

//----------------------------------------------------------------------------
void vtkLabeledShStrMapper::RenderOpaqueGeometry(vtkViewport *viewport, 
                                                vtkActor2D *actor)
  {
    int i,numComp = 0, pointIdLabels;
    double x[3];
    vtkDataSet *input=this->GetInput();

    if(!input)
      {
        vtkErrorMacro(<<"Need input data to render labels");
        return;
      }

    vtkPointData *pd=input->GetPointData();
    vtkTextProperty *tprop = this->LabelTextProperty;
    if(!tprop)
      {
        vtkErrorMacro(<<"Need text property to render labels");
        return;
      }

    input->Update();

    // Check to see whether we have to rebuild everything
    if ( this->GetMTime() > this->BuildTime || 
         input->GetMTime() > this->BuildTime ||
         tprop->GetMTime() > this->BuildTime)
      {
        vtkDebugMacro(<<"Rebuilding labels");

        // figure out what to label, and if we can label it
        pointIdLabels = 0;

        int arrayNum = (this->FieldDataArray < pd->GetNumberOfArrays() ?
                        this->FieldDataArray : pd->GetNumberOfArrays() - 1);
        vtkDataArray *data = pd->GetArray(arrayNum);

        // determine number of components and check input
        if(pointIdLabels)
          {
            ;
          }
        else
          if(data)
            {
              numComp = data->GetNumberOfComponents();
              assert(numComp==1);
            }
          else
            {
              vtkErrorMacro(<<"Need input data to render labels");
              return;
            }

        this->NLabels() = input->GetNumberOfPoints();
        if(this->NLabels() > this->NLabelsAlloc() )
          {
            // delete old stuff
            for (i=0; i < this->NLabelsAlloc(); i++)
              { this->TextMappers[i]->Delete(); }
            delete [] this->TextMappers;

            this->NLabelsAlloc() = this->NLabels();
            this->TextMappers = new vtkTextMapper * [this->NLabelsAlloc()];
            for(i=0; i<this->NLabelsAlloc(); i++)
              { this->TextMappers[i] = vtkTextMapper::New(); }
          }//if we have to allocate new text mappers
    
        for (i=0; i < this->NLabels(); i++)
          {
            const double tmp= data->GetComponent(i,0);
            const std::string str= vtkShStrArray::dbl_to_str(tmp);
            this->TextMappers[i]->SetInput(str.c_str());
            this->TextMappers[i]->SetTextProperty(tprop);
          }

        this->BuildTime.Modified();
      }

    for (i=0; i<this->NLabels(); i++)
      {
        input->GetPoint(i,x);
        actor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
        actor->GetPositionCoordinate()->SetValue(x);
        this->TextMappers[i]->RenderOpaqueGeometry(viewport, actor);
      }
  }

//----------------------------------------------------------------------------
int vtkLabeledShStrMapper::FillInputPortInformation(int vtkNotUsed( port ), vtkInformation* info)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkShStrArray");
    return 1;
  }

//----------------------------------------------------------------------------
void vtkLabeledShStrMapper::PrintSelf(ostream& os, vtkIndent indent)
  {
    this->Superclass::PrintSelf(os,indent);

    if( this->Input )
      { os << indent << "Input: (" << this->Input << ")\n"; }
    else
      { os << indent << "Input: (none)\n"; }

    if(this->LabelTextProperty)
      {
        os << indent << "Label Text Property:\n";
        this->LabelTextProperty->PrintSelf(os,indent.GetNextIndent());
      }
    else
      { os << indent << "Label Text Property: (none)\n"; }

    os << indent << "Label Mode: ";
    os << "Label Strs\n";

    os << indent << "Field Data Array: " << this->FieldDataArray << "\n";
  }
