# -*- coding: utf-8 -*-
from __future__ import division

import xc_base
import geom
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from model.sets import sets_mng 

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
        #Materiales elásticos (los incializamos aquí para luego aplicar el módulo elástico que corresponda a cada nudo)
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
        sNod= self.foundationSet.getNodes
        preprocessor= self.foundationSet.getPreprocessor
        modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
        self.createMaterials(preprocessor,self.foundationSet.name)
        idElem= preprocessor.getElementLoader.defaultTag
        for n in sNod:
            arTribNod=n.getTributaryArea()
            self.xSpring.E= self.cRoz*self.wModulus*arTribNod
            self.ySpring.E= self.cRoz*self.wModulus*arTribNod
            self.zSpring.E= self.wModulus*arTribNod
            nn= modelSpace.setBearing(n.tag,[self.xSpringName,self.ySpringName,self.zSpringName])
            self.springs.append(preprocessor.getElementLoader.getElement(idElem))
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
        self.svdReac= geom.SVD3d()
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
            self.svdReac+= geom.SVD3d(pos,f3d,m3d)

            #print "dispZ= ", n.getDisp[2]*1e3, "mm"
            n.setProp('soilPressure',[f3d.x/a,f3d.y/a,f3d.z/a])
            n.setProp('soilReaction',[f3d.x,f3d.y,f3d.z])
        return self.svdReac.reduceTo(self.getCentroid())

    def displayPressures(self, defDisplay, caption,fUnitConv,unitDescription):
        '''Display foundation pressures.'''
        reac= self.calcPressures()

        field= Fields.ExtrapolatedScalarField('soilPressure','getProp',self.foundationSet,component=2,fUnitConv= fUnitConv)
        field.display(defDisplay,caption= caption+' '+unitDescription)
