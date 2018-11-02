# -*- coding: utf-8 -*-
from __future__ import division


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import xc
from actions.earth_pressure import earth_pressure as ep
from model.sets import sets_mng as sets
from miscUtils import LogMessages as lmsg
import numpy as np
from actions import load_cases

class BaseVectorLoad(object):
    '''Base class for loads introduced using a load as an xcVector 

    :ivar name:     name identifying the load
    :ivar loadVector: load xc.Vector
    '''
    def __init__(self,name, loadVector):
        self.name=name
        self.loadVector= loadVector

    def __mul__(self,factor):
        '''Apply the factor to the load and append it to the current load pattern'''
        self.loadVector=factor*self.loadVector
        self.appendLoadToCurrentLoadPattern()
            
    def __rmul__(self,factor):
        '''Apply the factor to the load and append it to the current load pattern'''
        self.loadVector=factor*self.loadVector
        self.appendLoadToCurrentLoadPattern()
     

        

class InertialLoad(BaseVectorLoad):
    '''Inertial load (density*acceleration) applied to the elements in the list
    of mesh-sets

    :ivar name:     name identifying the load
    :ivar lstMeshSets: list of mesh-sets (of type LinSetToMesh or SurfSetToMesh)
    :ivar vAccel:   acceleration vector xc.Vector([ax,ay,az])
    '''
    def __init__(self,name, lstMeshSets, vAccel):
        super(InertialLoad,self).__init__(name,vAccel)
        self.lstMeshSets=lstMeshSets

    def appendLoadToCurrentLoadPattern(self):
        for ms in self.lstMeshSets:
            if 'shell' in ms.elemType.lower():
                loadVector= ms.matSect.getAreaDensity()*self.loadVector
                el_group= ms.primitiveSet.getElements
            elif 'beam' in ms.elemType.lower():
                loadVector= ms.matSect.getRho()*self.loadVector
                el_group= ms.primitiveSet.getElements
            for e in el_group:
                e.vector3dUniformLoadGlobal(loadVector)
                
    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        maxValue=0
        for ms in self.lstMeshSets:
            if 'shell' in ms.elemType.lower():
                val= ms.matSect.getAreaDensity()*self.loadVector.Norm()
            elif 'beam' in ms.elemType.lower():
                val= ms.matSect.getRho()*self.loadVector.Norm()
            maxValue=max(val,maxValue)
        return maxValue

             

class NodalLoad(BaseVectorLoad):
    '''Point load applied on a list of nodes

    :ivar name:   name identifying the load
    :ivar lstNod: list of nodes on which the load is applied.
    :ivar loadVector: xc.Vector with the six components of the 
          load: xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
    '''
    def __init__(self,name, lstNod, loadVector):
        super(NodalLoad,self).__init__(name,loadVector)
        self.name=name
        self.lstNod= lstNod

    def appendLoadToCurrentLoadPattern(self):
        for n in self.lstNod:
            n.newLoad(self.loadVector)
            
    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        return self.loadVector.Norm()


           
class UniformLoadOnBeams(BaseVectorLoad):
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
        super(UniformLoadOnBeams,self).__init__(name,loadVector)
        self.xcSet=xcSet
        self.refSystem= refSystem

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        for l in self.xcSet.getLines:
            for e in l.getElements():
                if self.refSystem=='Local':
                    load= e.vector3dUniformLoadLocal(self.loadVector)
                else:
                    load= e.vector3dUniformLoadGlobal(self.loadVector)
 
    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        return self.loadVector.Norm()
        

class UniformLoadOnLines(BaseVectorLoad):
    '''Uniform load applied to all the lines (not necessarily defined as lines
    for latter generation of beam elements, they can be lines belonging to 
    surfaces for example) found in the xcSet
    The uniform load is introduced as point loads in the nodes
    
    :ivar name:   name identifying the load
    :ivar xcSet:  set that contains the lines
    :ivar loadVector: xc.Vector with the six components of the load: 
                      xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
    '''                            
    def __init__(self,name,  xcSet, loadVector):
        super(UniformLoadOnLines,self).__init__(name,loadVector)
        self.xcSet=xcSet

    def appendLoadToCurrentLoadPattern(self):
        prep=self.xcSet.getPreprocessor
        pointsCont=prep.getMultiBlockTopology.getPoints
        for l in self.xcSet.getLines:
            ptExt1tag=l.getKPoints()[0]
            ext1=pointsCont.get(ptExt1tag).getPos
            nod=[n for n in l.getNodes()]
            ndistExtr1=[(n.getInitialPos3d).distPos3d(ext1) for n in nod]
            sortNod=[nod for ndistExtr1,nod in sorted(zip(ndistExtr1,nod))]
            lnInfl=[(sortNod[i-1].getInitialPos3d).distPos3d(sortNod[i+1].getInitialPos3d)/2 for i in range(1,len(sortNod)-1)]
            lnInfl.insert(0,(sortNod[0].getInitialPos3d).distPos3d(sortNod[1].getInitialPos3d)/2.0)
            p2=sortNod[-2].getInitialPos3d
            p1=sortNod[-1].getInitialPos3d
            lnInfl.append((p2.distPos3d(p1))/2.0)
