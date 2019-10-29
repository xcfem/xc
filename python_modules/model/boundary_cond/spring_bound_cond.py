# -*- coding: utf-8 -*-
from __future__ import division

import math
import xc_base
import geom
from model import predefined_spaces
from materials import typical_materials
from model.sets import sets_mng 
from miscUtils import LogMessages as lmsg
from postprocess.xcVtk.fields import Fields
from postprocess.xcVtk.FE_model import quick_graphics as QGrph
from postprocess.xcVtk.FE_model import vtk_FE_graphic

'''Generation of boundary conditions based on springs 
'''

__author__= " Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT) "
__copyright__= "Copyright 2017, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es "

class SpringBC(object):
    '''Spring to be used as boundary condition

    :ivar name: name
    :ivar modelSpace: model space
    :ivar Kx: spring stiffness in X direction (defaults to 0 -> free)
    :ivar Ky: spring stiffness in Y direction (defaults to 0 -> free)
    :ivar Kz: spring stiffness in Z direction (defaults to 0 -> free)
    '''
    def __init__(self,name,modelSpace,Kx=0,Ky=0,Kz=0):
        self.name=name
        self.modelSpace=modelSpace
        self.Kx=Kx
        self.Ky=Ky
        self.Kz=Kz
        self.springMat=[None,None,None]  #material initialization (no springs)
    
    def createSpringMaterials(self):
        '''create the spring materials in X, Y, Z directions. If there is
        no spring in any of these directions, its corresponding material is
        equal to None 
        '''
        if self.Kx != 0:
            typical_materials.defElasticMaterial(self.modelSpace.preprocessor,self.name + '_xSpring',self.Kx)
            self.springMat[0]=self.name + '_xSpring'
        if self.Ky != 0:
            typical_materials.defElasticMaterial(self.modelSpace.preprocessor,self.name + '_ySpring',self.Ky)
            self.springMat[1]=self.name + '_ySpring'
        if self.Kz != 0:
            typical_materials.defElasticMaterial(self.modelSpace.preprocessor,self.name + '_zSpring',self.Kz)
            self.springMat[2]=self.name + '_zSpring'


    def applyOnPointsInSet(self,setEnt):
        '''create springs in all the points included in the set of entities 
        given as parameter
        '''
        prep=setEnt.getPreprocessor
        lstnod=sets_mng.get_lstNod_on_points_fromSet(setEnt)
        return self.applyOnNodesLst(lstnod)
        
    def applyOnNodesInSet(self,setEnt):
        '''create springs in all the nodes included in the set of entities 
        given as parameter
        '''
        prep=setEnt.getPreprocessor
        lstnod=sets_mng.setNod_to_lst(setEnt)
        self.applyOnNodesLst(lstnod)
        
    def applyOnNodesIn3Dpos(self,lst3DPos):
        '''create springs in the nearest nodes to the coordinates in the 
        list of 3D positions given as parameter
        '''
        lstnod=sets_mng.get_lstNod_from_lst3DPos(self.modelSpace.preprocessor,lst3DPos)
        self.applyOnNodesLst(lstnod)
        
    def applyOnNodesLst(self,Nodelist):
        '''create spring boundary conditions in the nodes included in 
        the list of nodes passed as parameter.
        '''
        self.createSpringMaterials()
        for n in Nodelist:
            fixedNode, elem= self.modelSpace.setBearing(n.tag,self.springMat)

