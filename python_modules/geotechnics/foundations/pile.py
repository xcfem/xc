# -*- coding: utf-8 -*-
''' Base classes for pile foundations.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import math
from misc_utils import log_messages as lmsg
from materials.sections import section_properties as sp
from materials import typical_materials
from model import predefined_spaces

def get_node_zs(nodes):
    ''' Return a list of tuples containing the (node, z) paris with the z 
        coordinate sorted in descending order.'''
    def takeSecond(tupla):
       return tupla[1]
    retval= [(n,n.get3dCoo[2]) for n in nodes]
    retval.sort(key=takeSecond, reverse=True) # y in descending order
    return retval

def get_node_ys(nodes):
    ''' Return a list of tuples containing the (node, y) paris with the y 
        coordinate sorted in descending order.'''
    def takeSecond(tupla):
       return tupla[1]
    retval= [(n,n.get3dCoo[1]) for n in nodes]
    retval.sort(key=takeSecond, reverse=True) # y in descending order
    return retval

def generate_springs_pile_2d(modelSpace, nodes, linearSpringsConstants):
    '''Generate the springs that simulate the soils along the pile in a 2D
       problem.

    :param modelSpace: PredefinedSpace object used to create the FE model
                       (see predefined_spaces.py).
    :param nodes: nodes that will be attached by the springs.
    :param linearSpringsConstants: dictionary containing the linear spring
                                   constants corresponding to each node.
    '''
    #init spring elastic materials
    prep= modelSpace.preprocessor
    springX= typical_materials.defElasticMaterial(prep,'springX',1e-5)
    springY= typical_materials.defElasticMaterial(prep,'springY',1e-5)
    retval= list() # Spring elements.
    modelSpace= predefined_spaces.getModelSpace(prep)
    for n in nodes:
        if(n.tag in linearSpringsConstants):
            k_i= linearSpringsConstants[n.tag]
            springX.E= k_i[0] # horizontal component.
            springY.E= k_i[1] # vertical component.
            newNode, newElement= modelSpace.setBearing(n.tag, [springX.name, springY.name])
            retval.append(newElement) # append the "spring" element.
    return retval

def generate_springs_pile_3d(modelSpace, nodes, linearSpringsConstants):
    '''Generate the springs that simulate the soils along the pile in a 3D
       problem.

    :param modelSpace: PredefinedSpace object used to create the FE model
                       (see predefined_spaces.py).
    :param nodes: nodes that will be attached by the springs.
    :param linearSpringsConstants: dictionary containing the linear spring
                                   constants corresponding to each node.
    '''
    #init spring elastic materials
    prep= modelSpace.preprocessor
    springX= typical_materials.defElasticMaterial(prep,'springX',1e-5)
    springY= typical_materials.defElasticMaterial(prep,'springY',1e-5)
    springZ= typical_materials.defElasticMaterial(prep,'springZ',1e-5)
    retval= list() # Spring elements.
    modelSpace= predefined_spaces.getModelSpace(prep)
    for n in nodes:
        if(n.tag in linearSpringsConstants):
            k_i= linearSpringsConstants[n.tag]
            springX.E= k_i[0]
            springY.E= k_i[1]
            springZ.E= k_i[2]
            newNode, newElement= modelSpace.setBearing(n.tag,['springX','springY','springZ'])
            retval.append(newElement) # append the "spring" element.
    return retval

class SoilLayers(object):
    ''' Base class for soil horizons in pile computations.

    :ivar waterTableZ: global Z coordinate of the water table. 
    :ivar groundLevel: global Z coordinate of the ground level.
    '''
    def __init__(self, waterTableZ= -100, groundLevel= 0.0):
        ''' Constructor.

        :param waterTableZ: global Z coordinate of the water table.
        :param groundLevel: global Z coordinate of the ground level.
        '''
        self.waterTableZ= waterTableZ
        self.groundLevel= groundLevel

    def submerged(self, z):
        ''' Return true if z<waterTableZ.'''
        return z<self.waterTableZ


class Pile(object):
    '''Pile basic properties.
    
    :ivar crossSection: cross-section of the pile.
    :ivar E: elastic modulus of pile material.
    :ivar pileType: "endBearing" for end bearing piles 
                    "friction" for friction piles
    :ivar soilLayers:  properties of the soils horizons.
    :ivar pileSet: set of nodes and elements defining a single pile.
    '''
    def __init__(self, E, crossSection, pileType, soilLayers, pileSet= None, rho= 2.5e3):
        ''' Constructor.

        :param crossSection: cross-section of the pile.
        :param E: elastic modulus of pile material
        :param pileType: "endBearing" for end bearing piles 
                        "friction" for friction piles
        :param soilLayers:  properties of the soil horizons.
        :param pileSet: set of nodes and elements defining a single pile.
        :param rho: material density.
        '''
        self.E= E
        self.crossSection= crossSection
        self.pileType= pileType
        self.soilLayers= soilLayers
        self.pileSet= None
        if(pileSet):
            self.pileSet= pileSet
        self.rho= rho

     
    def getElasticMaterialData(self, overrideRho= None):
        '''Return the elastic material constitutive model of the pile itself.

        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        materialModelName= 'ElasticMaterialData_'+str(self.E)
        rho= self.rho
        if(overrideRho!=None):
            rho= overrideRho
        return typical_materials.MaterialData(name= materialModelName,E=self.E, nu= 0.2, rho= rho)
    
    def getZMax(self):
        ''' Return the maximum value of z for the pile nodes.'''
        retval= None
        if(self.pileSet):
            retval= max([n.get3dCoo[2] for n in self.pileSet.nodes])
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; pile element set not defined.')
        return retval
    
    def getZMin(self):
        ''' Return the minimum value of z for the pile nodes.'''
        retval= None
        if(self.pileSet):
            retval= min([n.get3dCoo[2] for n in self.pileSet.nodes])
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; pile element set not defined.')
        return retval
            
    def getAerialLength(self):
        '''Return the length of pile above the ground surface'''
        zMax= self.getZMax()
        return max(0,zMax-self.soilLayers.groundLevel)
    
    def getBuriedLength(self):
        '''Return the length of pile below the ground surface'''
        zMax= self.getZMax()
        zMin= self.getZMin()
        return min(zMax-zMin,self.soilLayers.groundLevel-zMin)
    
    def getTotalLength(self):
        '''Return the total length of the pile.'''
        aerL= self.getAerialLength()
        burL= self.getBuriedLength()
        return aerL+burL
    
    def getCrossSectionArea(self):
        '''Return the cross-sectional area of the pile'''
        return self.crossSection.A()
    
    def getPileElasticLengthInClay(self, Eterrain, majorAxis= False):
        ''' Return the equivalent elastic length of the pile embedded in
        cohesive soil.

        :param Eterrain: elastic modulus of the terrain. 
        :param majorAxis: true if the required inertia corresponds to the
                          bending around major axis. 
        '''
        Ipile= self.crossSection.I(majorAxis= majorAxis)
        LePA= math.pow(self.E*Ipile/(Eterrain/3.),0.25)
        if(LePA>2*self.getBuriedLength()):
            lmsg.error("Pile is too short to compute its elastic length.")
        return(LePA)

    def getPileAnchorageLengthInClay(self, Eterrain, majorAxis= False):
        ''' Return the equivalent anchorage length of the pile embedded in
        cohesive soil.

        :param Eterrain: elastic modulus of the terrain. 
        :param majorAxis: true if the required inertia corresponds to the
                          bending around major axis. 
        '''
        return 1.2*self.getPileElasticLengthInClay(Eterrain= Eterrain, majorAxis= majorAxis)

    def computeTributaryLengths(self, initialGeometry= False):
        ''' Compute the tributary lengths for the nodes of the finite
            element set.

        :param initialGeometry: if true compute lengths on the initial geometry
                                of the model. Otherwise use its current 
                                geometry.
        '''
        self.pileSet.resetTributaries()
        self.pileSet.computeTributaryLengths(initialGeometry)

    def getNodeZs(self):
        ''' Return a list of tuples containing the node an its z coordinate
            sorted in descending order.'''
        return get_node_zs(nodes= self.pileSet.nodes)
    
    def getNodeYs(self):
        ''' Return a list of tuples containing the node an its z coordinate
            sorted in descending order.'''
        return get_node_ys(nodes= self.pileSet.nodes)
    
    def getLinearSpringsConstants2D(self, alphaKh_x= 1.0, alphaKh_y= 1.0, alphaKv_z= 1.0):
        '''Compute the spring contants that simulate the soils along the pile 
           for a 2D problem.

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKv_Y: coefficient to be applied to the vertical stiffness of
                          a single pile in Y direction
        '''
        raise Exception('Abstract method, please override')
        
    def getLinearSpringsConstants3D(self, alphaKh_x= 1.0, alphaKh_y= 1.0, alphaKv_z= 1.0):
        '''Compute the spring contants that simulate the soils along the pile 
           for a 3D problem.

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKh_y: coefficient to be applied to the horizontal stiffness
                          of a single pile in Y direction
        :param alphaKh_Z: coefficient to be applied to the vertical stiffness of
                          a single pile in Z direction
        '''
        raise Exception('Abstract method, please override')
    
    def getVerticalStiffnessSinglePile(self):
        '''Return the vertical stiffness of a single pile
        '''
        raise Exception('Abstract method, please override')
        return None
    
    def generateSpringsPile2D(self, alphaKh_x, alphaKv_y):
        '''Generate the springs that simulate the soils along the pile in a 2D
           problem.

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKv_y: coefficient to be applied to the vertical stiffness of
                          a single pile in Y direction
        '''
        # Compute the 3D spring constants and ignore the second horizontal
        # component.
        linearSpringsConstants= self.getLinearSpringsConstants2D(alphaKh_x= alphaKh_x, alphaKv_y= alphaKv_y)
        prep= self.pileSet.getPreprocessor
        modelSpace= predefined_spaces.getModelSpace(prep)
        self.springs= generate_springs_pile_2d(modelSpace= modelSpace, nodes= self.pileSet.nodes, linearSpringsConstants= linearSpringsConstants) 

    
    def generateSpringsPile3D(self, alphaKh_x, alphaKh_y, alphaKv_z):
        '''Generate the springs that simulate the soils along the pile in a 3D
           problem.

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKh_y: coefficient to be applied to the horizontal stiffness
                          of a single pile in Y direction
        :param alphaKh_Z: coefficient to be applied to the vertical stiffness of
                          a single pile in Z direction
        '''
        linearSpringsConstants= self.getLinearSpringsConstants3D(alphaKh_x= alphaKh_x, alphaKh_y= alphaKh_y, alphaKv_z= alphaKv_z)
        prep= self.pileSet.getPreprocessor
        modelSpace= predefined_spaces.getModelSpace(prep)
        self.springs= generate_springs_pile_3d(modelSpace= modelSpace, nodes= self.pileSet.nodes, linearSpringsConstants= linearSpringsConstants)
        
    def generateSpringsPile(self, alphaKh_x, alphaKh_y, alphaKv_z):
        '''Generate the springs that simulate the soils along the pile

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKh_y: coefficient to be applied to the horizontal stiffness
                          of a single pile in Y direction
        :param alphaKh_Z: coefficient to be applied to the vertical stiffness of
                          a single pile in Z direction
        '''
        preprocessor= self.pileSet.getPreprocessor
        numDOFs= preprocessor.getNodeHandler.numDOFs
        if(numDOFs==6): # 3D structural mechanics
            self.generateSpringsPile3D(alphaKh_x= alphaKh_x, alphaKh_y= alphaKh_y, alphaKv_z= alphaKv_z)
        elif(numDOFs==3):
            self.generateSpringsPile2D(alphaKh_x= alphaKh_x, alphaKv_y= alphaKv_z)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for '+str(numDOFs) + ' degrees of freedom.')
            
            

class CircularPile(Pile):
    ''' Pile with circular cross-section.'''
    def __init__(self, E, pileType, diameter, soilLayers, pileSet):
        ''' Constructor.

        :param E: elastic modulus of pile material
        :param pileType: "endBearing" for end bearing piles 
                        "friction" for friction piles
        :param diameter: diameter of the pile cross-section.
        :param soilLayers:  properties of the soil horizons.
        :param pileSet: set of nodes and elements defining a single pile.
        '''
        cs= sp.CircularSection(name= None, Rext= diameter/2.0, Rint=0.0)

        super(CircularPile, self).__init__(E= E, crossSection= cs, pileType= pileType, soilLayers= soilLayers, pileSet= pileSet)

    def getDiameter(self):
        ''' Return the pile diameter.'''
        return self.crossSection.getDiameter()
            
    def defElasticShearSection2d(self, preprocessor, majorAxis= True, overrideRho= None):
        '''elastic section appropriate for 2D beam analysis, including shear deformations

        :param preprocessor: preprocessor object.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        matData= self.getElasticMaterialData(overrideRho= overrideRho)        
        return self.crossSection.defElasticShearSection2d(preprocessor= preprocessor, material= matData, majorAxis= majorAxis, overrideRho= overrideRho)
