import vtk
import logging

#Crea una matriz de cadenas de caracteres con datos de los puntos
# definidas en el conjunto.
def VtkCreaStrArraySetData(setToDraw, nmbTipoEnt, attr):
  # Definimos la matriz
  arr= vtk.vtkStringArray()
  arr.SetName(attr)
  container= None
  if(nmbTipoEnt=="pnts"):
    container= setToDraw.getPoints
  elif(nmbTipoEnt=="lines"):
    container= setToDraw.getLines
  elif(nmbTipoEnt=="nodos"):
    container= setToDraw.getNodes
  elif(nmbTipoEnt=="elementos"):
    container= setToDraw.getElements
  else:
    logging.error("error: "+nmbTipoEnt+" no implementada.")
  for e in container:
    tmp= str(getattr(e,attr))
    arr.InsertValue(e.getIdx,tmp)
  return arr
