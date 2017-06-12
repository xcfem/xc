# -*- coding: utf-8 -*-

def VtkDibujaElementos1D(nmbActor):
  # Define el actor a emplear para dibujar elementos.
  ugridMapper= vtk.vtkDataSetMapper()
  ugridMapper.SetInput(ugrid)
  nmbActor= vtk.vtkActor()
  nmbActor.SetMapper(ugridMapper)
  nmbActor.GetProperty().SetColor(0,0,0)
  nmbActor.SetRepresentationToWireframe()
  renderer.AddActor(nmbActor)

