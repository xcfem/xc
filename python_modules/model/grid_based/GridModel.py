# -*- coding: utf-8 -*-

'''gridModelData.py: model generation based on a grid of 3D positions. Data structures.'''

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

import uuid
import geom
import xc
from miscUtils import LogMessages as lmsg
from materials import typical_materials
from materials import parametrosSeccionRectangular
from model import predefined_spaces
from model import define_apoyos
from xcVtk import vtk_grafico_base
from xcVtk.malla_ef import vtk_grafico_ef
from xcVtk import LoadVectorField as lvf
import ijkGrid as grid

class NamedObjectsMap(dict):
  '''dictionary of objects which have a name.'''
  def __init__(self, objs):
    for o in objs:
      self.add(o)
  def add(self,obj):
    self[obj.name]= obj

class MaterialData(object):
  '''Base class to construct some material definition classes
  Attributes:
    name:         name identifying the material or section
    E:            Young’s modulus of the material
    nu:           Poisson’s ratio
    rho:          mass density
  '''
  def __init__(self,name,E,nu,rho):
    self.name= name
    self.E= E
    self.nu= nu
    self.rho= rho

class DeckMaterialData(MaterialData):
  '''Isotropic elastic section-material appropiate for plate and shell analysis
  Attributes:
    name:         name identifying the section
    E:            Young’s modulus of the material
    nu:           Poisson’s ratio
    rho:          mass density
    thickness:    overall depth of the section
  '''
  def __init__(self,name,E,nu,rho,thickness):
    super(DeckMaterialData,self).__init__(name,E,nu,rho)
    self.thickness= thickness
  def getAreaDensity(self):
    '''returns the mass per unit area'''
    return self.rho*self.thickness
  def setup(self,preprocessor):
    '''returns the elastic isotropic section appropiate for plate and shell analysis
    '''
    typical_materials.defElasticMembranePlateSection(preprocessor,self.name,self.E,self.nu,self.getAreaDensity(),self.thickness)

class BeamMaterialData(MaterialData):
  '''Constructs an elastic section appropiate for 3D beam analysis, 
  including shear deformations.
  Attributes:
    name:         name identifying the section
    E:            Young’s modulus of the material
    nu:           Poisson’s ratio
    rho:          mass density
    b:            cross-section width (parallel to local z axis)
    h:            cross-section height (parallel to local y axis)
  '''
  def __init__(self,name,E,nu,rho,b,h):
    super(BeamMaterialData,self).__init__(name,E,nu,rho)
    self.b= b
    self.h= h
  def getLongitudinalDensity(self):
    '''returns the mass per unit length'''
    return self.rho*self.b*self.h
  def setup(self,preprocessor):
    '''returns the elastic section appropiate for 3D beam analysis
    '''
    rs= parametrosSeccionRectangular.RectangularSection('',self.b,self.h,self.E,self.nu)
    typical_materials.defElasticShearSection3d(preprocessor,self.name,rs.A(),rs.E,rs.G(),rs.Iz(),rs.Iy(),rs.J(),rs.alphaZ())

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
    '''Returns an XC set with all the elements in the range list.'''
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
  '''Line defined by a range list, a material and an element type and size.'''
  def __init__(self,name, grid, material,elemType,elemSize):
    super(MaterialLine,self).__init__(name,grid,material,elemType,elemSize)

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
    '''Return a list of the elements of the material line.'''
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
    # Definimos transformaciones geométricas (no está programado, estas líneas son para que funcione por el momento) Preguntar a Luis cómo plantear el asunto de las tranformaciones geométricas 
    trfs= preprocessor.getTransfCooLoader
    self.trYGlobal=trfs.newPDeltaCrdTransf3d('trYGlobal')
    self.trYGlobal.xzVector=xc.Vector([0,1,0]) #dirección del eje Y local (el X local sigue siempre la dirección del eje de la barra)
    self.generateLines(dicLin)
    seedElemLoader= preprocessor.getElementLoader.seedElemLoader
    seedElemLoader.defaultTransformation= 'trYGlobal'
    for key in self:
      self[key].generateMesh(seedElemLoader)

