# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import vtk
import math
from misc_utils import log_messages as lmsg

def draw_vtk_symbols(pointData, renderer, scale):
    ''' Adds to the renderer the symbols stored in point data.

    :paramData: list of dictionaries containing the symbol type, color, 
                orientation and position of the symbols.
    '''
    symbolDict= dict()
    for pData in pointData:
        vDir= pData['vDir']
        symbolType= pData['symbType']
        color= pData['color']
        if(color==[1,0,0]):
            glyphColor= 'red'
        elif(color==[0,1,0]):
            glyphColor= 'green'
        elif(color==[0,0,1]):
            glyphColor= 'blue'
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+"; arbitrary colors for symbols not supported yet.")
            glyphColor= None
        vPos= pData['vPos']
        if symbolType in symbolDict:
            if(glyphColor in symbolDict[symbolType]):
                symbolDict[symbolType][glyphColor].append((vPos, vDir,color))
            else:
                symbolDict[symbolType][glyphColor]= [(vPos, vDir, color)]
        else:
            symbolDict[symbolType]= {glyphColor:[(vPos, vDir, color)]}
    # Create VTK points.
    for symbolType in symbolDict:
        for glyphColor in symbolDict[symbolType]:
            pointData= symbolDict[symbolType][glyphColor]
            vDir= pointData[0][1]
            color= pointData[0][2]
            glyphActor= point_to_glyph(pointData= pointData, symbType= symbolType, scale= scale)
            renderer.AddActor(glyphActor)
    
def point_to_glyph(pointData, symbType, scale):
    """ Convert points to glyphs.

    :param points: The points to glyph.
    :param symbType: type of symbol (available types: 'arrow', 'doubleArrow',
           'cone', 'doubleCone', 'sphere', 'doubleSphere','cube' ,
           'doubleCube', 'cylinder', 'doubleCylinder')    
    :param vDir: director vector to orient the symbol
    :param color: list [R,G,B] with the 3 components of color
    :param scale: The scale, used to determine the size of the
                  glyph representing the point, expressed as a
                  fraction of the largest side of the bounding
                  box surrounding the points. e.g. 0.05
    :return: The actor.
    """
    dirs= list()
    #colors= list()
    color= pointData[0][2]
    positions= list()
    for p in pointData:
        vPos= p[0]
        vDir= p[1]
        vPosVx=[vPos[i]-scale/2.0*vDir[i] for i in range(3)] #vertex position
        positions.append(vPosVx)
        dirs.append(vDir)
        # colors.append(p[2])
    glyphSource= get_symbol_source(symbType)
    
    vtkPoints= vtk.vtkPoints()
    orientation= vtk.vtkDoubleArray()
    orientation.SetName('Orientation')
    orientation.SetNumberOfComponents(3)
    scaleArray= vtk.vtkDoubleArray()
    scaleArray.SetName('Scale')
    scaleArray.SetNumberOfComponents(3)
    # colorArray= vtk.vtkFloatArray()
    # colorArray.SetName('Color')
    # colorArray.SetNumberOfComponents(3)
    for p, vDir in zip(positions, dirs):
        vtkPoints.InsertNextPoint(p[0], p[1], p[2])
        orientation.InsertNextTuple3(vDir[0],vDir[1],vDir[2])
        scaleArray.InsertNextTuple3(scale,scale/2.0,scale/2.0)
        # colorArray.InsertNextTuple3(color[0],color[1],color[2])

    polyData= vtk.vtkPolyData()
    polyData.SetPoints(vtkPoints)
    polyData.GetPointData().AddArray(orientation)
    polyData.GetPointData().AddArray(scaleArray)
    # polyData.GetPointData().AddArray(colorArray)

    mapper = vtk.vtkGlyph3DMapper()
    mapper.SetInputData(polyData)
    mapper.SetSourceConnection(glyphSource.GetOutputPort())
    mapper.SetOrientationModeToDirection()
    mapper.SetOrientationArray('Orientation')
    mapper.SetScaleModeToScaleByVectorComponents()
    mapper.SetScaleArray('Scale')
    mapper.ScalarVisibilityOff()
    # mapper.SetColorModeToDirectScalars()
    # mapper.SelectColorArray('Color')
    mapper.ScalingOn()

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(color[0],color[1],color[2])

    return actor

