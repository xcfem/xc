# -*- coding: utf-8 -*-

def VtkCargaIdsKPts(recordGrid):
  etiqKPt= VtkCreaStrArraySetData(recordGrid.setName,"pnts","codigo")
  recordGrid.uGrid.PointData.SetStrings(etiqKPt)

# Dibuja las etiquetas de los puntos
def VtkDibujaIdsKPts(nmbUGrid,setName,renderer):
  st= preprocessor.getSets.getSet(setName)
  numKPtsDI= st.size

  if(numKPtsDI>0):
    ids= vtk.VtkIdFilter()
    ids.SetInput(nmbUGrid)
    ids.CellsIdsOff()
    ids.PointIdsOff()

    VtkCargaIdsKPts(nmbUGrid,setName)
    visPts= vtk.vtkSelectVisiblePoints
    visPts.SetInput("ids")
    visPts.SetRenderer(renderer)
    visPts.SelectionWindowOff()
    #Create the mapper to display the point ids.  Specify the format to
    # use for the labels.  Also create the associated actor.}
    ldm= vtk.vtkLabeledShStrMapper
    ldm.SetInput(visPts)
    ldm.LabelTextProperty.SetColor(0.1,0.1,0.1)
    pointLabels= vtk.VtkActor2D()
    pointLabels.SetMapper(ldm)
    renderer.AddActor2D(pointLabels)
  else:
    print "El conjunto: '",setName,"' no tiene KPts"

# Define el actor a emplear para dibujar kpoints.
def VtkDefineActorKPoint(nmbUGrid,renderer,radius):
  sphereSource= vtk.VtkSphereSource()
  sphereSource.radius(radius)
  sphereSource.ResolTheta(5)
  sphereSource.ResolPhi(5)
    
  markKPts= vtk.vtkGlyph3D()
  markKPts.SetInput(nmbUGrid)
  markKPts.set_source(sphereSource)
  markKPts.ScalingOff()
  markKPts.OrientOff()
    
  mappKPts= vtk.VtkPolyDataMapper()
  mappKPts.SetInput(markKPts)
  visKPts= vtk.VtkActor()
  visKPts.SetMapper(mappKPts)
  visKPts.property.SetColor(.7, .5, .5)
  renderer.AddActor(visKPts)  