class ConstrainedRanges(IJKRangeList):
  '''Region of constrained nodes
     [uX, uY, uZ,rotX, rotY, rotZ]
     uX, uY, uZ: translations in the X, Y and Z directions; 
     rotX, rotY, rotZ: rotations about the X, Y and Z axis
     -free- means no constraint
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
  Attributes:
    name:     name to identify the region
    wModulus: Winkler modulus of the foundation (springs in Z direction)
    cRoz:     fraction of the Winkler modulus to apply for friction in
              the contact plane (springs in X, Y directions)
  '''
  def __init__(self,name, grid, wModulus, cRoz):
    ''' wModulus: Winkler modulus.
        cRoz: fraction of the Winkler modulus to apply in the contact plane.'''
    super(ElasticFoundationRanges,self).__init__(name,grid,list())
    self.wModulus= wModulus
    self.cRoz= cRoz
  def generateSprings(self,key,dicGeomEnt,kX,kY,kZ):
    cBal= self.wModulus
    cRozam= self.cRoz
    self.springs= list() #Tags of the news springs.
    s= self.getSet(dicGeomEnt)
    for r in self.ranges:
      s.resetTributarias()
      s.calculaAreasTributarias(False)
      sNod=s.getNodes
      idElem= self.grid.prep.getElementLoader.defaultTag
      for n in sNod:
        arTribNod=n.getAreaTributaria()
        kX.E=cRozam*cBal*arTribNod
        kY.E=cRozam*cBal*arTribNod
        kZ.E=cBal*arTribNod
        #print n.tag,arTribNod,kX.E,kY.E,kZ.E
        nn= define_apoyos.defApoyoXYZ(self.grid.prep,n.tag,idElem,'muellX','muellY','muellZ')
        self.springs.append(self.grid.prep.getElementLoader.getElement(idElem))
        idElem+= 1

  def getCDG(self):
    dx= 0.0; dy= 0.0; dz= 0.0
    A= 0.0
    for e in self.springs:
      n= e.getNodes[1]
      a= n.getAreaTributaria()
      pos= n.getInitialPos3d
      A+= a
      dx+= a*pos[0]; dy+= a*pos[1]; dz+= a*pos[2]
    dx/=A; dy/=A; dz/=A
    return geom.Pos3d(dx,dy,dz)

  def calcEarthPressures(self):
    'foundation pressures over the soil'
    svdReac= geom.SVD3d()
    for e in self.springs:
      n= e.getNodes[1]
      a= n.getAreaTributaria()
      pos= n.getInitialPos3d
       #print "dispZ= ", n.getDisp[2]*1e3, "mm"
      materials= e.getMaterials()
      matX= materials[0]
      matY= materials[1]
      matZ= materials[2]
      Fx= matX.getStress()
      SgX= Fx/a
      Fy= matY.getStress()
      SgY= Fy/a
      Fz= matZ.getStress()
      SgZ= Fz/a
      reac= geom.Vector3d(-Fx,-Fy,-Fz)
      svdReac+= geom.VDesliz3d(pos,reac)
      #print "Fx= ", Fx/1e3, " Fy= ", Fy/1e3, " Fz= ", Fz/1e3
      #print "SgX= ", SgX/1e6, " SgY= ", SgY/1e6, " SgZ= ", SgZ/1e6
      n.setProp("SgX",SgX)
      n.setProp("SgY",SgY)
      n.setProp("SgZ",SgZ)
      n.setProp("Fx",Fx)
      n.setProp("Fy",Fy)
      n.setProp("Fz",Fz)
    #print "reac= ", svdReac/1e3
    return svdReac


