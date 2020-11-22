# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import xc
import geom
from actions.earth_pressure import earth_pressure as ep
from model.sets import sets_mng as sets
from misc_utils import log_messages as lmsg
import numpy as np
from actions import load_cases
from actions.imposed_strain import imp_strain as imps 
from misc_utils import log_messages as lmsg

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
    :ivar lstSets: list of sets of elements
    :ivar vAccel:   acceleration vector xc.Vector([ax,ay,az])
    '''
    def __init__(self,name, lstSets, vAccel):
        super(InertialLoad,self).__init__(name,vAccel)
        self.lstSets=lstSets

    def appendLoadToCurrentLoadPattern(self):
        for s in self.lstSets:
            s.createInertiaLoads((-1)*self.loadVector)
'''
    def getMaxMagnitude(self):
#        Return the maximum magnitude of the vector loads
        maxValue=0
        for ms in self.lstMeshSets:
            if 'shell' in ms.elemType.lower():
                val= ms.matSect.getAreaDensity()*self.loadVector.Norm()
            elif 'beam' in ms.elemType.lower():
                val= ms.matSect.getRho()*self.loadVector.Norm()
            maxValue=max(val,maxValue)
        return maxValue
'''
             

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
    '''Uniform load applied on the beam elements in the set passed as 
    parameter.

    :ivar name:       name identifying the load
    :ivar xcSet:      set that contains the elements
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
        for e in self.xcSet.elements:
            if self.refSystem=='Local':
                load= e.vector3dUniformLoadLocal(self.loadVector)
            else:
                load= e.vector3dUniformLoadGlobal(self.loadVector)
 
    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        return self.loadVector.Norm()
        
class UniformLoadOnTrusses(BaseVectorLoad):
    '''Uniform load applied on the truss elements elements in 
    the set passed as parameter. Load is applied as point
    loads in the extremities of the truss element.

    :ivar name:       name identifying the load
    :ivar xcSet:      set that contains the lines
    :ivar loadVector: xc.Vector with the six components of the 
                      uniform load  in global coordinates
                      xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
    '''
    def __init__(self,name, xcSet, loadVector):
        super(UniformLoadOnTrusses,self).__init__(name,loadVector)
        self.xcSet=xcSet

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        for e in self.xcSet.elements:
            L= e.getLength(True); n1=e.getNodes[0]; n2=e.getNodes[1]
            F_nod=self.loadVector*L/2.
            n1.newLoad(F_nod); n2.newLoad(F_nod)
 
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
            nod=[n for n in l.nodes]
            ndistExtr1=[(n.getInitialPos3d).dist(ext1) for n in nod]
            sortNod=[nod for ndistExtr1,nod in sorted(zip(ndistExtr1,nod))]
            lnInfl=[(sortNod[i-1].getInitialPos3d).dist(sortNod[i+1].getInitialPos3d)/2 for i in range(1,len(sortNod)-1)]
            lnInfl.insert(0,(sortNod[0].getInitialPos3d).dist(sortNod[1].getInitialPos3d)/2.0)
            p2=sortNod[-2].getInitialPos3d
            p1=sortNod[-1].getInitialPos3d
            lnInfl.append((p2.dist(p1))/2.0)
#            lnInfl.append((sortNod[len(sortNod)-2].getInitialPos3d).dist(sortNod[len(sortNod)-1].getInitialPos3d)/2.0)
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
        for e in self.xcSet.elements:
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
        for e in aux_set.elements:
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
            areaSet=float(np.sum([e.getArea(False) for e in aux_set.elements]))
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
    :ivar soilData:  instance of the class EarthPressureModel that defines the 
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
        if(self.soilData!=None):
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
        if(self.soilData!=None):
            zmin=sets.get_min_coo_nod(self.xcSet,2)
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
                     0: strain along local x
                     1: strain along local y
                     3: strain along local z
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
        for e in self.xcSet.elements:
            eLoad= loadPattern.newElementalLoad("shell_strain_load")
            eLoad.elementTags= xc.ID([e.tag])
            eLoad.setStrainComp(0,self.DOFstrain,self.strain)
            eLoad.setStrainComp(1,self.DOFstrain,self.strain)
            eLoad.setStrainComp(2,self.DOFstrain,self.strain)
            eLoad.setStrainComp(3,self.DOFstrain,self.strain)

