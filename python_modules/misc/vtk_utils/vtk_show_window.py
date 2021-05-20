# -*- coding: utf-8 -*-

import vtk

def VtkMuestraVentana(renderer, nmbWindow,szx,szy):
  nmbWindow= vtk.vtkRenderWindow()
  nmbWindow.AddRenderer(renderer)
  iren= vtkRenderWindowInteractor()
  iren.SetRenderWindow(nmbWindow)
  renWin.SetSize(szx,szy)
  iren.Initialize()
  iren.Start()