class ElasticFoundationRangesMap(NamedObjectsMap):
  '''Constrained ranges dictionary.'''
  def __init__(self,efranges):
    super(ElasticFoundationRangesMap,self).__init__(efranges)
  def generateSprings(self, prep, dicGeomEnt):
    #Apoyo elástico en el terreno
    #Materiales elásticos (los incializamos aquí para luego aplicar el módulo elástico que corresponda a cada nudo)
    self.muellX=typical_materials.defElasticMaterial(prep,'muellX',0.5)
    self.muellY=typical_materials.defElasticMaterial(prep,'muellY',0.5)
    self.muellZ=typical_materials.defElasticMaterial(prep,'muellZ',1)
    for key in self.keys():
      apel= self[key]
      apel.generateSprings(key,dicGeomEnt,self.muellX,self.muellY,self.muellZ)

class LoadBase(object):
  '''Base class for loads.
     Attributes:
     name:     name identifying the load
  '''
  def __init__(self,name):
    self.name= name


class LoadOnSurfaces(LoadBase):
  '''Load over a list of surfaces (defined as range lists). 
  Attributes:
    name:     name identifying the load
    surfaces: list of names of material-surfaces sets, e.g. [deck,wall]'''
  def __init__(self,name, surfaces):
    super(LoadOnSurfaces,self).__init__(name)
    self.surfaces= surfaces