class StrainLoadOnBeams(object):
    '''Strain load applied on the beam elements in xcSet
    '''
    def __init__(self,name, xcSet,strain):
        self.name=name
        self.xcSet=xcSet
        self.strain=strain
    
    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the load pattern passed as parameter.'''
        prep=self.xcSet.getPreprocessor
        lcm=load_cases.LoadCaseManager(prep)
        loadPatternName= prep.getLoadHandler.getLoadPatterns.currentLoadPattern
        loadPattern= prep.getLoadHandler.getLoadPatterns[loadPatternName]
        pDef= xc.DeformationPlane(self.strain)
        for e in self.xcSet.elements:
            eLoad= loadPattern.newElementalLoad("beam_strain_load")
            eLoad.elementTags= xc.ID([e.tag])
            eLoad.backEndDeformationPlane= pDef
            eLoad.frontEndDeformationPlane= pDef
    
class StrainLoadOnTrusses(object):
    '''Strain load applied on the truss elements in xcSet
    '''
    def __init__(self,name, xcSet,strain):
        self.name=name
        self.xcSet=xcSet
        self.strain=strain
    
    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the load pattern passed as parameter.'''
        prep=self.xcSet.getPreprocessor
        lcm=load_cases.LoadCaseManager(prep)
        loadPatternName= prep.getLoadHandler.getLoadPatterns.currentLoadPattern
        loadPattern= prep.getLoadHandler.getLoadPatterns[loadPatternName]
        for e in self.xcSet.elements:
            eLoad= loadPattern.newElementalLoad("truss_strain_load")
            eLoad.elementTags= xc.ID([e.tag])
            eLoad.eps1= self.strain
            eLoad.eps2= self.strain
    
