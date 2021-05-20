# -*- coding: utf-8 -*-

from __future__ import print_function
import vtk
import math

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
    symTpLow= symbType.lower()
    if 'arrow' in symTpLow:
        symbSource= vtk.vtkArrowSource()
    elif 'cone' in symTpLow:
        symbSource= vtk.vtkConeSource()
    elif 'sphere' in symTpLow:
        symbSource= vtk.vtkSphereSource()
    elif 'cube' in symTpLow:
        symbSource= vtk.vtkCubeSource()
    elif 'shaftkey' in symTpLow:
        symbSource= vtk.vtkCubeSource()
        symbSource.SetZLength(0.25)
        symbSource.SetXLength(0.75)
    elif 'cylinder' in symTpLow:
        symbSource= vtk.vtkCylinderSource()
    elif 'disk' in symTpLow:
        symbSource= vtk.vtkCylinderSource()
        symbSource.SetHeight(0.25)
        symbSource.SetResolution(10)
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

