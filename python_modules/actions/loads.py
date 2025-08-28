# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato"
__copyright__= "Copyright 2017, AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega.org@gmail.com l.pereztato@ciccp.es"

import sys
import geom
import xc
from model.sets import sets_mng as sets
from misc_utils import log_messages as lmsg
import numpy as np
from actions.imposed_strain import imp_strain as imps 

class BaseVectorLoad(object):
    '''Base class for loads introduced using a load as an xcVector 

    :ivar name: name identifying the load
    :ivar loadVector: load xc.Vector
    '''
    def __init__(self,name, loadVector):
        ''' Constructor.

        :param name: name identifying the load
        :param loadVector: load xc.Vector
        '''
        self.name=name
        self.loadVector= loadVector

    def __mul__(self,factor):
        '''Apply the factor to the load and append it to the current 
           load pattern

        :param factor: factor to apply.
        '''
        self.loadVector=factor*self.loadVector
        self.appendLoadToCurrentLoadPattern()
            
    def __rmul__(self,factor):
        '''Apply the factor to the load and append it to the current 
           load pattern.

        :param factor: factor to apply.
        '''
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
            s.fillDownwards()
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
    :ivar loadVector: xc.Vector with the components of the load: 
                      xc.Vector([Fx,Fy,Fz,Mx,My,Mz]) for 3D elements,
                      xc.Vector([Fx,Fy,Mz]) for 2D elements
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
        self.xcSet.fillDownwards()
        retval= list()
        elements= self.xcSet.elements
        if(elements):
            elementTypes= elements.getTypes()
            firstElementType= elementTypes[0]
            if '3d' in (firstElementType).lower():
                for e in self.xcSet.elements:
                    if self.refSystem=='Local':
                        load= e.vector3dUniformLoadLocal(self.loadVector) # lgtm [py/multiple-definition]
                    else:
                        load= e.vector3dUniformLoadGlobal(self.loadVector) # lgtm [py/multiple-definition]
                    retval.append(load)
            elif '2d' in (firstElementType).lower():
                for e in self.xcSet.elements:
                    if self.refSystem=='Local':
                        load= e.vector2dUniformLoadLocal(self.loadVector) # lgtm [py/multiple-definition]
                    else:
                        load= e.vector2dUniformLoadGlobal(self.loadVector) # lgtm [py/multiple-definition]
                    retval.append(load)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+"; can't deduce if the element the dimension of the element from its type: "+str(firstElementType))
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+"; the set: "+str(self.xcSet.name)+" has no elements. No loads to append.")
            
        return retval
 
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
        self.xcSet.fillDownwards()
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
        self.xcSet.fillDownwards()
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
                n= sortNod[i]
                load= lnInfl[i]*self.loadVector
                n.newLoad(load)

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
        self.xcSet.fillDownwards()
        retval= list()
        for e in self.xcSet.elements:
            if self.refSystem=='Local':
                load=e.vector3dUniformLoadLocal(self.loadVector) # lgtm [py/multiple-definition]
            else:
                load=e.vector3dUniformLoadGlobal(self.loadVector) # lgtm [py/multiple-definition]
            retval.append(load)
        return retval

    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        return self.loadVector.Norm()

