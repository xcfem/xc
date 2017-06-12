# -*- coding: utf-8 -*-
def VtkDibujaKPts(nmbActor):
  # Define el actor a emplear para dibujar kpoints.
  sphereKPts= vtk.vtkSphereSource()
  sphereKPts.SetRadius(0.03)
  sphereKPts.SetThetaResolution(5)
  sphereKPts.SetPhiResolution(5)
        
  markKPts= vtk.vtkGlyph3D()
  markKPts.SetInput(ugrid) 
  markKPts.SetSource(sphereKPts)
  markKPts.ScalingOff()
  markKPts.OrientOff()

        
  mappKPts= vtk.vtkPolyDataMapper()
  mappKPts.SetInput(markKPts)
  nmbActor= vtk.vtkActor()
  nmbActor.SetMapper(mappKPts)
  nmbActor.GetProperty().SetColor(.5, .7, .5)
        
  renderer.AddActor(nmbActor)