class StrainGradientThermalLoadOnShells(imps.gradThermalStrain):
    '''Apply a thermal gradient between top and bottom faces of the shell 
    elements in xcSet.
   
    :ivar name:  name identifying the load
    :ivar xcSet: set that contains the elements
    :ivar elThick: thickness of elements
    :ivar alpha: Thermal expansion coefficient of material (1/ºC)
    :ivar Ttop: Temperature (ºC) at the top face of elements
    :ivar Tbottom: Temperature (ºC) at the bottom face of elements
    :ivar DOFstrain: degree of freedom to which apply the strain 
                     3: curvature around local x
                     4: curvature around local y
                     5: curvature around local z

    '''
    def __init__(self,name, elemSet,elThick,DOF,alpha,Ttop,Tbottom):
        super(StrainGradientThermalLoadOnShells,self).__init__(elemSet,elThick,DOF,alpha,Ttop,Tbottom)
        self.name=name
    
    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the load pattern passed as parameter.'''
        prep=self.elemSet.getPreprocessor
        lcm=load_cases.LoadCaseManager(prep)
        loadPatternName= prep.getLoadHandler.getLoadPatterns.currentLoadPattern
        loadPattern= prep.getLoadHandler.getLoadPatterns[loadPatternName]
        for e in self.elemSet.elements:
            eLoad= loadPattern.newElementalLoad("shell_strain_load")
            eLoad.elementTags= xc.ID([e.tag])
            eLoad.setStrainComp(0,self.DOF,self.curvature)
            eLoad.setStrainComp(1,self.DOF,self.curvature)
            eLoad.setStrainComp(2,self.DOF,self.curvature)
            eLoad.setStrainComp(3,self.DOF,self.curvature)

class WindLoadOnShells(BaseVectorLoad):
    '''Wind load applied on the beam elements in the set passed as 
    parameter.

    :ivar name:       name identifying the load
    :ivar xcSet:      set that contains the elements
    :ivar windParams: instance of class base_wind.windParams defining 
                      the parameters to calculate wind pressure.
    :ivar Cp: pressure coefficient
    :ivar positFace: if True wind acts against the positive face 
                     of the shell (defaults to True)
    :ivar Gf: gust factor (defaults to 1.0)
    '''
    def __init__(self,name, xcSet, windParams,Cp,positFace=True,Gf=1.0):
        vDirWind=None
        super(WindLoadOnShells,self).__init__(name,vDirWind)
        self.xcSet=xcSet
        self.windParams=windParams
        self.Cp=Cp
        self.positFace=positFace
        self.Gf=Gf

    def appendLoadToCurrentLoadPattern(self):
        for e in self.xcSet.elements:
            zCoo=e.getPosCentroid(True).z
            press=self.windParams.qz(zCoo)*self.Cp*self.Gf
            if self.positFace: press*=-1
            loadVector=xc.Vector([0,0,press])
            e.vector3dUniformLoadLocal(loadVector)
 
class WindLoadOnBeams(BaseVectorLoad):
    '''Wind load applied on the beam elements in the set passed as 
    parameter.

    :ivar name:       name identifying the load
    :ivar xcSet:      set that contains the elements
    :ivar windParams: instance of class base_wind.windParams defining 
                      the parameters to calculate wind pressure.
    :ivar Cp: pressure coefficient
    :ivar vDirWind: unitary xc.Vector in Wind direction (global coordinates)
                         xc.Vector([Wx,Wy,Wz]).
    :ivar Gf: gust factor (defaults to 1.0)
    '''
    def __init__(self,name, xcSet, windParams,Cp,vDirWind,surfExpossed=None,Gf=1.0):
        super(WindLoadOnBeams,self).__init__(name,vDirWind)
        self.xcSet=xcSet
        self.windParams=windParams
        self.Cp=Cp
        self.surfExpossed=surfExpossed
        self.Gf=Gf

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        if self.surfExpossed:
            for e in self.xcSet.elements:
                zCoo=e.getPosCentroid(True).z
                press=self.windParams.qz(zCoo)*self.Cp*self.surfExpossed*self.Gf
                e.vector3dUniformLoadGlobal(self.loadVector*press)
        else:
            for e in self.xcSet.elements:
                zCoo=e.getPosCentroid(True).z
                press=self.windParams.qz(zCoo)*self.Cp*e.getProp('crossSection').h()*self.Gf
                e.vector3dUniformLoadGlobal(self.loadVector*press)
            
    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        if  self.surfExpossed:
            zMax=-1e6
            for e in self.xcSet.elements:
                zMax=max(zMax,e.getPosCentroid(True).z)
            maxValue=self.windParams.qz(zCoo)*self.Cp*self.surfExpossed*self.Gf
        else:
            maxValue=-1e6
            for e in self.xcSet.elements:
                press=abs(self.windParams.qz(zCoo)*self.Cp*e.getProp('crossSection').h()*self.Gf)
                maxValue=max(maxValue,press)
        return maxValue

class WindLoadOnTrusses(BaseVectorLoad):
    '''Wind load applied on the truss elements in the set passed as 
    parameter.

    :ivar name:       name identifying the load
    :ivar xcSet:      set that contains the elements
    :ivar windParams: instance of class base_wind.windParams defining 
                      the parameters to calculate wind pressure.
    :ivar Cp:         pressure coefficient
    :ivar vDirWind: unitary xc.Vector in Wind direction (global coordinates)
                         xc.Vector([Wx,Wy,Wz]).
    :ivar surfExpossed: surface expossed to wind per unit length 
            of beam. If None surfExpossed is taken as the value of method h() 
            of property 'crossSection' for each element.
    :ivar Gf: gust factor (defaults to 1.0)
    '''
    def __init__(self,name, xcSet, windParams,Cp,vDirWind,surfExpossed=None,Gf=1.0):
        super(WindLoadOnTrusses,self).__init__(name,vDirWind)
        self.xcSet=xcSet
        self.windParams=windParams
        self.Cp=Cp
        self.surfExpossed=surfExpossed
        self.Gf=Gf

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        if self.surfExpossed:
            for e in self.xcSet.elements:
                L=e.getLength(True); n1=e.getNodes[0]; n2=e.getNodes[1]
                zCoo=e.getPosCentroid(True).z
                press=self.windParams.qz(zCoo)*self.Cp*self.surfExpossed*self.Gf
                F_nod=self.loadVector*(L*press/2.)
                F_nod6comp=xc.Vector([F_nod[0],F_nod[1],F_nod[2],0.,0.,0.])
                n1.newLoad(F_nod6comp); n2.newLoad(F_nod6comp)
        else:
            for e in self.xcSet.elements:
                L=e.getLength(True); n1=e.getNodes[0]; n2=e.getNodes[1]
                zCoo=e.getPosCentroid(True).z
                press=self.windParams.qz(zCoo)*self.Cp*e.getProp('crossSection').h()*self.Gf
                F_nod=self.loadVector*(L*press/2.)
                F_nod6comp=xc.Vector([F_nod[0],F_nod[1],F_nod[2],0.,0.,0.])
                n1.newLoad(F_nod6comp); n2.newLoad(F_nod6comp)
                

    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        maxValue=0
        if self.surfExpossed:
            for e in self.xcSet.elements:
                L=e.getLength(True); zCoo=e.getPosCentroid(True).z
                f=abs(self.windParams.qz(zCoo)*self.Cp*self.surfExpossed*L/2.)*self.Gf
                maxValue=max(maxValue,f)
        else:
            for e in self.xcSet.elements:
                L=e.getLength(True); zCoo=e.getPosCentroid(True).z
                f=abs(self.windParams.qz(zCoo)*self.Cp*e.getProp('crossSection').h()*L/2.)*self.Gf
                maxValue=max(maxValue,f)
                
        return maxValue


class SlindingVectorLoad(BaseVectorLoad):
    '''Distribute load expressed as a sliding vector over the nodes in 
    a set.

    :ivar name:       name identifying the load
    :ivar xcSet:      set that contains the nodes
    :ivar pntCoord:    (x,y,z) coordinates of a point of the sliding vector
    :ivar loadVector:  xc.Vector(Fx,Fy,Fz,Mx,My,Mz) components of the force sliding vector
    '''
    def __init__(self,name, xcSet, pntCoord,loadVector):
        super(SlindingVectorLoad,self).__init__(name,loadVector)
        self.xcSet=xcSet
        self.pntCoord=pntCoord
        
    def appendLoadToCurrentLoadPattern(self):
        O=geom.Pos3d(self.pntCoord[0],self.pntCoord[1],self.pntCoord[2])
        force=geom.Vector3d(self.loadVector[0],self.loadVector[1],self.loadVector[2])
        moment=geom.Vector3d(self.loadVector[3],self.loadVector[4],self.loadVector[5])
        loadSVS=geom.SlidingVectorsSystem3d(O,force,moment)
        ptList= list()
        nodeList= self.xcSet.nodes
        for n in nodeList:
            ptList.append(n.getInitialPos3d)
        loadVectors= loadSVS.distribute(ptList)
        for n, v in zip(nodeList,loadVectors):
            f= v.getVector3d()
            n.newLoad(xc.Vector([f.x,f.y,f.z,0.0,0.0,0.0]))

        