class ElasticFoundation(object):
    '''Region resting on springs (Winkler elastic foundation)

    :ivar wModulus: Winkler modulus of the foundation (springs in Z direction)
    :ivar cRoz:     fraction of the Winkler modulus to apply for friction in
                the contact plane (springs in X, Y directions)
    '''
    def __init__(self,wModulus,cRoz):
        self.wModulus= wModulus
        self.cRoz= cRoz
    def createMaterials(self,preprocessor,name):
        #elastic materials (los incializamos aquí para luego aplicar el módulo elástico que corresponda a cada nudo)
        self.xSpringName= name + '_xSpring'
        self.xSpring=typical_materials.defElasticMaterial(preprocessor,self.xSpringName,0.1)
        self.ySpringName= name + '_ySpring'
        self.ySpring=typical_materials.defElasticMaterial(preprocessor,self.ySpringName,0.1)
        self.zSpringName= name + '_zSpring'
        self.zSpring=typical_materials.defElasticMaterial(preprocessor,self.zSpringName,1)
    def generateSprings(self,xcSet):
        '''Creates the springs at the nodes.'''
        self.foundationSet= xcSet #Set with elastic supported elements
        self.springs= list() #spring elements.
        self.foundationSet.resetTributaries()
        self.foundationSet.computeTributaryAreas(False)
        sNod= self.foundationSet.nodes
        preprocessor= self.foundationSet.getPreprocessor
        modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
        self.createMaterials(preprocessor,self.foundationSet.name)
        idElem= preprocessor.getElementHandler.defaultTag
        for n in sNod:
            arTribNod=n.getTributaryArea()
            self.xSpring.E= self.cRoz*self.wModulus*arTribNod
            self.ySpring.E= self.cRoz*self.wModulus*arTribNod
            self.zSpring.E= self.wModulus*arTribNod
            nn= modelSpace.setBearing(n.tag,[self.xSpringName,self.ySpringName,self.zSpringName])
            self.springs.append(preprocessor.getElementHandler.getElement(idElem))
            idElem+= 1
    def getCentroid(self):
        '''Returns the geometric baricenter of the springs.'''
        dx= 0.0; dy= 0.0; dz= 0.0
        A= 0.0
        for e in self.springs:
            n= e.getNodes[1]
            a= n.getTributaryArea()
            pos= n.getInitialPos3d
            A+= a
            dx+= a*pos[0]; dy+= a*pos[1]; dz+= a*pos[2]
        dx/=A; dy/=A; dz/=A
        return geom.Pos3d(dx,dy,dz)
    def calcPressures(self):
        ''' Foundation pressures over the soil. Calculates pressures
         and forces in the free nodes of the springs
         (those that belongs to both the spring and the foundation)
         and stores these values as properties of those nodes:
         property 'soilPressure:' [xStress,yStress,zStress]
         property 'soilReaction:' [xForce,yForce,zForce]'''
        self.svdReac= geom.SlidingVectorsSystem3d()
        f3d= geom.Vector3d(0.0,0.0,0.0)
        m3d= geom.Vector3d(0.0,0.0,0.0)
        for e in self.springs:
            n= e.getNodes[1]
            rf= e.getResistingForce()
            a= n.getTributaryArea()
            if(len(rf)==6):
                f3d= geom.Vector3d(rf[0],rf[1],0.0)
                m3d= geom.Vector3d(0.0,0.0,rf[2])
            else: #len(rf)==12
                f3d= geom.Vector3d(rf[0],rf[1],rf[2])
                m3d= geom.Vector3d(rf[3],rf[4],rf[5])
            pos= n.getInitialPos3d
            self.svdReac+= geom.SlidingVectorsSystem3d(pos,f3d,m3d)
            n.setProp('soilPressure',[f3d.x/a,f3d.y/a,f3d.z/a])
            n.setProp('soilReaction',[f3d.x,f3d.y,f3d.z])
        return self.svdReac.reduceTo(self.getCentroid())

    def displayPressures(self, caption,fUnitConv,unitDescription,rgMinMax=None,fileName=None):
        '''Display foundation pressures for a single load case.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
              of the scalar field (if any) to be represented. All the values 
              less than vmin are displayed in blue and those greater than vmax 
              in red (defaults to None)
        :param fileName: file name (defaults to None -> screen display)
        '''
        reac= self.calcPressures()

        field= Fields.ExtrapolatedScalarField('soilPressure','getProp',self.foundationSet,component=2,fUnitConv= fUnitConv,rgMinMax=rgMinMax)
        defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
        field.display(defDisplay,caption= caption+' '+unitDescription,fName=fileName)

    def displayMaxPressures(self,FEcase,combs,caption,fUnitConv,unitDescription,rgMinMax=None,fileName=None):
        '''Calculate and display the maximum earth pressures (Z direction)
        obtained from the group of load combinations passed as paremeter.

        :param FEcase: finite element problem
        :param combs: load cases to analyze and compare to obtain the maximum 
                    pressures.
        :param caption: caption text to diaplay.
        :param fUnitConv: factor to apply to results (unit conversion)
        :param unitDescription: text to display as unit description.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
              of the scalar field (if any) to be represented. All the values 
              less than vmin are displayed in blue and those greater than vmax 
              in red (defaults to None)
        :param fileName: file name (defaults to None -> screen display)
        '''
        #Init max. pressures
        nodSet=self.foundationSet.getNodes
        for n in nodSet:
            n.setProp('maxSoilPressure',-1e10)
        #Calculate max. pressures
        for lc in combs:
            lcs=QGrph.LoadCaseResults(FEcase)
            lcs.solve(loadCaseName=combs[lc].name,loadCaseExpr=combs[lc].expr)
            reac= self.calcPressures()
            for n in nodSet:
                prs=n.getProp('soilPressure')[2]
                if prs > n.getProp('maxSoilPressure'):
                    n.setProp('maxSoilPressure',prs)
        #Display max. pressures
        field= Fields.ExtrapolatedScalarField(name='maxSoilPressure',functionName='getProp',xcSet=self.foundationSet,component=None,fUnitConv=fUnitConv,rgMinMax=rgMinMax)
        defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
        field.display(defDisplay,caption= caption+' '+unitDescription,fName=fileName)
        
