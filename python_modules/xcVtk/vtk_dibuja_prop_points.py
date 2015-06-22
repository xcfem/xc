# -*- coding: utf-8 -*-
def VtkDibujaPropPoints(nmbFilter):
   visPoints= vtk.vtkSelectVisiblePoints()
   visPoints.SetInput(nmbFilter)
   visPoints.SetRenderer(renderer)
   visPoints.SelectionWindowOff()

   #Create the mapper to display the point ids.  Specify the format to
   #   use for the labels.  Also create the associated actor.
   pointMapper= vtk.vtkLabeledShStrMapper()
   pointMapper.SetInput(visPoints)
   ltp= pointMapper.LabelTextProperty()
   ltp.SetColor(0,0,0.9)
   ltp.BoldOff()
   ltp.ShadowOff() 
   ltp.ItalicOff()
   ltp.SetJustificationToLeft()
   ltp.SetFontSize(10)

   pointLabels= vtk.vtkActor2D()
   pointLabels.SetMapper(pointMapper)
        
   renderer.AddActor2D(pointLabels)

