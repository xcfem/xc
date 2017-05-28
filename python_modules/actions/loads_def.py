# -*- coding: utf-8 -*-


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import xc_base
import geom
import xc
from actions.earth_pressure import soil_properties as sp

class InertialLoad(object):
    '''Inertial load (density*acceleration) applied to the elements in the list
    of mesh-sets

    :ivar name:     name identifying the load
    :ivar lstMeshSets: list of mesh-sets (of type LinSetToMesh or SurfSetToMesh)
    :ivar vAccel:   acceleration vector xc.Vector([ax,ay,az])
    '''
    def __init__(self,name, lstMeshSets, vAccel):
        self.name=name
        self.lstMeshSets=lstMeshSets
        self.vAccel= vAccel

    def appendLoadToCurrentLoadPattern(self):
        for ms in self.lstMeshSets:
            if 'shell' in ms.elemType:
                loadVector=ms.matSect.getAreaDensity()*self.vAccel
                el_group=ms.surfSet.getElements
            elif 'beam' in ms.elemType:
                loadVector=ms.matSect.getLongitudinalDensity()*self.vAccel
                el_group=ms.linSet.getElements
            for e in el_group:
                e.vector3dUniformLoadGlobal(loadVector)

class NodalLoad(object):
    '''Point load applied on a list of nodes

    :ivar name:   name identifying the load
    :ivar lstNod: list of nodes on which the load is applied.
    :ivar loadVector: xc.Vector with the six components of the 
          load: xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
    '''
    def __init__(self,name, lstNod, loadVector):
        self.name=name
        self.lstNod= lstNod
        self.loadVector= loadVector

    def appendLoadToCurrentLoadPattern(self):
        for n in self.lstNod:
            n.newLoad(self.loadVector)
            
class UniformLoadOnBeams(object):
    '''Uniform load applied on the beam elements generated from
    all the lines in the xcSet.

    :ivar name:       name identifying the load
    :ivar xcSet:      set that contains the lines
    :ivar loadVector: xc.Vector with the six components of the load: 
                      xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
    :ivar refSystem: reference system in which loadVector is defined:
                   'Local': element local coordinate system
                   'Global': global coordinate system (defaults to 'Global)
    '''
    def __init__(self,name, xcSet, loadVector,refSystem='Global'):
        self.name=name
        self.xcSet=xcSet
        self.loadVector= loadVector
        self.refSystem= refSystem

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        for l in self.xcSet.getLines:
            for e in l.getElements():
                if self.refSystem=='Local':
                    e.vector3dUniformLoadLocal(self.loadVector)
                else:
                    e.vector3dUniformLoadGlobal(self.loadVector)
 

class UniformLoadOnSurfaces(object):
    '''Uniform load applied on the shell elements generated from
    all the surfaces in the xcSet.

    :ivar name:       name identifying the load
    :ivar xcSet:      set that contains the surfaces
    :ivar loadVector: xc.Vector with the six components of the load: 
                      xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
    '''
    def __init__(self,name, xcSet, loadVector):
        self.name=name
        self.xcSet=xcSet
        self.loadVector= loadVector

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        for s in self.xcSet.getSurfaces:
            for e in s.getElements():
                e.vector3dUniformLoadGlobal(self.loadVector)

class EarthPressLoad(object):
    '''Earth pressure applied on the shell elements generated from
    all the surfaces in the xcSet. 
    
    :ivar name:      name identifying the load
    :ivar xcSet:     set that contains the surfaces
    :ivar soilProp:  instance of the class SoilProp that defines the 
                     soil parameters required to calculate the earth pressure
                     (K:coefficient of pressure, zGround: global Z coordinate 
                     of ground level,gammaSoil: weight density of soil, 
                     zWater: global Z coordinate of groundwater level, 
                     gammaWater: weight density of water) 
    :ivar vDir:      unit xc vector defining pressures direction
    '''
    def __init__(self,name, xcSet,soilData, vDir):
        self.name=name
        self.xcSet=xcSet
        self.soilData=soilData
        self.vDir=vDir
    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        for s in self.xcSet.getSurfaces:
            for e in s.getElements():
                presElem=self.soilData.getPressure(e.getCooCentroid(False)[2])
                if(presElem!=0.0):
                    e.vector3dUniformLoadGlobal(presElem*self.vDir)
 

 