class UnifLoadSurfNodesDistributed(BaseVectorLoad):
    '''Distribute uniform load defined on surfaces in surfSet 
    (not necessarily meshed) among the nodes that touch each 
    surface.

    :ivar name:       name identifying the load
    :ivar surfSet:    set that contains the surfaces (not necesarily meshed)
    :ivar loadVector: xc.Vector with the six components of the load: 
                      xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
    '''
    def __init__(self,name, surfSet, loadVector):
        super(UnifLoadSurfNodesDistributed,self).__init__(name,loadVector)
        self.surfSet=surfSet
        
    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        self.surfSet.fillDownwards()
        prep=self.surfSet.getPreprocessor
        if prep.getSets.exists('auxNodSet'): prep.getSets.removeSet('auxNodSet')
        for s in self.surfSet.surfaces:
            lstNodes=list()
            lstNodes+=[n for n in s.nodes]
            for l in s.getEdges: lstNodes+=l.nodes
            for p in s.getVertices: lstNodes+=p.nodes
            if len(lstNodes) > 0:
                surfLoadVect=s.getArea()*self.loadVector
                slv= SlidingVectorLoad('slv',nodes= lstNodes, pntCoord= s.getCentroid(), loadVector= surfLoadVect)
                slv.appendLoadToCurrentLoadPattern()
            
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
        aux_set.fillDownwards()
        retval= list()
        for e in aux_set.elements:
            if self.refSystem=='Local':
                load= e.vector3dUniformLoadLocal(factor*self.loadVector) # lgtm [py/multiple-definition]
            else:
                load= e.vector3dUniformLoadGlobal(factor*self.loadVector) # lgtm [py/multiple-definition]
            retval.append(load)
        aux_set.clear()
        return retval

    def distrParam(self):
        '''Return the set of elements over which to distribute the point load and the 
        coefficient that must be applied to it '''
        aux_set=None
        factor=0.0
        prep=self.xcSet.getPreprocessor
        aux_set=sets.set_included_in_orthoPrism(preprocessor=prep,setInit=self.xcSet,prismBase=self.prismBase,prismAxis=self.prismAxis,setName='aux_set'+self.name)
        if aux_set.getNumElements==0:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; can\'t distribute load: '+str(self.name)+' (no elements in set: '+str(self.xcSet.name)+')')
        else:
            areaSet=float(np.sum([e.getArea(False) for e in aux_set.elements]))
            factor=1.0/areaSet
        return (aux_set,factor)

    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        aux_set,factor=self.distrParam()
        return factor*self.loadVector.Norm()

            