#            lnInfl.append((sortNod[len(sortNod)-2].getInitialPos3d).distPos3d(sortNod[len(sortNod)-1].getInitialPos3d)/2.0)
            for i in range(len(sortNod)):
                n=sortNod[i]
                load=lnInfl[i]*self.loadVector
                sortNod[i].newLoad(lnInfl[i]*self.loadVector)

    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        return self.loadVector.Norm()
        

class UniformLoadOnSurfaces(BaseVectorLoad):
    '''Uniform load applied on the shell elements generated from
    all the surfaces in the xcSet.

    :ivar name:       name identifying the load
    :ivar xcSet:      set that contains the surfaces
    :ivar loadVector: xc.Vector with the six components of the load: 
                      xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
    :ivar refSystem: reference system in which loadVector is defined:
                     'Local': element local coordinate system
                     'Global': global coordinate system (defaults to 'Global)
    '''
    def __init__(self,name, xcSet, loadVector,refSystem='Global'):
        super(UniformLoadOnSurfaces,self).__init__(name,loadVector)
        self.xcSet=xcSet
        self.refSystem=refSystem
        
    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        for e in self.xcSet.getElements:
            if self.refSystem=='Local':
                load=e.vector3dUniformLoadLocal(self.loadVector)
            else:
                load=e.vector3dUniformLoadGlobal(self.loadVector)

    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        return self.loadVector.Norm()
                
class PointLoadOverShellElems(BaseVectorLoad):
    '''Point load distributed over the shell elements in xcSet whose 
    centroids are inside the prism defined by the 2D polygon prismBase
    and one global axis.

    :ivar name: name identifying the load
    :ivar xcSet: set that contains the shell elements
    :ivar loadVector: xc vector with the six components of the point load:
                      xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
    :ivar prismBase: 2D polygon that defines the n-sided base of the prism.
                      The vertices of the polygon are defined in global 
                      coordinates in the following way:
                         - for X-axis-prism: (y,z)
                         - for Y-axis-prism: (x,z)
                         - for Z-axis-prism: (x,y)
    :ivar prismAxis: axis of the prism (can be equal to 'X', 'Y', 'Z')
                      (defaults to 'Z')
    :ivar refSystem:  reference system in which loadVector is defined:
                      'Local': element local coordinate system
                      'Global': global coordinate system (defaults to 'Global')
    '''
    def __init__(self,name, xcSet, loadVector,prismBase,prismAxis='Z',refSystem='Global'):
        super(PointLoadOverShellElems,self).__init__(name,loadVector)
        self.xcSet=xcSet
        self.prismBase=prismBase
        self.prismAxis=prismAxis
        self.refSystem=refSystem

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        aux_set,factor=self.distrParam()
        for e in aux_set.getElements:
            if self.refSystem=='Local':
                load=e.vector3dUniformLoadLocal(factor*self.loadVector)
            else:
                load=e.vector3dUniformLoadGlobal(factor*self.loadVector)
        aux_set.clear()

    def distrParam(self):
        '''Return the set of elements over which to distribute the point load and the 
        coefficient that must be applied to it '''
        aux_set=None
        factor=0.0
        prep=self.xcSet.getPreprocessor
        aux_set=sets.set_included_in_orthoPrism(preprocessor=prep,setInit=self.xcSet,prismBase=self.prismBase,prismAxis=self.prismAxis,setName='aux_set'+self.name)
        if aux_set.getNumElements==0:
            lmsg.warning('Can\'t distribute load: '+self.name+'(Elements in set = 0)')
        else:
            areaSet=float(np.sum([e.getArea(False) for e in aux_set.getElements]))
            factor=1.0/areaSet
        return (aux_set,factor)

    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        aux_set,factor=self.distrParam()
        return factor*self.loadVector.Norm()

            
