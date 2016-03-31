# -*- coding: utf-8 -*-

'''Field vector over points'''

import math
import vtk
from miscUtils import LogMessages as lmsg
from xcVtk import FieldBase as fb

class VectorField(fb.FieldBase):
  '''Vector field defined at points.'''
  def __init__(self,name,fUnitConv,scaleFactor,showPushing= True):
    super(VectorField,self).__init__(name,fUnitConv)
    self.scaleFactor= scaleFactor
    self.showPushing= showPushing #If true vector push else pulls from point
    self.points= vtk.vtkPoints()
    self.vectors= vtk.vtkDoubleArray()
    self.vectorsName= self.name+'Vectors'
    self.vectors.SetName(self.vectorsName)
    self.vectors.SetNumberOfComponents(3)

  def calculateLengths(self):
    self.lengths= vtk.vtkDoubleArray()
    self.lenghtsName= self.name+'Lengths'
    self.lengths.SetName(self.lenghtsName)
    sz= self.vectors.GetNumberOfTuples()
    if(sz):
      self.lengths.SetNumberOfValues(sz)
      for i in range(0,sz):
        v= self.vectors.GetTuple(i)
        l= math.sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])*self.fConvUnidades
        self.lengths.SetValue(i,l)
    else:
      lmsg.warning('VectorField.calculateLengths: no vectors defined.')
    return self.lengths

  def insertNextPair(self,px,py,pz,vx,vy,vz):
    '''Inserts a point in the collection.'''
    if(self.showPushing):
      self.points.InsertNextPoint(px-vx*self.fConvUnidades*self.scaleFactor,py-vy*self.fConvUnidades*self.scaleFactor,pz-vz*self.fConvUnidades*self.scaleFactor)
    else:
      self.points.InsertNextPoint(px,py,pz)
    self.vectors.InsertNextTuple3(vx,vy,vz)

  def setupPolydata(self):
    self.polydata= vtk.vtkPolyData()
    self.polydata.SetPoints(self.points)
    self.polydata.GetPointData().AddArray(self.vectors)
    self.calculateLengths()
    self.polydata.GetPointData().AddArray(self.lengths)
    return self.polydata

  def setupGlyph(self):
    self.setupPolydata()
    # Generate the arrow for the glyphs
    arrow = vtk.vtkArrowSource()#vtk.vtkConeSource()
    #arrow.SetRadius(0.1)
    #arrow.SetHeight(0.5)
    self.glyph = vtk.vtkGlyph3D()
    self.glyph.SetInput(self.polydata)
    self.glyph.SetSourceConnection(arrow.GetOutputPort())
    self.glyph.ScalingOn()
    self.glyph.SetScaleModeToScaleByScalar()
    self.glyph.SetVectorModeToUseVector()
    self.glyph.OrientOn()
    # Tell the filter to "clamp" the scalar range
    #self.glyph.ClampingOn()  
    # Set the overall (multiplicative) scaling factor
    self.glyph.SetScaleFactor(self.scaleFactor)

    # Set the Range to "clamp" the data to 
    #   -- see equations above for nonintuitive definition of "clamping"
    # The fact that I'm setting the minimum value of the range below
    #   the minimum of my data (real min=0.0) with the equations above
    #   forces a minimum non-zero glyph size.
     
    #self.glyph.SetRange(-10, 10)    # Change these values to see effect on cone sizes
     
    # Tell glyph which attribute arrays to use for what
    self.glyph.SetInputArrayToProcess(0,0,0,0,self.lenghtsName)	# scalars
    self.glyph.SetInputArrayToProcess(1,0,0,0,self.vectorsName) # vectors
    # self.glyph.SetInputArrayToProcess(2,0,0,0,'nothing')	# normals
    #self.glyph.SetInputArrayToProcess(3,0,0,0,self.lenghtsName)		# colors
     
    # Calling update because I'm going to use the scalar range to set the color map range
    self.glyph.Update()

  def setupMapper(self):
    self.setupGlyph()
    self.mapper = vtk.vtkPolyDataMapper()
    self.mapper.SetInputConnection(self.glyph.GetOutputPort())
    self.mapper.SetScalarModeToUsePointFieldData()
    self.mapper.SetColorModeToMapScalars()
    self.mapper.ScalarVisibilityOn()
    #self.mapper.SetScalarRange(self.glyph.GetOutputDataObject(0).GetPointData().GetArray(self.lenghtsName).GetRange())
    self.mapper.SetScalarRange(self.lengths.GetRange())
    self.mapper.SelectColorArray(self.lenghtsName)
    self.creaLookUpTable()
    self.mapper.SetLookupTable(self.lookUpTable)
    return self.mapper

  def setupActor(self):
    self.setupMapper()
    self.actor = vtk.vtkActor()
    self.actor.SetMapper(self.mapper)
    return self.actor

  def addToDisplay(self, recordDisplay):
    # Adds the vector field to the display.
    self.setupActor()
    recordDisplay.renderer.AddActor(self.actor)
    self.creaColorScaleBar()
    # mapper2D= vtk.vtkPolyDataMapper2D()
    # self.scalarBar.SetMapper(mapper2D)
    recordDisplay.renderer.AddActor2D(self.scalarBar)