def takeSecond(elem):
    return elem[1]

class PileFoundation(object):
    '''Pile foundation model according to art. 5.13.1 (single pile) and art. 
    5.13.3. (pile group) of «Guía de cimentaciones para obras de carretera»,  
    by «Ministerio de Fomento».
    
    :ivar pileSet: set of elements defining a single pile
    :ivar pileDiam: diameter of the pile
    :ival E: elastic modulus of pile material
    :ival pileBearingCapacity: total bearing capacity (skin friction + 
                               point bearing capacity ) of the pile.
    :ivar pileType: "endBearing" for end bearing piles 
                    "friction" for friction piles
    :ivar groundLevel: ground elevation
    :ivar soilsProp:  properties of the levels of soil, defined from 
                     top to bottom as a list: 
                     - [[zBottomSoil,typeSoil,propSoil], ...] where,
                     'zBottomSoil' is the global Z coordinate of the 
                     bottom level of the soil,
                     -'typeSoil' is the type od soil: 'sandy' or 'clay'
                     -'propSoil' is the property of the soil:
                            -'nh' for sandy soil, corresponding to the 
                                  compactness of the sandy soil.
                            -'su' for clay soil, corresponding to the 
                                  shear strength of the saturated cohesive soil.
    '''
    def __init__(self,setPile,pileDiam,E,pileType,pileBearingCapacity,groundLevel,soilsProp):
        self.setPile=setPile
        self.pileDiam=pileDiam
        self.E=E
        self.pileType=pileType
        self.pileBearingCapacity=pileBearingCapacity
        self.groundLevel=groundLevel
        self.soilsProp=soilsProp
        self.pileType=pileType

    def getPileAerialLength(self):
        '''Return the length of pile above the ground surface'''
        zMax=max([n.get3dCoo[2] for n in self.setPile.nodes])
        return max(0,zMax-self.groundLevel)

    def getPileBuriedLength(self):
        '''Return the length of pile below the ground surface'''
        zMax=max([n.get3dCoo[2] for n in self.setPile.nodes])
        zMin=min([n.get3dCoo[2] for n in self.setPile.nodes])
        return min(zMax-zMin,self.groundLevel-zMin)
            
    def getCalcLengthEndBearingPile(self):
        '''Return the calculation length for a single end bearing pile 
        '''
        aerL=self.getPileAerialLength()
        burL=self.getPileBuriedLength()
        return aerL+burL

    def getCalcLengthFrictionPile(self):
        '''Return the calculation length for a single friction pile 
        '''
        aerL=self.getPileAerialLength()
        burL=self.getPileBuriedLength()
        return aerL+2/3.*burL
    
    def getCalcLength(self):
        '''Return the calculation length for a single pile'''
        if str.lower(self.pileType[:2]) in 'friction':
            return self.getCalcLengthFrictionPile()
        else:
            return self.getCalcLengthEndBearingPile()

    def getAreaPile(self):
        '''Return the cross-sectional area of the cylindrical pile'''
        return math.pi*self.pileDiam**2/4.

    def getVerticalStiffnessSinglePile(self):
        '''Return the vertical stiffness of a single pile
        '''
        Lc=self.getCalcLength()
        A=self.getAreaPile()
        Kv=1/(self.pileDiam/40/self.pileBearingCapacity+Lc/A/self.E)
        return Kv

    def generateSpringsPile(self,alphaKh_x,alphaKh_y,alphaKv_z):
        '''Generate the springs that simulate the soils along the pile

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKh_y: coefficient to be applied to the horizontal stiffness
                          of a single pile in Y direction
        :param alphaKh_Z: coefficient to be applied to the vertical stiffness of
                          a single pile in Z direction
        '''
        prep=self.setPile.getPreprocessor
        #init spring elastic materials
        springX=typical_materials.defElasticMaterial(prep,'springX',1e-5)
        springY=typical_materials.defElasticMaterial(prep,'springY',1e-5)
        springZ=typical_materials.defElasticMaterial(prep,'springZ',1e-5)
        self.setPile.resetTributaries()
        self.setPile.computeTributaryLengths(False)
        lstNodPile=[(n,n.get3dCoo[2]) for n in self.setPile.nodes]
        lstNodPile.sort(key=takeSecond,reverse=True) #z in descending order
        if self.soilsProp[-1][0] >= lstNodPile[-1][1]:
            self.soilsProp[-1][0]=lstNodPile[-1][1]-1
        zval=lstNodPile[0][1]
        while zval > self.groundLevel:  #aerial zone of pile
            lstNodPile.pop(0)
            zval=lstNodPile[0][1]
        modelSpace= predefined_spaces.getModelSpace(prep)
        #Springs horizontal stiffness
        z=lstNodPile[0][1]
        self.springs= list() #spring elements.
        if self.pileDiam <= 1:
            coefKh=1
        else:
            coefKh=self.pileDiam
        for s in range(len(self.soilsProp)):
            zBottom=self.soilsProp[s][0]
            soilType=self.soilsProp[s][1][:2].lower()
            soilPrp=self.soilsProp[s][2]
            if soilType not in ('sa','cl'):
                lmsg.warning('wrong type of soil')
            while z>zBottom:
                n=lstNodPile[0][0]
                lnTribNod=n.getTributaryLength()
                if soilType == 'sa': #sandy soil
                    Kh_x=alphaKh_x*soilPrp*(self.groundLevel-z)*lnTribNod*coefKh
                    Kh_y=alphaKh_y*soilPrp*(self.groundLevel-z)*lnTribNod*coefKh
                else:
                    Kh_x=75*alphaKh_x*soilPrp*lnTribNod*coefKh
                    Kh_y=75*alphaKh_y*soilPrp*lnTribNod*coefKh
                springX.E=Kh_x
                springY.E=Kh_y
                if len(lstNodPile)==1:
                    #Spring vertical stiffness (end of pile)
                    Kv_end=alphaKv_z*self.getVerticalStiffnessSinglePile()
                    springZ.E=Kv_end
                    nn= modelSpace.setBearing(n.tag,['springX','springY','springZ'])
                    self.springs.append(nn[1])
                    break
                else:
                    nn= modelSpace.setBearing(n.tag,['springX','springY','springZ'])
                    self.springs.append(nn[1])
                    z=lstNodPile[1][1]
                    lstNodPile.pop(0)
        return
            
        
        