class EarthPressLoad():
    '''Earth pressure applied on the elements (shell or beams)
    included in the set xcSet. 
    
    :ivar name: name identifying the load
    :ivar xcSet: set that contains the elements (shells and/or beams)
    :ivar soilData:  instance of the class EarthPressureModel that defines the 
          soil parameters required to calculate the earth pressure
          (K:coefficient of pressure, zGround: global Z coordinate of
          ground level,gammaSoil: weight density of soil, zWater: global Z
          coordinate of groundwater level, gammaWater: weight density of
          water) . soilProp is None if earth thrust is not considered.
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
#        super(EarthPressLoad,self).__init__(name,vDir)
        self.name=name
        self.xcSet=xcSet
        self.soilData=soilData
        self.vDir=vDir
        self.stripLoads=list()
        self.lineLoads=list()
        self.horzLoads=list()

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        self.xcSet.fillDownwards()
        if(self.soilData!=None):
            # self.soilData.xcSet=self.xcSet; self.soilData.vDir=self.vDir
            self.soilData.appendLoadToCurrentLoadPattern(xcSet=self.xcSet,vDir=self.vDir)
        for stripL in self.stripLoads:
            # stripL.xcSet=self.xcSet ; stripL.vDir=self.vDir
            stripL.appendLoadToCurrentLoadPattern(xcSet= self.xcSet, vDir= self.vDir)
        for lineL in self.lineLoads:
            # lineL.xcSet=self.xcSet; lineL.vDir=self.vDir
            lineL.appendLoadToCurrentLoadPattern(xcSet= self.xcSet, vDir= self.vDir)
        for horzL in self.horzLoads:
            # horzL.xcSet=self.xcSet; horzL.vDir=self.vDir
            horzL.appendLoadToCurrentLoadPattern(xcSet= self.xcSet, vDir= self.vDir)

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
   


#       ___ _            _        _              _    
#      / __| |_ _ _ __ _(_)_ _   | |___  __ _ __| |___
#      \__ \  _| '_/ _` | | ' \  | / _ \/ _` / _` (_-<
#      |___/\__|_| \__,_|_|_||_| |_\___/\__,_\__,_/__/
# Strain loads                                                    

def isotropic_strain_load_on_set(loadPattern, xcSet, strain):
    ''' Defines an isotropic and homogeneous strain load applied in the 
        elements of the set.

    :param loadPattern: load pattern to append the strain loads to.
    :param xcSet: XC set that contains the elements
    :param strain: strain (e.g.: alpha x deltaT for thermal expansion)
    '''
    # Classify the element by type.
    elemByType= dict()
    for e in xcSet.elements:
        eType= e.tipo() # element type
        eTag= e.tag # element identifier
        if(eType in elemByType): # if type already in the dictionary
            elemByType[eType].append(eTag)
        else: # if not
            elemByType[eType]= [eTag]
    # Apply the adequate strain load to each element type.
    for key in elemByType:
        if('Shell' in key): # strain load on shell elements.
            eLoad= loadPattern.newElementalLoad("shell_strain_load")
            eTags= elemByType[key]
            eLoad.elementTags= xc.ID(eTags)
            for dof in [0,1]: # isotropic => same strain for both DOFs.
                eLoad.setStrainComp(0, dof, strain)
                eLoad.setStrainComp(1, dof, strain)
                eLoad.setStrainComp(2, dof, strain)
                eLoad.setStrainComp(3, dof, strain)
        elif('Beam' in key): # strain load on beam elements.
            pDef= xc.DeformationPlane(strain)
            eLoad= loadPattern.newElementalLoad("beam_strain_load")
            eTags= elemByType[key]
            eLoad.elementTags= xc.ID(eTags)
            eLoad.backEndDeformationPlane= pDef
            eLoad.frontEndDeformationPlane= pDef
        elif('Truss' in key): # strain load on beam elements.
            eLoad= loadPattern.newElementalLoad("truss_strain_load")
            eTags= elemByType[key]
            eLoad.elementTags= xc.ID(eTags)
            eLoad.eps1= strain
            eLoad.eps2= strain
        elif('Zero' in key):
            pass # zero lenght are not treated for now.
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; isotropic strain for element type: '"+str(key)+' not implemented yet.')



# TO DO: change the method in order to be able to append to current load pattern
class StrainLoadOnShells(object):
    '''Strain load applied on the shell elements in xcSet
    
    :ivar name:  name identifying the load
    :ivar xcSet: set that contains the elements
    :ivar DOFstrain: list of degrees of freedom to which apply the strain 
                     0: strain along local x
                     1: strain along local y
                     2: strain along local z
    :ivar strain: strain (e.g.: alpha x deltaT for thermal expansion)

    '''
    def __init__(self,name, xcSet, DOFstrain, strain):
        ''' Constructor.

        :param name:  name identifying the load
        :param xcSet: set that contains the elements
        :param DOFstrain: list of degrees of freedom to which apply the strain 
                         0: strain along local x
                         1: strain along local y
                         2: strain along local z
        :param strain: strain (e.g.: alpha x deltaT for thermal expansion)
        '''
        self.name= name
        self.xcSet= xcSet
        self.strain= strain
        if isinstance(DOFstrain, int):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; this syntax will be DEPRECATED; specify the list of degrees of freedom affected by the strain.')
            self.DOFstrains= [DOFstrain]
        else:
            self.DOFstrains= DOFstrain
    
    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the load pattern passed as parameter.'''
        self.xcSet.fillDownwards()
        prep=self.xcSet.getPreprocessor
        loadPatternName= prep.getLoadHandler.getLoadPatterns.currentLoadPattern
        loadPattern= prep.getLoadHandler.getLoadPatterns[loadPatternName]
        eLoad= loadPattern.newElementalLoad("shell_strain_load")
        eLoad.elementTags= self.xcSet.elements.getTags()
        for dof in self.DOFstrains:
            eLoad.setStrainComp(0, dof, self.strain)
            eLoad.setStrainComp(1, dof, self.strain)
            eLoad.setStrainComp(2, dof, self.strain)
            eLoad.setStrainComp(3, dof, self.strain)

