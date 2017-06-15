def VtkDibujaNodos(nmbActor):
   # Define el actor a emplear para dibujar kpoints.
   sphereNodos= vtk.vtkSphereSource()
   sphereNodos.SetRadius(0.05)
   sphereNodos.SetThetaResolution(5)
   sphereNodos.SetPhiResolution(5)
        
   markNodos= vtk.vtkGlyph3D()
   markNodos.SetInput(ugrid)
   markNodos.SetSource(sphereNodos)
   markNodos.ScalingOff()
   markNodos.OrientOff()
        
   mappNodos= vtk.vtkPolyDataMapper().SetInput(markNodos)
   nmbActor= vtk.vtkActor()
   nmbActor.SetMapper(mappNodos)
   nmbActor.GetProperty().SetColor(.7, .5, .5)
   renderer.AddActor(nmbActor)

