# -*- coding: utf-8 -*-

import vtk

#Genera la imagen en un archivo.
def plotToFile(fName, renWin, renderer, width, height,fileFormat):
  w2i = vtk.vtkWindowToImageFilter()
  writer = vtk.vtkJPEGWriter()
  w2i.SetInput(renWin)
  w2i.Update()
  writer.SetInputConnection(w2i.GetOutputPort())
  writer.SetFileName(fName)
  renWin.Render()
  w2i.Update()
  writer.Write()