class StrainLoadOnBeams(object):
    '''Strain load applied on the beam elements in xcSet
    '''
    def __init__(self,name, xcSet,strain):
        self.name= name
        self.xcSet= xcSet
        self.strain= strain
    
    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the load pattern passed as parameter.'''
        self.xcSet.fillDownwards()
        prep=self.xcSet.getPreprocessor
        loadPatternName= prep.getLoadHandler.getLoadPatterns.currentLoadPattern
        loadPattern= prep.getLoadHandler.getLoadPatterns[loadPatternName]
        pDef= xc.DeformationPlane(self.strain)
        eLoad= loadPattern.newElementalLoad("beam_strain_load")
        eLoad.elementTags= self.xcSet.elements.getTags()
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
        self.xcSet.fillDownwards()
        prep=self.xcSet.getPreprocessor
        loadPatternName= prep.getLoadHandler.getLoadPatterns.currentLoadPattern
        loadPattern= prep.getLoadHandler.getLoadPatterns[loadPatternName]
        eLoad= loadPattern.newElementalLoad("truss_strain_load")
        eLoad.elementTags= self.xcSet.elements.getTags()
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
        self.elemSet.fillDownwards()
        prep=self.elemSet.getPreprocessor
        loadPatternName= prep.getLoadHandler.getLoadPatterns.currentLoadPattern
        loadPattern= prep.getLoadHandler.getLoadPatterns[loadPatternName]
        eLoad= loadPattern.newElementalLoad("shell_strain_load")
        eLoad.elementTags= self.elemSet.elements.getTags()
        eLoad.setStrainComp(0, self.DOF, self.curvature)
        eLoad.setStrainComp(1, self.DOF, self.curvature)
        eLoad.setStrainComp(2, self.DOF, self.curvature)
        eLoad.setStrainComp(3, self.DOF, self.curvature)

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
        self.xcSet.fillDownwards()
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
    def __init__(self,name, xcSet, windParams,Cp,vDirWind,exposedSurf=None,Gf=1.0):
        super(WindLoadOnBeams,self).__init__(name,vDirWind)
        self.xcSet=xcSet
        self.windParams=windParams
        self.Cp=Cp
        self.exposedSurf=exposedSurf
        self.Gf=Gf

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        self.xcSet.fillDownwards()
        if self.exposedSurf:
            for e in self.xcSet.elements:
                zCoo=e.getPosCentroid(True).z
                press=self.windParams.qz(zCoo)*self.Cp*self.exposedSurf*self.Gf
                e.vector3dUniformLoadGlobal(self.loadVector*press)
        else:
            for e in self.xcSet.elements:
                zCoo=e.getPosCentroid(True).z
                press=self.windParams.qz(zCoo)*self.Cp*e.getProp('crossSection').h()*self.Gf
                e.vector3dUniformLoadGlobal(self.loadVector*press)
            
    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        if  self.exposedSurf:
            zMax=-1e6
            for e in self.xcSet.elements:
                zMax=max(zMax,e.getPosCentroid(True).z)
            maxValue=self.windParams.qz(zMax)*self.Cp*self.exposedSurf*self.Gf
        else:
            maxValue=-1e6
            for e in self.xcSet.elements:
                zCoo= e.getPosCentroid(True).z
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
    :ivar exposedSurf: surface expossed to wind per unit length 
            of beam. If None exposedSurf is taken as the value of method h() 
            of property 'crossSection' for each element.
    :ivar Gf: gust factor (defaults to 1.0)
    '''
    def __init__(self,name, xcSet, windParams,Cp,vDirWind,exposedSurf=None,Gf=1.0):
        super(WindLoadOnTrusses,self).__init__(name,vDirWind)
        self.xcSet=xcSet
        self.windParams=windParams
        self.Cp=Cp
        self.exposedSurf=exposedSurf
        self.Gf=Gf

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        self.xcSet.fillDownwards()
        if self.exposedSurf:
            for e in self.xcSet.elements:
                L=e.getLength(True); n1=e.getNodes[0]; n2=e.getNodes[1]
                zCoo=e.getPosCentroid(True).z
                press=self.windParams.qz(zCoo)*self.Cp*self.exposedSurf*self.Gf
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
        if self.exposedSurf:
            for e in self.xcSet.elements:
                L=e.getLength(True); zCoo=e.getPosCentroid(True).z
                f=abs(self.windParams.qz(zCoo)*self.Cp*self.exposedSurf*L/2.)*self.Gf
                maxValue=max(maxValue,f)
        else:
            for e in self.xcSet.elements:
                L=e.getLength(True); zCoo=e.getPosCentroid(True).z
                f=abs(self.windParams.qz(zCoo)*self.Cp*e.getProp('crossSection').h()*L/2.)*self.Gf
                maxValue=max(maxValue,f)
                
        return maxValue


