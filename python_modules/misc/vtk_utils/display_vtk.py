# -*- coding: utf-8 -*-

from vtk.vtkRenderingCore import vtkWindowToImageFilter
from vtk.vtkIOImage import vtkJPEGWriter

#Genera la imagen en un archivo.
def plotToFile(fName, renWin, renderer, width, height,fileFormat):
  w2i = vtkWindowToImageFilter()
  writer = vtkJPEGWriter()
  w2i.SetInput(renWin)
  w2i.Update()
  writer.SetInputConnection(w2i.GetOutputPort())
  writer.SetFileName(fName)
  renWin.Render()
  w2i.Update()
  writer.Write()


