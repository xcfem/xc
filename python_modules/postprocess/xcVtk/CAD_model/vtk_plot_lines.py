# -*- coding: utf-8 -*-
def VtkDibujaLineas(nmbActor):
  # Define el actor a emplear para dibujar elementos.
  ugridMapper= vtk.vtkDataSetMapper().SetInput(ugrid)
  nmbActor= vtk.vtkActor()
  nmbActor.SetMapper(ugridMapper)
  nmbActor.GetProperty().SetColor(0,0,0)
  nmbActor.GetProperty().SetRepresentationToWireFrame()

  renderer.AddActor(nmbActor)