class SlidingVectorLoad(BaseVectorLoad):
    '''Distribute load expressed as a sliding vector over the nodes in 
    a set.

    :ivar name: name identifying the load
    :ivar nodes: nodes to distribute the load on.
    :ivar pntCoord: (x,y,z) coordinates of a point of the sliding vector.
    :ivar loadVector:  xc.Vector(Fx,Fy,Fz,Mx,My,Mz) components of the force sliding vector
    '''
    def __init__(self,name, nodes, pntCoord, loadVector):
        ''' Constructor.

        :param name: name identifying the load
        :param nodes: nodes to distribute the load on.
        :param pntCoord: (x,y,z) coordinates of a point of the sliding vector.
        :param loadVector: xc.Vector(Fx,Fy,Fz,Mx,My,Mz) components of the force sliding vector.
        '''
        super(SlidingVectorLoad,self).__init__(name,loadVector)
        self.loadedNodes= nodes
        self.pntCoord= pntCoord
        
    def appendLoadToCurrentLoadPattern(self):
        ''' Append the loads to the current load pattern.'''
        retval= list()
        nodeList= self.loadedNodes
        if(len(nodeList)>0):
            n0= nodeList[0]
            numDOFs= n0.getNumberDOF
            dimSpace= n0.dim
            if(numDOFs==6):
                O=geom.Pos3d(self.pntCoord[0],self.pntCoord[1],self.pntCoord[2])
                force=geom.Vector3d(self.loadVector[0],self.loadVector[1],self.loadVector[2])
                moment=geom.Vector3d(self.loadVector[3],self.loadVector[4],self.loadVector[5])
                loadSVS= geom.SlidingVectorsSystem3d(O,force,moment)
                ptList= list()
                for n in nodeList:
                    ptList.append(n.getInitialPos3d)
                loadVectors= loadSVS.distribute(ptList)
                for n, v in zip(nodeList,loadVectors):
                    f= v.getVector3d()
                    retval.append(n.newLoad(xc.Vector([f.x,f.y,f.z,0.0,0.0,0.0])))
            elif(numDOFs==3):
                if(dimSpace==2):
                    O= geom.Pos2d(self.pntCoord[0],self.pntCoord[1])
                    force= geom.Vector2d(self.loadVector[0],self.loadVector[1])
                    moment= self.loadVector[2]
                    loadSVS= geom.SlidingVectorsSystem2d(O,force,moment)
                    ptList= list()
                    for n in nodeList:
                        ptList.append(n.getInitialPos2d)
                    loadVectors= loadSVS.distribute(ptList)
                    for n, v in zip(nodeList,loadVectors):
                        f= v.getVector2d()
                        retval.append(n.newLoad(xc.Vector([f.x,f.y,0.0])))
                else:
                    O= geom.Pos3d(self.pntCoord[0],self.pntCoord[1],self.pntCoord[2])
                    force= geom.Vector3d(self.loadVector[0],self.loadVector[1],self.loadVector[2])
                    moment= geom.Vector3d(0.0, 0.0, 0.0)
                    loadSVS= geom.SlidingVectorsSystem3d(O,force,moment)
                    ptList= list()
                    for n in nodeList:
                        ptList.append(n.getInitialPos3d)
                    loadVectors= loadSVS.distribute(ptList)
                    for n, v in zip(nodeList, loadVectors):
                        f= v.getVector3d()
                        retval.append(n.newLoad(xc.Vector([f.x,f.y, f.z])))
            elif(numDOFs==2):
                O= geom.Pos2d(self.pntCoord[0],self.pntCoord[1])
                force= geom.Vector2d(self.loadVector[0],self.loadVector[1])
                moment= self.loadVector[5]
                loadSVS= geom.SlidingVectorsSystem2d(O,force,moment)
                ptList= list()
                for n in nodeList:
                    ptList.append(n.getInitialPos2d)
                loadVectors= loadSVS.distribute(ptList)
                for n, v in zip(nodeList, loadVectors):
                    f= v.getVector2d()
                    retval.append(n.newLoad(xc.Vector([f.x,f.y])))
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; algorithm for '+str(numDOFs)+' degrees of freedom not implemented yet.')
        return retval

        
