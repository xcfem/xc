# -*- coding: utf-8 -*-

'''gridModelData.py: model generation based on a grid of 3D positions. Data structures.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

import uuid
import geom
import xc
from miscUtils import LogMessages as lmsg
from materials import typical_materials
from materials import paramRectangularSection
from model import predefined_spaces
from model import ElasticFoundation as ef
from xcVtk import vtk_grafico_base
from xcVtk.malla_ef import vtk_grafico_ef
from xcVtk.malla_ef import Fields
from xcVtk import LoadVectorField as lvf
from xcVtk import LocalAxesVectorField as lavf
from xcVtk.malla_ef import QuickGraphics as qg
import ijkGrid as grid
import math

class NamedObjectsMap(dict):
  '''dictionary of objects which have a name.'''
  def __init__(self, objs):
    for o in objs:
      self.add(o)
  def add(self,obj):
    self[obj.name]= obj

class DeckMaterialData(object):
  '''Isotropic elastic section-material appropiate for plate and shell analysis
  
  :ivar name:         name identifying the section
  :ivar thickness:    overall depth of the section
  :ivar material:     instance of a class that defines the elastic modulus, shear modulus
                      and mass density of the material
  '''
  def __init__(self,name,thickness,material):
    self.name=name
    self.thickness= thickness
    self.material=material
  def getAreaDensity(self):
    ''':returns: the mass per unit area'''
    return self.material.rho*self.thickness
  def setup(self,preprocessor):
    ''':returns: the elastic isotropic section appropiate for plate and shell analysis
    '''
    typical_materials.defElasticMembranePlateSection(preprocessor,self.name,self.material.E,self.material.nu,self.getAreaDensity(),self.thickness)

class BeamMaterialData(object):
  '''Constructs an elastic section appropiate for 3D beam analysis, including shear deformations.
  
  :ivar name:         name identifying the section
  :ivar section:      instance of a class that defines the geometric and
                      mechanical characteristiscs of a section
                      e.g: RectangularSection, CircularSection, ISection, ...
  :ivar material:     instance of a class that defines the elastic modulus, shear modulus
                      and mass density of the material
  '''
  def __init__(self,name,section,material):
    self.name=name
    self.section=section
    self.material=material
  def getLongitudinalDensity(self):
    ''':returns: the mass per unit length'''
    return self.material.rho*self.section.A()
  def setup(self,preprocessor):
    ''':returns: the elastic section appropiate for 3D beam analysis
    '''
    typical_materials.defElasticShearSection3d(preprocessor,self.name,self.section.A(),self.material.E,self.material.G(),self.section.Iz(),self.section.Iy(),self.section.J(),self.section.alphaZ())

class MaterialDataMap(NamedObjectsMap):
  '''Material data dictionary.'''
  def __init__(self,materialDataList):
    super(MaterialDataMap,self).__init__(materialDataList)
  def setup(self, preprocessor):
    for key in self:
      self[key].setup(preprocessor)

class IJKRangeList(object):
  ''' a named IJK range list.'''
  def __init__(self, name, grid, ranges= None):
    self.name= name
    self.grid= grid
    if(ranges is None):
      self.ranges= list() 
    else:
      self.ranges= ranges

  def getPreprocessor(self):
    return self.grid.prep

  def append(self,ijkRange):
    self.ranges.append(ijkRange)

  def appendRanges(self,ijkRangeList):
    if(ijkRangeList.grid==self.grid):
      self.ranges.extend(ijkRangeList.ranges)
    else:
      lmsg.error("Error: reference grids are not the same.")

  def appendSurfaceListRanges(self,surfaces):
    for s in surfaces:
      self.appendRanges(s)

  def appendLoadVectorToCurrentLoadPattern(self, dicGeomEnt, loadVector):
    '''append the load to the current load pattern.'''
    i= 0
    for r in self.ranges:
      nmbrSet=self.name+'LoadVector'+str(i)
      s= self.grid.appendLoadInRangeToCurrentLoadPattern(r,dicGeomEnt,nmbrSet,loadVector)
      i+= 1
    return i

  def appendEarthPressureToCurrentLoadPattern(self, dicGeomEnt, earthPressure):
    i= 0
    for r in self.ranges:
      nmbrSet=self.name+'EarthPressure'+str(i)
      s= self.grid.appendEarthPressureToCurrentLoadPattern(r,dicGeomEnt,nmbrSet,earthPressure)
      i+= 1
    return i

  def getSet(self,dicGeomEnt):
    ''':Returns: an XC set with all the elements in the range list.'''
    setName= self.name + '_xcSet'
    xcSets= self.getPreprocessor().getSets
    retval= xcSets.defSet(setName)
    for r in self.ranges:
      tmpName= setName+'_tmp'
      s= self.grid.getSetInRange(r,dicGeomEnt,tmpName)
      retval.append(s)
      xcSets.removeSet(tmpName)
    return retval


def getIJKRangeListFromSurfaces(surfaces):
  '''generates the list of ranges that delimite all the surfaces given as
  a parameter

  :param surfaces:  list of objects of type  ``MaterialSurface`` 
  :returns: object of type ``IJKRangeList``
  '''
  retval= None
  if(surfaces):
    retval= IJKRangeList(surfaces[0].name+'IJKRangeList',surfaces[0].grid)
    retval.appendSurfaceListRanges(surfaces)
  else:
    lmsg.error('Error: surface list empty.')
  return retval

class MaterialBase(IJKRangeList):
  '''Base class for lines and surfaces defined by a range list, a material and an element type and size.'''
  def __init__(self,name, grid, material,elemType,elemSize):
    super(MaterialBase,self).__init__(name,grid)
    self.material= material
    self.elemType= elemType
    self.elemSize= elemSize


class MaterialSurface(MaterialBase):
  '''Surface defined by a range list, a material and an element type and size.'''
  def __init__(self,name, grid, material,elemType,elemSize):
    super(MaterialSurface,self).__init__(name,grid,material,elemType,elemSize)

  def generateAreas(self, dicGeomEnt):
    self.lstSup= list()
    for ijkRange in self.ranges:
      self.lstSup+= self.grid.generateAreas(ijkRange,dicGeomEnt)
    for s in self.lstSup:
       s.setElemSizeIJ(self.elemSize,self.elemSize)

  def generateMesh(self, seedElementLoader):
    seedElementLoader.defaultMaterial= self.material.name
    elem= seedElementLoader.newElement(self.elemType,xc.ID([0,0,0,0]))
    for s in self.lstSup:
      s.genMesh(xc.meshDir.I)

  def appendVector3dUniformLoadGlobalToCurrentLoadPattern(self,loadVector):
    for s in self.lstSup:
      elSup=s.getElements()
      for el in elSup:
        elementLoad= el.vector3dUniformLoadGlobal(loadVector)    

  def appendStrainGradientToLoadPattern(self,loadPattern,nabla):
    for s in self.lstSup:
      elSup=s.getElements()
      for el in elSup:
        eleLoad= loadPattern.newElementalLoad("shell_strain_load")
        eleLoad.elementTags= xc.ID([el.tag])
        eleLoad.setStrainComp(0,3,nabla)
        eleLoad.setStrainComp(1,3,nabla)
        eleLoad.setStrainComp(2,3,nabla)
        eleLoad.setStrainComp(3,3,nabla)
  def getElements(self):
    '''Return a list of the elements of the material surface.'''
    retval= []
    for sup in self.lstSup:
      elSup= sup.getElements()
      for elem in elSup:
        retval.append(elem)
    return retval
  def getElementsPart(self):
    '''DEPRECATED (use getElements); return a list of the elements of the material surface.'''
    lmsg.warning('MaterialSurface.getElementsPart DEPRECATED; use getElements.')
    return self.getElements()

class MaterialLine(MaterialBase):
  '''Line defined by a range list, a material and an element type, size and direction vector.

  :ivar name:     name to identify the material-line
  :ivar material: name of the material that makes up the line
  :ivar elemType: element type to be used in the discretization
  :ivar elemSize: mean size of the elements
  :ivar vDirLAxY: direction vector for the element local axis Y 
  :ivar ranges:   lists of grid ranges to delimit the lines of the type in question