def get_symbol_source(symbType):
    ''' Return the apropiate VTK source object.

    :param symbType: type of symbol (available types: 'arrow', 'doubleArrow',
           'cone', 'doubleCone', 'sphere', 'doubleSphere','cube' ,
           'doubleCube', 'cylinder', 'doubleCylinder')
    '''
    symTpLow= symbType.lower()
    if 'arrow' in symTpLow:
        retval= vtk.vtkArrowSource()
    elif 'cone' in symTpLow:
        retval= vtk.vtkConeSource()
    elif 'sphere' in symTpLow:
        retval= vtk.vtkSphereSource()
    elif 'cube' in symTpLow:
        retval= vtk.vtkCubeSource()
    elif 'shaftkey' in symTpLow:
        retval= vtk.vtkCubeSource()
        retval.SetZLength(0.25)
        retval.SetXLength(0.75)
    elif 'cylinder' in symTpLow:
        retval= vtk.vtkCylinderSource()
    elif 'disk' in symTpLow:
        retval= vtk.vtkCylinderSource()
        retval.SetHeight(0.25)
        retval.SetResolution(10)
    retval.Update()
    return retval

def drawVtkSymb(symbType,renderer, RGBcolor, vPos, vDir, scale):
    '''Adds to the renderer a symbol of type 'arrow', 'doubleArrow',
    'cone', 'doubleCone', 'sphere', 'doubleSphere','cube' , 'doubleCube',
    'cylinder', 'doubleCylinder'

    :param symbType: type of symbol (available types: 'arrow', 'doubleArrow',
           'cone', 'doubleCone', 'sphere', 'doubleSphere','cube' ,
           'doubleCube', 'cylinder', 'doubleCylinder')
    :param renderer: vtk renderer
    :param RGBcolor: list [R,G,B] with the 3 components of color
    :param vPos: list [x,y,z] with the 3 coordinates of the point where
           to place the symbol.
    :param vDir: director vector to orient the symbol
    :param scale: scale to be applied to the symbol representation
    '''
    symbSource= get_symbol_source(symbType)
    vPosVx=[vPos[i]-scale/2.0*vDir[i] for i in range(3)] #vertex position
    addSymb(symbSource,renderer, RGBcolor, vPosVx, vDir, scale)
    if 'double' in symTpLow:
        vPosVx=[vPosVx[i]-scale*vDir[i] for i in range(3)] #vertex position
        addSymb(symbSource,renderer, RGBcolor, vPosVx, vDir, scale)
    
def addSymb(symbSource,renderer, RGBcolor, vPosVx, vDir, scale):
    '''function called by drawVtkSymb that puts the symbol in the renderer
    '''
    actor= vtk.vtkActor()
    parallelTo(actor,vDir)
    actor.SetPosition(vPosVx[0],vPosVx[1],vPosVx[2])
    actor.SetScale(scale,scale/2,scale/2)
    actor.GetProperty().SetColor(RGBcolor[0],RGBcolor[1],RGBcolor[2])
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(symbSource.GetOutputPort())
    actor.SetMapper(mapper)
    renderer.AddActor(actor)


def parallelTo(actor,vDir):
    '''function called by drawVtkSymb to orient the symbol parallel to 
    the director vector
    '''
    nc= len(vDir)
    if(nc!=3):
        print("parallelTo: ", vDir, " wrong dimension (must be 3).")
    else:
        v= [vDir[0],vDir[1],vDir[2]]
        thetaZ= math.degrees(math.atan2(v[1],v[0]))
        actor.RotateZ(thetaZ)
        thetaY= -math.degrees(math.atan2(v[2],math.sqrt((v[0]*v[0])+(v[1]*v[1]))))
        actor.RotateY(thetaY)