class MovableLoad(object):
    ''' Load that moves along a row of nodes.

    :ivar P: load value (constant or function of time).
    :ivar v: load speed (constant or function of time).
    :ivar t0: time when the load enters the first node.
    :ivar nodePositions: list of pairs (nodeTag, xNode).
    '''
    def __init__(self, P, v, t0:float, nodes):
        ''' Constructor.

        :param P: load value (constant or function of time).
        :param v: load speed.
        :param t0: time when the load enters the first node.
        :param nodes: list of pairs (nodeTag, xNode).
        '''
        self.P= P
        self.v= v
        self.t0= t0
        tmp= [list(item) for item in zip(*nodes)] # unzip the list.
        self.nodeTags= tmp[0]
        self.nodePositions= tmp[1]

    def getLoadOnNode(self, iNode:int, t):
        ''' Return the value of the load in the node.

        :param iNode: node index.
        :param t: time.
        '''
        retval= 0.0
        vVal= self.v(t) if(callable(self.v)) else self.v # speed value at time t.
        x= vVal*(t-self.t0)
        pVal= self.P(t) if(callable(self.P)) else self.P # load value at time t.
        xNode= self.nodePositions[iNode]
        if(x<xNode): # before the node.
            xPreviousNode= self.nodePositions[iNode-1] if(iNode>0) else xNode 
            if(x>xPreviousNode):
                retval= pVal*(x-xPreviousNode)/(xNode-xPreviousNode)
        elif(x>xNode): # after the node.
            numNodes= len(self.nodePositions)
            xNextNode= self.nodePositions[iNode+1] if(iNode<numNodes-1) else xNode
            if(x<xNextNode):
                retval= pVal*(xNextNode-x)/(xNextNode-xNode)
        else: # in the node.
            retval= pVal
        return retval

    def getNodeLoadHistory(self, tBegin, tEnd, step):
        ''' Compute the load history for each node.

        :param tBegin: start time.
        :param tEnd: end time.
        :param step: time step.
        '''
        # Compute time values.
        ti= np.arange(tBegin, tEnd+step, step)
        loadHistory= dict()
        for nodeTag in self.nodeTags:
            tmp= list()
            loadHistory[nodeTag]= tmp
            targetNode= self.nodeTags.index(nodeTag)
            for t in ti:
                tmp.append(self.getLoadOnNode(iNode= targetNode, t= t))
        return ti, loadHistory
