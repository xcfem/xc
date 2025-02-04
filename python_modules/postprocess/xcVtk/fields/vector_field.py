# -*- coding: utf-8 -*-
'''Field vector over points'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import vtk
from misc_utils import log_messages as lmsg
from postprocess.xcVtk.fields import field_base as fb
from postprocess.xcVtk.fields import vector_field_data as vfd


def get_double_headed_arrow():
    ''' Create a double headed arrow that can be set as source connection
        for a glyph.
    '''
    arrow= vtk.vtkArrowSource()
    arrow.Update()
    arrow_input= vtk.vtkPolyData()
    arrow_input.ShallowCopy(arrow.GetOutput())

    second_head= vtk.vtkConeSource()
    tip_length= arrow.GetTipLength()
    second_head.SetHeight(tip_length)
    second_head.SetRadius(arrow.GetTipRadius())
    second_head.SetResolution(arrow.GetTipResolution())
    second_head.SetCenter([1.0-1.1*tip_length,0,0])
    second_head.Update()
    second_head_input= vtk.vtkPolyData()
    second_head_input.ShallowCopy(second_head.GetOutput())

    # Append the two meshes
    appendFilter= vtk.vtkAppendPolyData()
    appendFilter.AddInputData(arrow_input)
    appendFilter.AddInputData(second_head_input)
    appendFilter.Update()
  
    #  Remove any duplicate points.
    retval= vtk.vtkCleanPolyData()
    retval.SetInputConnection(appendFilter.GetOutputPort())
    retval.Update()
    return retval

class VectorField(fb.FieldBase):
    '''Vector field defined at points.

    :ivar data: vector field data.
    :ivar showPushing: if true the loads ares showed pushing the loaded 
                            point (as oppssed to pull). Default: True.
    :ivar symType: shape of the symbol (defaults to an arrow).
    '''
    def __init__(self, name, fUnitConv, scaleFactor, showPushing= True, symType=vtk.vtkArrowSource()):
        ''' Constructor.
        
        :param name: name of the vector field.
        :param fUnitConv: unit conversion factor.
        :param scaleFactor: scale factor for the size of the vectors.
        :param showPushing: if true the loads ares showed pushing the loaded 
                            point (as oppssed to pull). Default: True.
        :param symType: shape of the symbol (defaults to an arrow).
        '''
        super(VectorField,self).__init__(name, fUnitConv)
        self.data= vfd.VectorFieldData(name, 3, scaleFactor)
        self.showPushing= showPushing #If true vector push else pulls from point
        self.symType= symType

    def setupMapper(self):
        self.data.setupGlyph(self.fUnitConv, self.symType)
        self.mapper = vtk.vtkPolyDataMapper()
        self.mapper.SetInputConnection(self.data.glyph.GetOutputPort())
        self.mapper.SetScalarModeToUsePointFieldData()
        self.mapper.SetColorModeToMapScalars()
        self.mapper.ScalarVisibilityOn()
        self.mapper.SetScalarRange(self.data.lengths.GetRange())
        self.mapper.SelectColorArray(self.data.lengthsName)
        self.createLookUpTable()
        self.mapper.SetLookupTable(self.lookUpTable)
        return self.mapper

    def setupActor(self):
        self.setupMapper()
        self.actor = vtk.vtkActor()
        self.actor.SetMapper(self.mapper)
        return self.actor

    def populateFromPairList(self, pairList):
        ''' Adds vector data from the pair list
            argument.

        :param pairList: list pairs (position, vector).
        '''
        for pair in pairList:
            p= pair[0]
            v= pair[1]
            self.data.insertNextVector(v[0],v[1],v[2])
            self.data.insertNextPair(p[0],p[1],p[2],v[0],v[1],v[2],self.fUnitConv,self.showPushing)

    def addToDisplay(self, recordDisplay, orientation= 1, title= None):
        ''' Adds the vector field to the display. 

        :param orientation: orientation of the scalar bar (1: horizontal,
                            2: left-vertical, 3 right-vertical)
                            (defaults to horizontal scalar bar)
        :param title: title of the scalar bar
        '''
        nTuples= self.data.getNumberOfTuples()
        if(nTuples>0):
            self.setupActor()
            recordDisplay.renderer.AddActor(self.actor)
            self.creaColorScaleBar(orientation,title)
            recordDisplay.renderer.AddActor2D(self.scalarBar)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            message= className+'.'+methodName+'; no vectors to display. Command ignored.'
            lmsg.warning(message)


def get_force_and_torque_vector_fields(forceFieldName, forcePairs, torqueFieldName, torquePairs, fUnitConv, scaleFactor, showPushing= True):
    ''' Creates two vector fields representing the forces and the moment 
        corresponding to the given data.

    :param forceFieldName: name of the force field.
    :param forcePairs: list of pairs (position, force vector).
    :param torqueFieldName: name of the torque field.
    :param torquePairs: list of pairs (position, torque vector).
    :param fUnitConv: unit conversion factor.
    :param scaleFactor: scale factor for the size of the vectors.
    :param showPushing: if true the loads ares showed pushing the loaded 
                        point (as oppssed to pull). Default: True.
    '''
    # Arrow representing forces.
    forceArrow= vtk.vtkArrowSource()
    vFieldF= VectorField(name= forceFieldName, fUnitConv= fUnitConv,scaleFactor= scaleFactor, showPushing= True,symType= forceArrow) # Force
    vFieldF.populateFromPairList(forcePairs)
    # Double-headed arrow representing moments.
    torqueArrow= get_double_headed_arrow()
    vFieldM= VectorField(name= torqueFieldName, fUnitConv= fUnitConv,scaleFactor= scaleFactor, showPushing= True, symType= torqueArrow) # Moment
    vFieldM.populateFromPairList(torquePairs)
    return vFieldF, vFieldM


def get_disp_and_rotation_vector_fields(dispFieldName, dispPairs, rotationFieldName, rotationPairs, fUnitConv, scaleFactor, showPushing= False):
    ''' Creates two vector fields representing the displacements and the
        rotations corresponding to the given data.

    :param dispFieldName: name of the disp field.
    :param dispPairs: list of pairs (position, disp vector).
    :param rotationFieldName: name of the rotation field.
    :param rotationPairs: list of pairs (position, rotation vector).
    :param fUnitConv: unit conversion factor.
    :param scaleFactor: scale factor for the size of the vectors.
    :param showPushing: if true the loads ares showed pushing the loaded 
                        point (as oppssed to pull). Default: True.
    '''
    vFieldD= None
    if(dispFieldName is not None):
        if(len(dispPairs)>0):
            dispArrow= vtk.vtkArrowSource()
            vFieldD= VectorField(name= dispFieldName, fUnitConv= fUnitConv, scaleFactor= scaleFactor, showPushing= showPushing, symType= dispArrow) # Displacement
            vFieldD.populateFromPairList(dispPairs)
    vFieldR= None
    if(rotationFieldName is not None):
        
        if(len(rotationPairs)>0):
            rotationArrow= get_double_headed_arrow()
            vFieldR= VectorField(name= rotationFieldName, fUnitConv= fUnitConv, scaleFactor= scaleFactor,showPushing= showPushing, symType= rotationArrow) # Rotation
            vFieldR.populateFromPairList(rotationPairs)

    return vFieldD, vFieldR