class EarthPressLoad(BaseVectorLoad):
    '''Earth pressure applied on the elements (shell or beams)
    included in the set xcSet. 
    
    :ivar name: name identifying the load
    :ivar xcSet: set that contains the elements (shells and/or beams)
    :ivar soilProp:  instance of the class EarthPressureModel that defines the 
          soil parameters required to calculate the earth pressure
          (K:coefficient of pressure, zGround: global Z coordinate of
          ground level,gammaSoil: weight density of soil, zWater: global Z
          coordinate of groundwater level, gammaWater: weight density of
          water) . soilProp==None if earth thrust is not considered.
    :ivar vDir:unit xc vector defining pressures direction
    :ivar stripLoads: list of instances of the class
          StripLoadOnBackfill to define (if any) strip surcharge loads on
          the backfill (defaults to [], no loads)
    :ivar lineLoads: list of instances of the class
          LineVerticalLoadOnBackfill  to define (if any) line
          surcharge loads acting in vertical direction on the backfill
          (defaults to [], no loads)
    :ivar horzLoads: list of instances of the class
          HorizontalLoadOnBackfill to define (if any) surcharge loads
          acting in horizontal direction on the backfill (defaults to
          [], no loads)


    '''
    def __init__(self,name, xcSet,soilData, vDir):
        super(EarthPressLoad,self).__init__(name,vDir)
        self.xcSet=xcSet
        self.soilData=soilData
        self.stripLoads=list()
        self.lineLoads=list()
        self.horzLoads=list()

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        if self.soilData<>None:
            self.soilData.appendLoadToCurrentLoadPattern(self.xcSet,self.loadVector)
        for stripL in self.stripLoads:
            stripL.appendLoadToCurrentLoadPattern(self.xcSet,self.loadVector)
        for lineL in self.lineLoads:
            lineL.appendLoadToCurrentLoadPattern(self.xcSet,self.loadVector)
        for horzL in self.horzLoads:
            horzL.appendLoadToCurrentLoadPattern(self.xcSet,self.loadVector)

    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        maxValue=0
        if self.soilData<>None:
            zmin=sets.getMinCooNod(self.xcSet,2)
            pmax=self.soilData.getPressure(zmin)
            maxValue=max(maxValue,pmax)
        for stripL in self.stripLoads:
            pmax=stripL.getMaxMagnitude(self.xcSet)
            maxValue=max(maxValue,pmax)
        for lineL in self.lineLoads:
            pmax=lineL.getMaxMagnitude(self.xcSet)
            maxValue=max(maxValue,pmax)
        for horzL in self.horzLoads:
            pmax=horzL.getMaxMagnitude()        
            maxValue=max(maxValue,pmax)
        return maxValue
   

 

# TO DO: change the method in order to be able to append to current load pattern
class StrainLoadOnShells(object):
    '''Strain load applied on the shell elements in xcSet
    
    :ivar name:  name identifying the load
    :ivar xcSet: set that contains the elements
    :ivar DOFstrain: degree of freedom to which apply the strain 
    :ivar strain: strain (e.g.: alpha x deltaT for thermal expansion)

    '''
    def __init__(self,name, xcSet,DOFstrain,strain):
        self.name=name
        self.xcSet=xcSet
        self.strain=strain
        self.DOFstrain=DOFstrain
    
    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the load pattern passed as parameter.'''
        prep=self.xcSet.getPreprocessor
        lcm=load_cases.LoadCaseManager(prep)
        loadPatternName= prep.getLoadHandler.getLoadPatterns.currentLoadPattern
        loadPattern= prep.getLoadHandler.getLoadPatterns[loadPatternName]
        for s in self.xcSet.getSurfaces:
            for e in s.getElements():
                eLoad= loadPattern.newElementalLoad("shell_strain_load")
                eLoad.elementTags= xc.ID([e.tag])
                eLoad.setStrainComp(0,self.DOFstrain,self.strain)
                eLoad.setStrainComp(1,self.DOFstrain,self.strain)
                eLoad.setStrainComp(2,self.DOFstrain,self.strain)
                eLoad.setStrainComp(3,self.DOFstrain,self.strain)

class StrainGradientLoadOnBeams(object):
    '''Strain load applied on the beam elements generated from
    all the lines in the xcSet. 
    '''
    def __init__(self,name, xcSet,strain):
        self.name=name
        self.xcSet=xcSet
        self.strain=strain
    
    def appendLoadToLoadPattern(self,loadPattern):
        ''' Append load to the load pattern passed as parameter.'''
        pDef= xc.DeformationPlane(strain)
        for l in self.xcSet.getLines:
            for e in l.getElements():
                eLoad= loadPattern.newElementalLoad("beam_strain_load")
                eLoad.elementTags= xc.ID([e.tag])
                eleLoad.backEndDeformationPlane= pDef
                eleLoad.frontEndDeformationPlane= pDef
    
