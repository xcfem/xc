def VtkDibujaNodes(nmbActor):
   # Define el actor a emplear para dibujar kpoints.
   sphereNodes= vtk.vtkSphereSource()
   sphereNodes.SetRadius(0.05)
   sphereNodes.SetThetaResolution(5)
   sphereNodes.SetPhiResolution(5)
        
   markNodes= vtk.vtkGlyph3D()
   markNodes.SetInput(ugrid)
   markNodes.SetSource(sphereNodes)
   markNodes.ScalingOff()
   markNodes.OrientOff()
        
   mappNodes= vtk.vtkPolyDataMapper().SetInput(markNodes)
   nmbActor= vtk.vtkActor()
   nmbActor.SetMapper(mappNodes)
   nmbActor.GetProperty().SetColor(.7, .5, .5)
   renderer.AddActor(nmbActor)

