# -*- coding: utf-8 -*-
'''Generation of boundary conditions based on springs.'''

from __future__ import division

__author__= " Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT) "
__copyright__= "Copyright 2017, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es "

import sys
import geom
from model import predefined_spaces
from materials import typical_materials
from model.sets import sets_mng 
from misc_utils import log_messages as lmsg
from postprocess.xcVtk.fields import fields
from postprocess.xcVtk.FE_model import vtk_FE_graphic


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
        self.modelSpace= modelSpace
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
        lstnod=sets_mng.get_lstNod_on_points_fromSet(setEnt)
        return self.applyOnNodesLst(lstnod)
        
    def applyOnNodesInSet(self,setEnt):
        '''create springs in all the nodes included in the set of entities 
        given as parameter
        '''
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
            if(__debug__):
                if((not elem) or (not fixedNode)):
                    AssertionError('Can\'t set bearing on node: '+str(n.tag))
                

    def applyBConNode(self,node):
        '''create spring boundary conditions in the node.
        Return the spring element created.
        '''
        self.createSpringMaterials()
        unusedFixedNode, elem= self.modelSpace.setBearing(node.tag,self.springMat)
        return elem

class ElasticFoundation(object):
    '''Region resting on springs (Winkler elastic foundation)

    :ivar wModulus: Winkler modulus of the foundation (springs in Z direction)
    :ivar cRoz:     fraction of the Winkler modulus to apply for friction in
                the contact plane (springs in X, Y directions)
    :ivar noTensionZ: if True springs in Z direction are made of 
                      no-tension material (defaults to None)
    '''
    def __init__(self,wModulus,cRoz,noTensionZ=False):
        self.wModulus= wModulus
        self.cRoz= cRoz
        self.noTensionZ=noTensionZ
        
    def createMaterials2D(self, preprocessor, name):
        ''' Create the elastic materials for the zero-length elements in a 2D
        space.

        elastic materials (initialize them here and then we apply the 
        corresponding elastic modulus to each node).
        '''
        self.xSpringName= name + '_xSpring'
        self.xSpring=typical_materials.defElasticMaterial(preprocessor,self.xSpringName,0.1)
        self.ySpringName= name + '_ySpring'
        if self.noTensionZ:
            self.ySpring= typical_materials.defElastNoTensMaterial(preprocessor,self.ySpringName,1)
        else:
            self.ySpring= typical_materials.defElasticMaterial(preprocessor,self.ySpringName,1)
            
    def createMaterials3D(self, preprocessor, name):
        ''' Create the elastic materials for the zero-length elements in a 3D
        space.

         elastic materials (initialize them here and then we apply the corresponding elastic modulus to each node).
        '''
        self.xSpringName= name + '_xSpring'
        self.xSpring=typical_materials.defElasticMaterial(preprocessor,self.xSpringName,0.1)
        self.ySpringName= name + '_ySpring'
        self.ySpring=typical_materials.defElasticMaterial(preprocessor,self.ySpringName,0.1)
        self.zSpringName= name + '_zSpring'
        if self.noTensionZ:
            self.zSpring=typical_materials.defElastNoTensMaterial(preprocessor,self.zSpringName,1)
        else:
            self.zSpring=typical_materials.defElasticMaterial(preprocessor,self.zSpringName,1)
            
    def createMaterials(self, preprocessor, name):
        ''' Create the elastic materials for the zero-length elements in a 3D
        space.

         elastic materials (initialize them here and then we apply the corresponding elastic modulus to each node).
        '''
        dimSpace= preprocessor.getNodeHandler.dimSpace # space dimension.
        if(dimSpace==2):
            self.createMaterials2D(preprocessor, name)
        else:
            self.createMaterials3D(preprocessor, name)
            
    def generateSprings(self,xcSet):
        '''Creates the springs at the nodes of the xcSet given as parameter.'''
        self.foundationSet= xcSet #Set with elastic supported elements
        self.springs= list() #spring elements.
        self.foundationSet.resetTributaries()
        self.foundationSet.computeTributaryAreas(False)
        sNod= self.foundationSet.nodes
        preprocessor= self.foundationSet.getPreprocessor
        # Get the dimensions of the space (usually 2 or 3).
        dimSpace= preprocessor.getNodeHandler.dimSpace # space dimension.
        if(dimSpace==2):
            modelSpace= predefined_spaces.getStructuralMechanics2DSpace(preprocessor)
        else:
            modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
        self.createMaterials(preprocessor,self.foundationSet.name)
        idElem= preprocessor.getElementHandler.defaultTag
        for n in sNod:
            arTribNod= n.getTributaryArea()
            self.xSpring.E= self.cRoz*self.wModulus*arTribNod
            if(dimSpace==2): # Two-dimensional problem.
                self.ySpring.E= self.wModulus*arTribNod
                nodeSprings= [self.xSpringName,self.ySpringName]
            else: # Three-dimensional problem.
                self.ySpring.E= self.cRoz*self.wModulus*arTribNod
                self.zSpring.E= self.wModulus*arTribNod
                nodeSprings= [self.xSpringName,self.ySpringName,self.zSpringName]
            Nn= modelSpace.setBearing(n.tag, nodeSprings)
            if __debug__:
                if(not Nn):
                    AssertionError('Can\'t set bearing on node: '+str(n.tag))
                
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
        if __debug__:
            if(not reac):
                AssertionError('Error computing pressures.')

        field= fields.ExtrapolatedScalarField('soilPressure','getProp',self.foundationSet,component=2,fUnitConv= fUnitConv,rgMinMax=rgMinMax)
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        field.display(displaySettings,caption= caption+' '+unitDescription,fileName=fileName)

    def displayMaxPressures(self,modelSpace,analysis,combs,caption,fUnitConv,unitDescription,rgMinMax=None,fileName=None):
        '''
        Calculate and display the maximum earth pressures (Z direction)
        obtained from the group of load combinations passed as paremeter.

        :param analysis: analysis type (e.g. predefined_solutions.simple_static_linear(FEcase))
        
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
        # Calculate max. pressure
        comb_keys=[key for key in combs] 
        for k in comb_keys:
            print(combs[k].name,combs[k].expr)
            modelSpace.removeAllLoadPatternsFromDomain()
            modelSpace.addNewLoadCaseToDomain(combs[k].name,combs[k].expr)
            result= analysis.analyze(1)
            if(result!=0):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; can\'t solve for load case: '+str(combs[k].name)+'.')
                exit(-1)
            reac= self.calcPressures()
            if __debug__:
                if(not reac):
                    AssertionError('Error computing pressures.')
            for n in nodSet:
                prs=n.getProp('soilPressure')[2]
                if prs > n.getProp('maxSoilPressure'):
                    n.setProp('maxSoilPressure',prs)
        # Display max. pressures
        field= fields.ExtrapolatedScalarField(name='maxSoilPressure',functionName='getProp',xcSet=self.foundationSet,component=None,fUnitConv=fUnitConv,rgMinMax=rgMinMax)
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        field.display(displaySettings,caption= caption+' '+unitDescription,fileName=fileName)
        modelSpace.removeLoadCaseFromDomain(combs[k].name)
        

    
        
def spring_bound_cond_anydir(setNodes,orientation,Klist,name):
    '''Apply spring boundary conditions to a set of nodes

    :param setNodes: set of nodes to apply boundary conditions
    :param orientation: (list) of two vectors [x,y] used to orient 
       the zero length element, where: 
       x are the vector components in global coordinates defining 
          local x-axis (optional)
       y: vector components in global coordinates defining a  vector
            that lies in the local x-y plane of the element(optional).
     If the optional orientation vector are not specified, the local
     element axes coincide with the global axes. Otherwise, the local
     z-axis is defined by the cross product between the vectors x 
     and yp specified in the command line.
    :param Klist: stiffness [KX,KY,KZ,KrotX,KrotY,KrotZ]
    :param name: string to name the spring materials
    '''
    prep=setNodes.getPreprocessor
    nodes=prep.getNodeHandler
    constr=prep.getBoundaryCondHandler
    matKX=typical_materials.defElasticMaterial(prep,name+'matKX',Klist[0])
    matKY=typical_materials.defElasticMaterial(prep,name+'matKY',Klist[1])
    matKZ=typical_materials.defElasticMaterial(prep,name+'matKZ',Klist[2])
    matKrotX=typical_materials.defElasticMaterial(prep,name+'matKrotX',Klist[3])
    matKrotY=typical_materials.defElasticMaterial(prep,name+'matKropY',Klist[4])
    matKrotZ=typical_materials.defElasticMaterial(prep,name+'matKrotZ',Klist[5])
    materials=[matKX.name,matKY.name,matKZ.name,matKrotX.name,matKrotY.name,matKrotZ.name]
    for n in setNodes.nodes:
        p=n.getInitialPos3d
        nn=nodes.newNodeXYZ(p.x,p.y,p.z)
        for i in range(6):
            constr.newSPConstraint(nn.tag,i,0.0)
        predefined_spaces.setBearingBetweenNodes(prep,n.tag,nn.tag,materials,orientation)
