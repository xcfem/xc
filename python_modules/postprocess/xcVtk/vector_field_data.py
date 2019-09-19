# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

'''Data to represent field vector in VTK.'''

import math
import vtk
from miscUtils import LogMessages as lmsg
from postprocess.xcVtk import field_base as fb

class VectorFieldData(object):
  '''Vectors defined at points.'''
  def __init__(self, name, numberOfComponents= 3, scaleFactor= 1.0):
    '''
    Parameters:
      name: name to identify VTK arrays related to this object.
      numberOrComponents: size of the vectors.
      scaleFactor: scale factor for the size of the vectors.
    '''
    super(VectorFieldData,self).__init__()
    self.name= name
    self.scaleFactor= scaleFactor
    self.points= vtk.vtkPoints()
    self.vectors= vtk.vtkDoubleArray()
    self.vectorsName= self.name+'Vectors'
    self.vectors.SetName(self.vectorsName)
    self.vectors.SetNumberOfComponents(numberOfComponents)

  def getNumberOfTuples(self):
    return self.vectors.GetNumberOfTuples()

  def calculateLengths(self,fUnitConv= 1.0):
    '''
    Lengths of the vectors.
    Parameters:
      fUnitConv: unit conversion scale factor i.e. kN -> 1e-3.
    '''
    self.lengths= vtk.vtkDoubleArray()
    self.lenghtsName= self.name+'Lengths'
    self.lengths.SetName(self.lenghtsName)
    sz= self.getNumberOfTuples()
    if(sz):
      self.lengths.SetNumberOfValues(sz)
      for i in range(0,sz):
        v= self.vectors.GetTuple(i)
        l= math.sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])*fUnitConv
        self.lengths.SetValue(i,l)
    else:
      lmsg.warning('VectorFieldData.calculateLengths: no vectors defined.')
    return self.lengths

  def insertNextVector(self,vx,vy,vz):
    self.vectors.InsertNextTuple3(vx,vy,vz)
  
  def insertNextPair(self,px,py,pz,vx,vy,vz,fUnitConv= 1.0,pushing= False):
    '''Inserts a point in the collection.
       Parameters:
         px,py,pz: coordinates of the point.
         vx,vy,vz: coordinates of the vector.
         pushing: true: arrow ends in the point.
                  false: arrow starts in the point.
    '''
    factor= fUnitConv*self.scaleFactor
    if(pushing):
      self.points.InsertNextPoint(px-vx*factor,py-vy*factor,pz-vz*factor)
    else:
      self.points.InsertNextPoint(px,py,pz)

  def getPolydata(self,fUnitConv= 1.0):
    retval= vtk.vtkPolyData()
    retval.SetPoints(self.points)
    sz= self.getNumberOfTuples()
    if(sz>0):
      retval.GetPointData().AddArray(self.vectors)
      self.calculateLengths(fUnitConv)
      retval.GetPointData().AddArray(self.lengths)
    else:
      lmsg.warning('VectorFieldData.getPolydata: no vectors.')
    return retval

  def setupGlyph(self,fUnitConv= 1.0,symType=vtk.vtkArrowSource()):
    self.polydata= self.getPolydata(fUnitConv)
    # Generate the arrow for the glyphs
    self.glyph = vtk.vtkGlyph3D()
    self.glyph.SetInputData(self.polydata)
    self.glyph.SetSourceConnection(symType.GetOutputPort())
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
    #self.glyph.SetInputArrayToProcess(3,0,0,0,self.lenghtsName) # colors

    # Calling update because I'm going to use the scalar range to set the color map range
    self.glyph.Update()