class InertialLoadOnMaterialSurfaces(LoadOnSurfaces):
  '''Inertial load applied to the shell elements belonging to a list of 
  surfaces 
  Attributes:
    name:     name identifying the load
    surfaces: list of names of material-surfaces sets, e.g. [deck,wall]
    accel:    list with the three components of the acceleration vector [ax,ay,az]
  '''
  def __init__(self,name, surfaces, accel):
    super(InertialLoadOnMaterialSurfaces,self).__init__(name, surfaces)
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
  Attributes:
    name:     name identifying the load
    points: list of points (list of geom.Pos3d(x,y,z)) where the load must be applied.
    loadVector: xc.Vector with the six components of the load: xc.Vector([Fx,Fy,Fz,Mx,My,Mz]).
  '''
  def __init__(self,name, points, loadVector):
    super(LoadOnPoints,self).__init__(name)
    self.points= points
    self.loadVector= loadVector

  def appendLoadToLoadPattern(self,nodes,loadPattern):
     for pos in self.points:
       nod= nodes.getDomain.getMesh.getNearestNode(pos)
       loadPattern.newNodalLoad(nod.tag,self.loadVector)

class PressureLoadOnSurfaces(LoadOnSurfaces):
  '''Uniform load applied to shell elements
    Attributes:
    name:       name identifying the load
    surfaces:   lists of grid ranges to delimit the surfaces to
                be loaded
    loadVector: list with the three components of the load vector
  '''
  def __init__(self,name, surfaces, loadVector):
    super(PressureLoadOnSurfaces,self).__init__(name, surfaces)
    self.loadVector= loadVector

  def appendLoadToCurrentLoadPattern(self,dicGeomEnt):
    ''' Append load to the current load pattern.'''
    loadVector= xc.Vector(self.loadVector)
    self.surfaces.appendLoadVectorToCurrentLoadPattern(dicGeomEnt,loadVector)

class EarthPressureOnSurfaces(LoadOnSurfaces):
  '''Earth pressure applied to shell elements
    Attributes:
    name:       name identifying the load
    surfaces:   lists of grid ranges to delimit the surfaces to
                be loaded
    earthPressure: instance of the class EarthPressure, with 
                the following attributes:
                  K:Coefficient of pressure
                  zTerrain:global Z coordinate of ground level
                  gammaTerrain: weight density of soil 
                  zWater: global Z coordinate of groundwater level 
                          (if zGroundwater<minimum z of model => there is no groundwater)
                  gammaWater: weight density of water
                  vDir: unit vector defining pressures direction
   '''
  def __init__(self,name, surfaces, earthPressure):
    super(EarthPressureOnSurfaces,self).__init__(name, surfaces)
    self.earthPressure= earthPressure

  def appendEarthPressureToCurrentLoadPattern(self,dicGeomEnt):
    self.surfaces.appendEarthPressureToCurrentLoadPattern(dicGeomEnt,self.earthPressure)


class StrainLoadOnSurfaces(LoadOnSurfaces):
  '''Strain load over a list of surfaces (defined as range lists).'''
  def __init__(self,name, surfaces, epsilon):
    super(StrainLoadOnSurfaces,self).__init__(name, surfaces)
    self.epsilon= epsilon


class StrainGradientLoadOnSurfaces(StrainLoadOnSurfaces):
  '''Strain gradient load over a list of surfaces (defined as range lists).'''
  def __init__(self,name, surfaces,epsilon):
    super(StrainGradientLoadOnSurfaces,self).__init__(name, surfaces, epsilon)

  def appendLoadToLoadPattern(self,lPattern):
    for csup in self.surfaces:
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
  Attributes:
    inercLoad:     list of inertial loads
    unifPressLoad: list of pressures on surfaces
    unifVectLoad:  list of uniform loads on shell elements
    pointLoad:     list of point loads
    earthPressLoad:list of earth pressure loads
    hydrThrustLoad:list of hydrostatic thrust on the walls that delimit a volume
    tempGrad:      list of temperature gradient loads
  '''
  def __init__(self,name, inercLoad= None, unifPressLoad= None, unifVectLoad= None, pointLoad= None, earthPressLoad= None, hydrThrustLoad= None, tempGrad= None):
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
    for load in self.unifPressLoad:
      lmsg.log('unifPressLoad: '+ load.name)
      load.appendLoadToCurrentLoadPattern(dicGeomEnt)
    for load in self.unifVectLoad:
      lmsg.log('unifVectLoad: '+ load.name)
      load.appendLoadToCurrentLoadPattern(dicGeomEnt)


  def appendPunctualLoadsToLoadPattern(self,nodes):
    '''Append punctual loads to the load pattern.'''
    for cpunt in self.pointLoad:
      lmsg.log('pointLoad: '+ cpunt.name)
      cpunt.appendLoadToLoadPattern(nodes,self.lPattern)

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
  Attributes:
    loadStates: list of actions
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
  Attributes:
    FEProblem:  XC finite element problem
  '''
  def __init__(self,FEProblem):
    self.FEProblem= FEProblem

  def getFEProblem(self):
    '''returns the FE problem linked with the grid model'''
    return self.FEProblem
  def getPreprocessor(self):
    '''returs the preprocessor'''
    return self.FEProblem.getPreprocessor

  def newMaterialSurface(self,name,material,elemType,elemSize):
    '''returns a type of surface to be discretized from the defined 
    material, type of element and size of the elements.
    Parameters:
      name:     name to identify the type of surface
      material: name of the material that makes up the surface
      elemType: element type to be used in the discretization
      elemSize: mean size of the elements
    '''
    return MaterialSurface(name, self.grid, material,elemType,elemSize)

  def newMaterialLine(self,name,material,elemType,elemSize):
    '''returns a type of line to be discretized from the defined 
    material, type of element and size of the elements.
    Parameters:
      name:     name to identify the surface
      material: name of the material that makes up the surface
      elemType: element type be used in the discretization
      elemSize: mean size of the elements
    '''
    return MaterialLine(name, self.grid, material,elemType,elemSize)

  def setMaterials(self,materialDataList):
    '''returns the dictionary of materials contained in the list
    given as a parameter
    '''
    self.materialData= MaterialDataMap(materialDataList)
    return self.materialData

  def setMaterialSurfacesMap(self,materialSurfaceList):
    '''returns the dictionary of the material-surfaces contained in the list
    given as a parameter
    '''
    self.conjSup= MaterialSurfacesMap(materialSurfaceList)
    return self.conjSup

  def setMaterialLinesMap(self,materialLineList):
    '''returns the dictionary of the material-lines contained in the list
    given as a parameter
    '''
    self.conjLin= MaterialLinesMap(materialLineList)
    return self.conjLin

  def getElements(self, nombreConj):
    '''Return a list of the elements of the material surfaces and linew.'''
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
    '''Returns the grid of coordinates in the three axes
    Parameters:
      xList: list of coordinates in global X axis
      yList: list of coordinates in global Y axis  
      zList: list of coordinates in global Z axis  
    '''
    self.grid= grid.ijkGrid(self.getPreprocessor(),xList,yList,zList)
    return self.grid

  def getSurfacesFromListOfRanges(self,listOfRanges):
    '''Returns surfaces (IJKRangeList object) created from the list of ranges.'''
    sf= list()
    for r in listOfRanges:
      sf.append(grid.IJKRange(r[0],r[1]))
    unique_name = str(uuid.uuid4())
    return IJKRangeList(unique_name,self.grid,sf)

  def newConstrainedRanges(self,name,constraints):
    return ConstrainedRanges(name, self.grid, constraints)

  def setConstrainedRangesMap(self,constrainedRangesList):
    self.constrainedRanges= ConstrainedRangesMap(constrainedRangesList)
    return self.constrainedRanges

  def newElasticFoundationRanges(self,name, wModulus, cRoz):
    '''Returns a region resting on springs (Winkler elastic foundation)
    Parameters:
      name:     name to identify the region
      wModulus: Winkler modulus of the foundation (springs in Z direction)
      cRoz:     fraction of the Winkler modulus to apply for friction in
                the contact plane (springs in X, Y directions)
    '''
    return ElasticFoundationRanges(name, self.grid, wModulus, cRoz)

  def setElasticFoundationRangesMap(self,elasticFoundationRangesList):
    '''Returns a dictionary with the list of Winkler elastic foundations
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
    self.dicGeomEnt= dict() #Surfaces dictionary.
    self.conjSup.generateMesh(self.getPreprocessor(),self.dicGeomEnt)
    self.dicLin= dict() #Lines dictionary.
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
    '''Append the loads for each load state into corresponding load patterns
       and returns a dictionary with identifiers and the geometric
       entities (lines and surfaces) generated.
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

  def displayLoad(self,setToDisplay=None,loadCaseNm='',unitsScale=1.0,vectorScale=1.0,multByElemArea=False,viewNm="XYZPos",caption= '',fileName=None):
    '''vector field display of the loads applied to the chosen set of elements 
    in the load case passed as parameter
    Parameters:
      setToDisplay:   set of elements to be displayed (defaults to total set)
      loadCaseNm:     name of the load case to be depicted
      unitsScale:     factor to apply to the results if we want to change
                      the units.
      vectorScale:    factor to apply to the vectors length in the 
                      representation
      
      multByElemArea: boolean value that must be 'True' if we want to 
                      represent the total load on each element 
                      (=load multiplied by element area) and 'False' if we 
                      are going to depict the value of the uniform load 
                      per unit area
      viewNm:         name of the view  that contains the renderer (possible
                      options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                      "ZPos", "ZNeg")
      fileName:       full name of the graphic file to generate. Defaults to 
                      None, in this case it returns a console output graphic.
      caption:        text to display in the graphic 
    '''
    if setToDisplay == None:
        setToDisplay=self.getPreprocessor().getSets.getSet('total')
        setToDisplay.fillDownwards()

    defGrid= vtk_grafico_base.RecordDefGrid()
    defGrid.xcSet=setToDisplay
    vField=lvf.LoadVectorField(loadCaseNm,unitsScale,vectorScale)
    vField.multiplyByElementArea=multByElemArea
    vField.dumpLoads(self.getPreprocessor())
    defDisplay= vtk_grafico_ef.RecordDefDisplayEF()
    defDisplay.viewName= viewNm
    defDisplay.defineEscenaMalla(defGrid,None) 
    vField.addToDisplay(defDisplay)
    defDisplay.displayScene(caption,fileName)
    return defDisplay