'''
  def __init__(self,name, grid, material,elemType,elemSize,vDirLAxY):
    super(MaterialLine,self).__init__(name,grid,material,elemType,elemSize)
    self.vDirLAxY=vDirLAxY
  def generateLines(self, dicLin):
    self.lstLines= list()
    for ijkRange in self.ranges:
      self.lstLines+= self.grid.generateLines(ijkRange,dicLin)
    for s in self.lstLines:
       s.setElemSize(self.elemSize)

  def generateMesh(self, seedElementLoader):
    seedElementLoader.defaultMaterial= self.material.name
    elem= seedElementLoader.newElement(self.elemType,xc.ID([0,0]))
    for l in self.lstLines:
      l.genMesh(xc.meshDir.I)

  def getElements(self):
    ''':returns: a list of the elements of the material line.'''
    retval= []
    for lin in self.lstLines:
      elLin= lin.getElements()
      for elem in elLin:
        retval.append(elem)
    return retval
  def getElementsPart(self):
    '''DEPRECATED (use getElements); return a list of the elements of the material line.'''
    lmsg.warning('MaterialLine.getElementsPart DEPRECATED; use getElements.')
    return self.getElements()
  
class MaterialSurfacesMap(NamedObjectsMap):
  '''MaterialSurfaces dictionary.'''
  def __init__(self,surfaces):
    super(MaterialSurfacesMap,self).__init__(surfaces)
  def generateAreas(self, dicGeomEnt):
    for key in self:
      self[key].generateAreas(dicGeomEnt)
  def generateMesh(self, preprocessor, dicGeomEnt, firstElementTag= 1):
    self.generateAreas(dicGeomEnt)
    preprocessor.getCad.getSurfaces.conciliaNDivs()
    seedElemLoader= preprocessor.getElementLoader.seedElemLoader
    seedElemLoader.defaultTag= firstElementTag
    for key in self:
      self[key].generateMesh(seedElemLoader)

class MaterialLinesMap(NamedObjectsMap):
  '''MaterialLines dictionary.'''
  def __init__(self,surfaces):
    super(MaterialLinesMap,self).__init__(surfaces)
  def generateLines(self, dicLin):
    for key in self:
      self[key].generateLines(dicLin)
  def generateMesh(self, preprocessor, dicLin):
    trfs= preprocessor.getTransfCooLoader
    self.trYGlobal=trfs.newPDeltaCrdTransf3d('trYGlobal')
    self.generateLines(dicLin)
    seedElemLoader= preprocessor.getElementLoader.seedElemLoader
    for key in self:
      self.trYGlobal.xzVector=self[key].vDirLAxY
      seedElemLoader.defaultTransformation= 'trYGlobal'
      self[key].generateMesh(seedElemLoader)

class ConstrainedRanges(IJKRangeList):
  '''Constrains in the nodes belonging to the lines in a range-region

  :ivar name:  name to identify the boundary condition
  :ivar grid:  name of the grid-based model
  :ivar constraints: constraint conditions, expressed as [uX, uY, uZ,rotX, rotY, rotZ], where:

       - uX, uY, uZ: translations in the X, Y and Z directions; 
       - rotX, rotY, rotZ: rotations about the X, Y and Z axis
       - 'free': means no constraint
  '''
  def __init__(self,name, grid, constraints):
    super(ConstrainedRanges,self).__init__(name,grid,list())
    self.constraints= constraints
  def generateContraintsInLines(self):
    constraints= self.grid.prep.getConstraintLoader
    for rng in self.ranges:
      lstLin= self.grid.getLstLinRange(rng)
      for l in lstLin:
        setnod=l.getNodes()
        for n in setnod:
          for i in range(0,6):
            coac= self.constraints[i]
            if(coac <> 'free'):
              constraints.newSPConstraint(n.tag,i,coac) 


class ConstrainedRangesMap(NamedObjectsMap):
  '''Constrained ranges dictionary.'''
  def __init__(self,cranges):
    super(ConstrainedRangesMap,self).__init__(cranges)
  def generateContraintsInLines(self):
    for key in self:
      self[key].generateContraintsInLines()

class ElasticFoundationRanges(IJKRangeList):
  '''Region resting on springs (Winkler elastic foundation)
  
  :ivar name:     name to identify the region
  :ivar wModulus: Winkler modulus of the foundation (springs in Z direction)
  :ivar cRoz:     fraction of the Winkler modulus to apply for friction in the contact plane (springs in X, Y directions)
  '''
  def __init__(self,name, grid, wModulus, cRoz):
    ''' 
    :ivar wModulus: Winkler modulus.
    :ivar cRoz: fraction of the Winkler modulus to apply in the contact plane.'''
    super(ElasticFoundationRanges,self).__init__(name,grid,list())
    self.elasticFoundation= ef.ElasticFoundation(wModulus,cRoz)
  def generateSprings(self,dicGeomEnt):
    '''Creates the springs at the nodes.'''
    self.springs= list() #spring elements.
    s= self.getSet(dicGeomEnt)
    self.elasticFoundation.generateSprings(s)

  def getCDG(self):
    ''':Returns: the geometric baricenter of the springs.'''
    return self.elasticFoundation.getCentroid()

  def calcPressures(self):
    ''' Foundation pressures over the soil. Calculates pressures and forces in the free nodes of the springs (those that belongs to both the spring and the foundation) and stores these values as properties of those nodes:

       - property `soilPressure`:' [xStress,yStress,zStress]
       - property `soilReaction`: [xForce,yForce,zForce]
    '''
    return self.elasticFoundation.calcPressures()

  def displayPressures(self, defDisplay, caption,fUnitConv,unitDescription):
    '''Display foundation pressures.'''
    reac= self.elasticFoundation.displayPressures(defDisplay, caption,fUnitConv,unitDescription)


class ElasticFoundationRangesMap(NamedObjectsMap):
  '''Constrained ranges dictionary.'''
  def __init__(self,efranges):
    super(ElasticFoundationRangesMap,self).__init__(efranges)
  def generateSprings(self, prep, dicGeomEnt):
    #Apoyo elástico en el terreno
    #Materiales elásticos (los incializamos aquí para luego aplicar el módulo elástico que corresponda a cada nudo)
    for key in self.keys():
      apel= self[key]
      apel.generateSprings(dicGeomEnt)

class LoadBase(object):
  '''Base class for loads.
     
  :ivar name:     name identifying the load
  '''
  def __init__(self,name):
    self.name= name


class LoadByLstGridRange(LoadBase):
  '''Load over a list of entities (defined as range lists). 
  
  :ivar name:     name identifying the load
  :ivar lstGridRg:   list of grid ranges to delimit the entities to be loaded
  '''
  def __init__(self,name, lstGridRg):
    super(LoadByLstGridRange,self).__init__(name)
    self.lstGridRg= lstGridRg



class InertialLoadOnMaterialSurfaces(LoadBase):
  '''Inertial load applied to the shell elements belonging to a list of 
  surfaces 
  
  :ivar name:     name identifying the load
  :ivar surfaces: list of names of material-surfaces sets, e.g. [deck,wall]
  :ivar accel:    list with the three components of the acceleration vector [ax,ay,az]
  '''
  def __init__(self,name, surfaces, accel):
    super(InertialLoadOnMaterialSurfaces,self).__init__(name)
    self.surfaces=surfaces
    self.acceleration= accel

  def appendLoadToCurrentLoadPattern(self):
    vectorAceleracion= xc.Vector(self.acceleration)
    for csup in self.surfaces:
      mat= csup.material
      masaUnit= mat.getAreaDensity()
      vectorCarga=masaUnit*vectorAceleracion
      csup.appendVector3dUniformLoadGlobalToCurrentLoadPattern(vectorCarga)

class LoadOnPoints(LoadBase):
  '''Load that acts on one or several points 
  
  :ivar name:     name identifying the load
  :ivar points: list of points (list of geom.Pos3d(x,y,z)) where the load must be applied.
  :ivar loadVector: xc.Vector with the six components of the load: xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
  '''
  def __init__(self,name, points, loadVector):
    super(LoadOnPoints,self).__init__(name)
    self.points= points
    self.loadVector= loadVector

  def appendLoadToLoadPattern(self,nodes,loadPattern):
     for pos in self.points:
       nod= nodes.getDomain.getMesh.getNearestNode(pos)
       loadPattern.newNodalLoad(nod.tag,self.loadVector)


class UniformLoadOnLinesInRange(LoadByLstGridRange):
  '''Uniform load applied to all the lines found in the list of grid ranges
  passed as parameter.
    
  :ivar name:       name identifying the load
  :ivar grid:  name of the grid-based model
  :ivar lstGridRg:  lists of grid ranges to delimit the lines to be loaded
  :ivar loadVector: xc.Vector with the six components of the load: xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
  '''                            
  def __init__(self,name, grid,lstGridRg, loadVector):
    super(UniformLoadOnLinesInRange,self).__init__(name, lstGridRg)
    self.grid=grid
    self.loadVector= loadVector

  def appendLoadToLoadPattern(self,loadPattern):
    for rng in self.lstGridRg:
      lstLin= self.grid.getLstLinRange(rng)
      for l in lstLin:
        setnod=l.getNodes()
        sortNod=list()
        for n in setnod:
          pn=n.getInitialPos3d
          ndistOrig=math.sqrt(pn.x**2+pn.y**2+pn.z**2)
          sortNod.append([ndistOrig,n])
          sortNod.sort()
          #first and last item of the ordered list duplicated for 
          #obtaining the contributing length of nodes
          sortNod.insert(0,sortNod[0])
          sortNod.append(sortNod[len(sortNod)-1])
        for i in range(1,len(sortNod)-1):
           nod=sortNod[i][1]
           precNod=sortNod[i-1][1]
           follwNod=sortNod[i+1][1]
           p1=precNod.getInitialPos3d
           p2=follwNod.getInitialPos3d
           longInfl=(p1.distPos3d(p2))/2.0
           loadPattern.newNodalLoad(nod.tag,longInfl*self.loadVector)


class PressureLoadOnSurfaces(LoadByLstGridRange):
  '''Uniform load applied to shell elements
    
  :ivar name:       name identifying the load
  :ivar lstGridRg:  lists of grid ranges to delimit the surfaces to be loaded
  :ivar loadVector: list with the three components of the load vector
  '''
  def __init__(self,name, lstGridRg, loadVector):
    super(PressureLoadOnSurfaces,self).__init__(name, lstGridRg)
    self.loadVector= loadVector

  def appendLoadToCurrentLoadPattern(self,dicGeomEnt):
    ''' Append load to the current load pattern.'''
    loadVector= xc.Vector(self.loadVector)
    self.lstGridRg.appendLoadVectorToCurrentLoadPattern(dicGeomEnt,loadVector)

class EarthPressureOnSurfaces(LoadByLstGridRange):
  '''Earth pressure applied to shell elements
    
  :ivar name:       name identifying the load
  :ivar lstGridRg:   lists of grid ranges to delimit the surfaces to be loaded
  :ivar earthPressure: instance of the class EarthPressure, with the following attributes:

                  - K:Coefficient of pressure
                  - zGround:global Z coordinate of ground level
                  - gammaSoil: weight density of soil 
                  - zWater: global Z coordinate of groundwater level (if zGroundwater<minimum z of model => there is no groundwater)
                  - gammaWater: weight density of water
                  - vDir: unit vector defining pressures direction
   '''
  def __init__(self,name, lstGridRg, earthPressure):
    super(EarthPressureOnSurfaces,self).__init__(name, lstGridRg)
    self.earthPressure= earthPressure

  def appendEarthPressureToCurrentLoadPattern(self,dicGeomEnt):
    self.lstGridRg.appendEarthPressureToCurrentLoadPattern(dicGeomEnt,self.earthPressure)


class StrainLoadOnSurfaces(LoadByLstGridRange):
  '''Strain load over a list of surfaces (defined as range lists).'''
  def __init__(self,name, lstGridRg, epsilon):
    super(StrainLoadOnSurfaces,self).__init__(name, lstGridRg)
    self.epsilon= epsilon


class StrainGradientLoadOnSurfaces(StrainLoadOnSurfaces):
  '''Strain gradient load over a list of surfaces (defined as range lists).'''
  def __init__(self,name, lstGridRg,epsilon):
    super(StrainGradientLoadOnSurfaces,self).__init__(name, lstGridRg, epsilon)

  def appendLoadToLoadPattern(self,lPattern):
    for csup in self.lstGridRg:
      mat= csup.material
      esp= mat.thickness
      nabla= self.epsilon/esp
      csup.applyStrainGradient(lPattern,nabla)

class LoadOnSurfacesMap(NamedObjectsMap):
  '''Map of inertial load over surfaces.'''
  def __init__(self,efranges):
    super(LoadOnSurfacesMap,self).__init__(efranges)

class LoadState(object):
  '''Definition of the actions to be combined in design situations for 
  performing a limit state analysis
  
  :ivar inercLoad:     list of inertial loads
  :ivar unifPressLoad: list of pressures on surfaces
  :ivar unifVectLoad:  list of uniform loads on shell elements
  :ivar pointLoad:     list of point loads
  :ivar unifLoadLinRng: list of uniform loads on the lines in a list of grid ranges
  :ivar earthPressLoad:list of earth pressure loads
  :ivar hydrThrustLoad:list of hydrostatic thrust on the walls that delimit a volume
  :ivar tempGrad:      list of temperature gradient loads
  '''
  def __init__(self,name, inercLoad= None, unifPressLoad= None, unifVectLoad= None, pointLoad= None, unifLoadLinRng=None,earthPressLoad= None, hydrThrustLoad= None, tempGrad= None):
    self.name= name
    if(inercLoad):
      self.inercLoad= inercLoad
    else:
      self.inercLoad= list()
    if(unifPressLoad):
      self.unifPressLoad= unifPressLoad
    else:
      self.unifPressLoad= list()
    if(unifVectLoad):
      self.unifVectLoad= unifVectLoad
    else:
      self.unifVectLoad= list()
    if(pointLoad):
      self.pointLoad= pointLoad
    else:
      self.pointLoad= list()
    if(unifLoadLinRng):
      self.unifLoadLinRng= unifLoadLinRng
    else:
      self.unifLoadLinRng= list()
    if(earthPressLoad):
      self.earthPressLoad= earthPressLoad
    else:
      self.earthPressLoad= list()
    if(hydrThrustLoad):
      self.hydrThrustLoad= hydrThrustLoad
    else:
      self.hydrThrustLoad= list()
    if(tempGrad):
      self.tempGrad= tempGrad
    else:
      self.tempGrad= list()
    self.lPattern= None #Corresponding load pattern.

  def appendInertialLoadsToCurrentLoadPattern(self):
    for pp in self.inercLoad:
      lmsg.log('inercLoad: '+ pp.name)
      pp.appendLoadToCurrentLoadPattern()

  def appendUniformLoadsToCurrentLoadPattern(self,dicGeomEnt):
#    print 'antes: self.unifPressLoad=', len(self.unifPressLoad)
    for load in self.unifPressLoad:
      lmsg.log('unifPressLoad: '+ load.name)
      load.appendLoadToCurrentLoadPattern(dicGeomEnt)
#    print 'despues: self.unifPressLoad=', len(self.unifPressLoad)
    for load in self.unifVectLoad:
      lmsg.log('unifVectLoad: '+ load.name)
      load.appendLoadToCurrentLoadPattern(dicGeomEnt)


  def appendPunctualLoadsToLoadPattern(self,nodes):
    '''Append punctual loads to the load pattern.'''
    for cpunt in self.pointLoad:
      lmsg.log('pointLoad: '+ cpunt.name)
      cpunt.appendLoadToLoadPattern(nodes,self.lPattern)

  def appendUniformLoadOnLinesInRangeToLoadPattern(self):
    '''uniform loads on the lines in a list of grid ranges to the load pattern.'''
    for unifLin in self.unifLoadLinRng:
      lmsg.log('unifLoadLinRng: '+ unifLin.name)
      unifLin.appendLoadToLoadPattern(self.lPattern)

  def appendEarthPressureLoadsToCurrentLoadPattern(self,dicGeomEnt):
    for ep in self.earthPressLoad:
      lmsg.log('earthPressLoad: '+ ep.name)
      ep.appendEarthPressureToCurrentLoadPattern(dicGeomEnt)

  def appendTemperatureGradientToLoadPattern(self):
    for gt in self.tempGrad:
      lmsg.log('tempGrad: '+ gt.name)
      gt.appendLoadToLoadPattern(self.lPattern)

  def appendLoadsToLoadPattern(self,dicGeomEnt,nodes):
    #Inertial loads
    self.appendInertialLoadsToCurrentLoadPattern()
  
     #Uniform load (fx,fy,fz) on surfaces.
    self.appendUniformLoadsToCurrentLoadPattern(dicGeomEnt)
  
    #Cargas lineales
    #Cargas puntuales
    self.appendPunctualLoadsToLoadPattern(nodes)

    #uniform loads on the lines in a list of grid ranges to the load pattern
    self.appendUniformLoadOnLinesInRangeToLoadPattern()

    #Empuje de tierras
    self.appendEarthPressureLoadsToCurrentLoadPattern(dicGeomEnt)
  
    #Gradiente de temperatura
    self.appendTemperatureGradientToLoadPattern()

  def generateLoadPattern(self,preprocessor,dictGeomEnt,lPatterns):
    lmsg.log('   ***   '+self.name+'   ***   ')
    if(not self.lPattern):
      self.lPattern= lPatterns.newLoadPattern("default",self.name)
      lPatterns.currentLoadPattern= self.name
      self.appendLoadsToLoadPattern(dictGeomEnt,preprocessor.getNodeLoader)
    else:
      lmsg.error('Error load pattern: '+ self.name+ ' already generated.')
    return self.lPattern

class LoadStateMap(NamedObjectsMap):
  '''Dictionary of actions
  
  :ivar loadStates: list of actions
  '''
  def __init__(self,loadStates):
    super(LoadStateMap,self).__init__(loadStates)

  def generateLoadPatterns(self,preprocessor,dicGeomEnt):
    ''' generate a load pattern for each one of the load states. '''
    loadLoader= preprocessor.getLoadLoader
    lPatterns= loadLoader.getLoadPatterns
    ts= lPatterns.newTimeSeries("constant_ts","ts")   #Load modulation.
    lPatterns.currentTimeSeries= "ts"
    retval= dict()
    for key in self.keys():
      loadState= self[key]
      retval[key]= loadState.generateLoadPattern(preprocessor,dicGeomEnt,lPatterns)
    return retval

class GridModel(object):
  '''Class for creating FE models (geometry, materials, mesh, constraints and loads) based on a grid 3D positions.
  
  :ivar FEProblem:  XC finite element problem
  '''
  def __init__(self,FEProblem):
    self.FEProblem= FEProblem
    self.dicGeomEnt= dict() #Surfaces dictionary.
    self.dicLin= dict() #Lines dictionary.
    
  def getFEProblem(self):
    ''':returns: the FE problem linked with the grid model'''
    return self.FEProblem
  def getPreprocessor(self):
    ''':returns: the preprocessor'''
    return self.FEProblem.getPreprocessor

  def newMaterialSurface(self,name,material,elemType,elemSize):
    ''':returns: a type of surface to be discretized from the defined 
    material, type of element and size of the elements.
    
    :param name:     name to identify the type of surface
    :param material: name of the material that makes up the surface
    :param elemType: element type to be used in the discretization
    :param elemSize: mean size of the elements
    '''
    return MaterialSurface(name, self.grid, material,elemType,elemSize)

  def newMaterialLine(self,name,material,elemType,elemSize,vDirLAxY):
    ''':returns: a type of line to be discretized from the defined 
    material, type of element and size of the elements.
    
    :param name:     name to identify the line
    :param material: name of the material that makes up the line
    :param elemType: element type be used in the discretization
    :param elemSize: mean size of the elements
    :param vDirLAxY: direction vector for the element local axis Y (width, nDivIJ)
    '''
    return MaterialLine(name, self.grid, material,elemType,elemSize,vDirLAxY)

  def setMaterials(self,materialDataList):
    ''':returns: the dictionary of materials contained in the list given as a parameter
    '''
    self.materialData= MaterialDataMap(materialDataList)
    return self.materialData

  def setMaterialSurfacesMap(self,materialSurfaceList):
    ''':returns: the dictionary of the material-surfaces contained in the list given as a parameter
    '''
    self.conjSup= MaterialSurfacesMap(materialSurfaceList)
    return self.conjSup

  def setMaterialLinesMap(self,materialLineList):
    ''':returns: the dictionary of the material-lines contained in the list given as a parameter
    '''
    self.conjLin= MaterialLinesMap(materialLineList) 
    return self.conjLin

  def getElements(self, nombreConj):
    ''':returns: a list of the elements of the material surfaces or lines.'''
    retval= list()
    keys= self.conjSup.keys()
    if(nombreConj in keys):
      sup= self.conjSup[nombreConj]
      retval= sup.getElements()
    else:
      keys= self.conjLin.keys()
      if(nombreConj in keys):
        lin= self.conjLin[nombreConj]
        retval= lin.getElements()
      else:
        lmsg.warning('part: '+ nombreConj+ ' not found.')
    return retval

  def getElementsFromParts(self, parts):
    retval= []
    for p in parts:
      retval+= self.getElements(p)
    return retval

  def getSetFromParts(self,setName,parts):
    elems= self.getElementsFromParts(parts)
    # Definimos el conjunto
    st= self.getPreprocessor().getSets.defSet(setName)
    st.clear() #Clean set if exists.
    for e in elems:
      st.getElements.append(e)
    st.fillDownwards()
    return st

  def setGrid(self,xList,yList,zList):
    ''':Returns: the grid of coordinates in the three axes
    
    :param xList: list of coordinates in global X axis
    :param yList: list of coordinates in global Y axis  
    :param zList: list of coordinates in global Z axis  
    '''
    self.grid= grid.ijkGrid(self.getPreprocessor(),xList,yList,zList)
    return self.grid

  def getSurfacesFromListOfRanges(self,listOfRanges):
    ''':Returns: surfaces (IJKRangeList object) created from the list of ranges.'''
    sf= list()
    for r in listOfRanges:
      sf.append(grid.IJKRange(r[0],r[1]))
    unique_name = str(uuid.uuid4())
    return IJKRangeList(unique_name,self.grid,sf)

  def newConstrainedRanges(self,name,constraints):
    '''Creates a new range-region of constained nodes (only those nodes
    that belong to the lines contained in the region)

    :param name:        name to identify the boundary condition
    :param constraints: constraint conditions, expressed as [uX, uY, uZ,rotX, rotY, rotZ], where:

                          - uX, uY, uZ: translations in the X, Y and Z directions; 
                          - rotX, rotY, rotZ: rotations about the X, Y and Z axis
                          - 'free': means no constraint
  
    '''
    return ConstrainedRanges(name, self.grid, constraints)

  def setConstrainedRangesMap(self,constrainedRangesList):
    self.constrainedRanges= ConstrainedRangesMap(constrainedRangesList)
    return self.constrainedRanges

  def newElasticFoundationRanges(self,name, wModulus, cRoz):
    ''':Returns: a region resting on springs (Winkler elastic foundation)
    
    :param name:     name to identify the region
    :param wModulus: Winkler modulus of the foundation (springs in Z direction)
    :param cRoz:     fraction of the Winkler modulus to apply for friction in
                the contact plane (springs in X, Y directions)
    '''
    return ElasticFoundationRanges(name, self.grid, wModulus, cRoz)

  def setElasticFoundationRangesMap(self,elasticFoundationRangesList):
    ''':Returns: a dictionary with the list of Winkler elastic foundations
    given as a parameter
    '''
    self.elasticFoundationRanges= ElasticFoundationRangesMap(elasticFoundationRangesList)
    return self.elasticFoundationRanges

  def setLoadStates(self,loadStateMap):
    self.loadStates= loadStateMap

  def generateMesh(self):
    self.grid.generatePoints() #Key points generation.
    self.materialData.setup(self.getPreprocessor()) #Material definition.
    nodes= self.getPreprocessor().getNodeLoader
    predefined_spaces.gdls_resist_materiales3D(nodes)
    nodes.newSeedNode()
#    self.dicGeomEnt= dict() #Surfaces dictionary.
    if(hasattr(self,'conjSup')):
      self.conjSup.generateMesh(self.getPreprocessor(),self.dicGeomEnt)
#    self.dicLin= dict() #Lines dictionary.
    if(hasattr(self,'conjLin')):
      self.conjLin.generateMesh(self.getPreprocessor(),self.dicGeomEnt)
    if(hasattr(self,'constrainedRanges')):
      self.constrainedRanges.generateContraintsInLines()
    if(hasattr(self,'elasticFoundationRanges')):
      self.elasticFoundationRanges.generateSprings(self.getPreprocessor(),self.dicGeomEnt)
    for setName in self.conjSup: #???
      #vars()[setName]= 
      grid.setEntLstSurf(self.getPreprocessor(),self.conjSup[setName].lstSup,setName)

  def generateLoads(self):
    '''Append the loads for each load state into corresponding load patterns and :returns: a dictionary with identifiers and the geometric entities (lines and surfaces) generated.
    '''
    if(hasattr(self,'loadStates')):
      self.lPatterns= self.loadStates.generateLoadPatterns(self.getPreprocessor(),self.dicGeomEnt)
      for cs in self.conjSup: #???
        nbrset='set'+cs
        self.lPatterns[nbrset]= grid.setEntLstSurf(self.getPreprocessor(),self.conjSup[cs].lstSup,nbrset)
    return self.dicGeomEnt

  def displayMesh(self,partToDisplay,caption= ''):
    defDisplay= vtk_grafico_ef.RecordDefDisplayEF()
    defDisplay.grafico_mef(partToDisplay,caption)
    return defDisplay

  def displayLocalAxes(self,setToDisplay=None,vectorScale=1.0,viewNm="XYZPos",caption= '',fileName=None):
    '''vector field display of the loads applied to the chosen set of elements in the load case passed as parameter
    
    :param setToDisplay:   set of elements to be displayed (defaults to total set)
    :param vectorScale:    factor to apply to the vectors length in the representation
    :param viewNm:         name of the view  that contains the renderer (possible options: "XYZPos", "XPos", "XNeg","YPos", "YNeg", "ZPos", "ZNeg")
    :param fileName:       full name of the graphic file to generate. Defaults to `None`, in this case it returns a console output graphic.
    :param caption:        text to display in the graphic 
    '''
    if(setToDisplay == None):
      setToDisplay=self.getPreprocessor().getSets.getSet('total')
      setToDisplay.fillDownwards()
      lmsg.warning('set to display not defined; using total set.')

    defDisplay= vtk_grafico_ef.RecordDefDisplayEF()
    defDisplay.setupGrid(setToDisplay)
    vField=lavf.LocalAxesVectorField(setToDisplay.name+'_localAxes',vectorScale)
    vField.dumpLocalAxes(setToDisplay)
    defDisplay.viewName= viewNm
    defDisplay.defineEscenaMalla(None) 
    vField.addToDisplay(defDisplay)
    defDisplay.displayScene(caption,fileName)
    return defDisplay

  def displayLoad(self,setToDisplay=None,loadCaseNm='',unitsScale=1.0,vectorScale=1.0,multByElemArea=False,viewNm="XYZPos",caption= '',fileName=None):
    '''vector field display of the loads applied to the chosen set of elements in the load case passed as parameter
    
    :param setToDisplay:   set of elements to be displayed (defaults to total set)
    :param loadCaseNm:     name of the load case to be depicted
    :param unitsScale:     factor to apply to the results if we want to change the units.
    :param vectorScale:    factor to apply to the vectors length in the representation
      
    :param multByElemArea: boolean value that must be `True` if we want to represent the total load on each element (=load multiplied by element area) and `False` if we are going to depict the value of the uniform load per unit area
    :param viewNm:         name of the view  that contains the renderer (possible options: `XYZPos`, `XPos`, `XNeg`,`YPos`, `YNeg`, `ZPos`, `ZNeg`)
    :param fileName:       full name of the graphic file to generate. Defaults to ` None`, in this case it returns a console output graphic.
    :param caption:        text to display in the graphic 
    '''
    if(setToDisplay == None):
      setToDisplay=self.getPreprocessor().getSets.getSet('total')
      setToDisplay.fillDownwards()
      lmsg.warning('set to display not defined; using total set.')

    defDisplay= vtk_grafico_ef.RecordDefDisplayEF()
    defDisplay.setupGrid(setToDisplay)
    vField=lvf.LoadVectorField(loadCaseNm,unitsScale,vectorScale)
    vField.multiplyByElementArea=multByElemArea
    vField.dumpLoads(self.getPreprocessor())
    defDisplay.viewName= viewNm
    defDisplay.defineEscenaMalla(None) 
    vField.addToDisplay(defDisplay)
    defDisplay.displayScene(caption,fileName)
    return defDisplay

#QuickGraphics class definition.
# This class is aimed at providing the user with a quick and easy way to 
# display results (internal forces, displacements) of an user-defined
# load case
class QuickGraphics(qg.QuickGraphics):
  '''This class is aimed at providing the user with a quick and easy way to display results (internal forces, displacements) of an user-defined load case.
  
  :ivar loadCaseName:   name of the load case to be created
  :ivar loadCaseExpr:   expression that defines de load case as a
                   combination of previously defined actions
                   e.g. '1.0*GselfWeight+1.0*GearthPress'
  '''
  def __init__(self,model):
    super(QuickGraphics,self).__init__(model.getFEProblem())
