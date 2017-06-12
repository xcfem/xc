def VtkDefineIdFilter(nmbIdFilter):
  # vtkIdFilter genera escalares a partir del identificador
  nmbIdFilter= vtk.vtkIdFilter()
  nmbIdFilter.SetInput(ugrid)
  nmbIdFilter.CellIdsOff()
  nmbIdFilter.PointIdsOff()

