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
from scipy.constants import g
from misc_utils import log_messages as lmsg
from materials.sections import section_properties as sp
from materials import typical_materials
from model import predefined_spaces
from model.sets import sets_mng 

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

def generate_springs_pile_2d(modelSpace, nodes, linearSpringsConstants, storeAsPileNodeProperties= True):
    '''Generate the springs that simulate the soils along the pile in a 2D
       problem.

    :param modelSpace: PredefinedSpace object used to create the FE model
                       (see predefined_spaces.py).
    :param nodes: nodes that will be attached by the springs.
    :param linearSpringsConstants: dictionary containing the linear spring
                                   constants corresponding to each node.
    :param storeAsPileNodeProperties: if true store the spring constants as
                                      properties of the pile nodes.
    '''
    #init spring elastic materials
    prep= modelSpace.preprocessor
    springX= modelSpace.getMaterialFromName(name= 'generate_springs_pile_2d_springX')
    if(not springX):
        springX= typical_materials.defElasticMaterial(prep,'generate_springs_pile_2d_springX',1e-5)
    springY= modelSpace.getMaterialFromName(name= 'generate_springs_pile_2d_springY')
    if(not springY):
        springY= typical_materials.defElasticMaterial(prep,'generate_springs_pile_2d_springY',1e-5)
    retval= list() # Spring elements.
    for n in nodes:
        if(n.tag in linearSpringsConstants):
            k_i= linearSpringsConstants[n.tag]
            springX.E= k_i[0] # horizontal component.
            springY.E= k_i[1] # vertical component.
            newNode, newElement= modelSpace.setBearing(n.tag, [springX.name, springY.name])
            retval.append(newElement) # append the "spring" element.
            # store spring constants as node properties.
            if(storeAsPileNodeProperties): 
                n.setProp('k_x', k_i[0])
                n.setProp('k_y', k_i[1])
    return retval

def generate_springs_pile_3d(modelSpace, nodes, linearSpringsConstants, storeAsPileNodeProperties= True):
    '''Generate the springs that simulate the soils along the pile in a 3D
       problem.

    :param modelSpace: PredefinedSpace object used to create the FE model
                       (see predefined_spaces.py).
    :param nodes: nodes that will be attached by the springs.
    :param linearSpringsConstants: dictionary containing the linear spring
                                   constants corresponding to each node.
    :param storeAsPileNodeProperties: if true store the spring constants as
                                      properties of the pile nodes.
    '''
    #init spring elastic materials
    prep= modelSpace.preprocessor
    springX= modelSpace.getMaterialFromName(name= 'generate_springs_pile_3d_springX')
    if(not springX):
        springX= typical_materials.defElasticMaterial(prep,'generate_springs_pile_3d_springX',1e-5)
    springY= modelSpace.getMaterialFromName(name= 'generate_springs_pile_3d_springY')
    if(not springY):
        springY= typical_materials.defElasticMaterial(prep,'generate_springs_pile_3d_springY',1e-5)
    springZ= modelSpace.getMaterialFromName(name= 'generate_springs_pile_3d_springZ')
    if(not springZ):
        springZ= typical_materials.defElasticMaterial(prep,'generate_springs_pile_3d_springZ',1e-5)
    retval= list() # Spring elements.
    for n in nodes:
        if(n.tag in linearSpringsConstants):
            k_i= linearSpringsConstants[n.tag]
            springX.E= k_i[0]
            springY.E= k_i[1]
            springZ.E= k_i[2]
            newNode, newElement= modelSpace.setBearing(n.tag,[springX.name, springY.name, springZ.name])
            retval.append(newElement) # append the "spring" element.
            # store spring constants as node properties.
            if(storeAsPileNodeProperties):
                n.setProp('k_x', k_i[0])
                n.setProp('k_y', k_i[1])
                n.setProp('k_z', k_i[2])
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
        self.tributaryLengths= sets_mng.get_tributary_lengths(xcSet= self.pileSet, initialGeometry= initialGeometry) # tributary lengths.

    def getTributaryLength(self, n):
        ''' Return the tributary length corresponding to the given node.

        :param n: node to retrieve the triburary length for.
        '''
        retval= 0.0
        if(hasattr(self, 'tributaryLengths')):
            nTag= n.tag
            if(nTag in self.tributaryLengths):
                retval= self.tributaryLengths[nTag]
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; have not been computed yet. Call computeTributaryLengths to compute them.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return retval

    def getNodeZs(self):
        ''' Return a list of tuples containing the node an its z coordinate
            sorted in descending order.'''
        return get_node_zs(nodes= self.pileSet.nodes)
    
    def getNodeYs(self):
        ''' Return a list of tuples containing the node an its z coordinate
            sorted in descending order.'''
        return get_node_ys(nodes= self.pileSet.nodes)
    
    def getLinearSpringsConstants2D(self, alphaKh_x= 1.0, alphaKv_y= 1.0):
        '''Compute the spring contants that simulate the soils along the pile 
           for a 2D problem.

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKv_Y: coefficient to be applied to the vertical stiffness of
                          a single pile in Y direction
        '''
        raise Exception('Abstract method, please override')

    def getBromsLateralSubgradeReactionModulus(self, E):
        ''' Return the lateral subgrade reaction of the soil according to Broms
        formula (Geotecnia y Cimientos Vol. III Cimentaciones, Excavaciones y 
        Aplicaciones. ISBN: 8472070174, 9788472070172.

        E: modulus of deformation of soil.
        '''
        area= self.getCrossSectionArea()
        diameter= 2*math.sqrt(area/math.pi)
        return 0.75*(E/diameter)
        
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
    
    def generateSpringsPile2D(self, alphaKh_x, alphaKv_y, storeAsPileNodeProperties= True):
        '''Generate the springs that simulate the soils along the pile in a 2D
           problem.

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKv_y: coefficient to be applied to the vertical stiffness of
                          a single pile in Y direction
        :param storeAsPileNodeProperties: if true store the spring constants as
                                          properties of the pile nodes.
        '''
        # Compute the 3D spring constants and ignore the second horizontal
        # component.
        linearSpringsConstants= self.getLinearSpringsConstants2D(alphaKh_x= alphaKh_x, alphaKv_y= alphaKv_y)
        prep= self.pileSet.getPreprocessor
        modelSpace= predefined_spaces.getModelSpace(prep)
        self.springs= generate_springs_pile_2d(modelSpace= modelSpace, nodes= self.pileSet.nodes, linearSpringsConstants= linearSpringsConstants, storeAsPileNodeProperties= storeAsPileNodeProperties)
        return self.springs

    
    def generateSpringsPile3D(self, alphaKh_x, alphaKh_y, alphaKv_z, storeAsPileNodeProperties= True):
        '''Generate the springs that simulate the soils along the pile in a 3D
           problem.

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKh_y: coefficient to be applied to the horizontal stiffness
                          of a single pile in Y direction
        :param alphaKh_Z: coefficient to be applied to the vertical stiffness of
                          a single pile in Z direction
        :param storeAsPileNodeProperties: if true store the spring constants as
                                          properties of the pile nodes.
        '''
        linearSpringsConstants= self.getLinearSpringsConstants3D(alphaKh_x= alphaKh_x, alphaKh_y= alphaKh_y, alphaKv_z= alphaKv_z)
        prep= self.pileSet.getPreprocessor
        modelSpace= predefined_spaces.getModelSpace(prep)
        self.springs= generate_springs_pile_3d(modelSpace= modelSpace, nodes= self.pileSet.nodes, linearSpringsConstants= linearSpringsConstants, storeAsPileNodeProperties= storeAsPileNodeProperties)
        return self.springs
        
    def generateSpringsPile(self, alphaKh_x, alphaKh_y, alphaKv_z, storeAsPileNodeProperties= True):
        '''Generate the springs that simulate the soils along the pile

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKh_y: coefficient to be applied to the horizontal stiffness
                          of a single pile in Y direction
        :param alphaKh_Z: coefficient to be applied to the vertical stiffness of
                          a single pile in Z direction
        :param storeAsPileNodeProperties: if true store the spring constants as
                                          properties of the pile nodes.
        '''
        preprocessor= self.pileSet.getPreprocessor
        numDOFs= preprocessor.getNodeHandler.numDOFs
        retval= None
        if(numDOFs==6): # 3D structural mechanics
            retval= self.generateSpringsPile3D(alphaKh_x= alphaKh_x, alphaKh_y= alphaKh_y, alphaKv_z= alphaKv_z, storeAsPileNodeProperties= storeAsPileNodeProperties)
        elif(numDOFs==3):
            retval= self.generateSpringsPile2D(alphaKh_x= alphaKh_x, alphaKv_y= alphaKv_z, storeAsPileNodeProperties= storeAsPileNodeProperties)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for '+str(numDOFs) + ' degrees of freedom.')
        return retval
            
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
    
# The following functions are based on the example 14.4.1 of OpenSees
# (see https://openseespydoc.readthedocs.io/en/stable/src/pile.html) by Pavan
# Chigullapally from University of Auckland, Auckland.

##########################################################
#                                                         #
# Procedure to compute ultimate lateral resistance, p_u,  #
#  and displacement at 50% of lateral capacity, y50, for  #
#  p-y springs representing cohesionless soil.            #
#                                                         #
###########################################################

# References:
#
# American Petroleum Institute (API) (1987). Recommended Practice for
# Planning, Designing and Constructing Fixed Offshore Platforms. API
# Recommended Practice 2A(RP-2A), Washington D.C, 17th edition.
#
# Brinch Hansen, J. (1961). "The ultimate resistance of rigid piles against
# transversal forces." Bulletin No. 12, Geoteknisk Institute, Copenhagen, 59.
#
#  Boulanger, R. W., Kutter, B. L., Brandenberg, S. J., Singh, P., and Chang,
#  D. (2003). Pile Foundations in liquefied and laterally spreading ground
#  during earthquakes: Centrifuge experiments and analyses. Center for
#  Geotechnical Modeling, University of California at Davis, Davis, CA.
#  Rep. UCD/CGM-03/01.
#
# Reese, L.C. and Van Impe, W.F. (2001), Single Piles and Pile Groups Under
# Lateral Loading. A.A. Balkema, Rotterdam, Netherlands.

def _get_pult(pyDepth, rho, phi, b, tribLength, puSwitch):
    '''  Compute the ultimate resistance (pult) according to API 
         recommendations (Reese and Van Impe, 2001 or API, 1987) for puSwitch= 1
         OR per the method of Brinch Hansen (1961) for puSwitch= 2 

    :param pyDepth: depth of the poing of interest (m).
    :param rho: soil unit mass (kg/m3).
    :param phi: soil internal friction angle (radians).
    :param b: pile diameter (m).
    :param tribLength: tributary length (m).
    :param puSwitch: if (puSwitch= 1) compute pult according to API 
                     recommendations (Reese and Van Impe, 2001 or API, 1987).
                     if (puSwitch= 2) compute pult according to the method 
                     of Brinch Hansen (1961).
    '''
    # define ultimate lateral resistance, pult 
    zbRatio= pyDepth / b

    
    #-------API recommended method-------    
    if puSwitch == 1:
    
      # obtain loading-type coefficient A for given depth-to-diameter ratio zb
      #  ---> values are obtained from a figure and are therefore approximate
        zb= []
        dataNum= 41
        for i in range(dataNum):
            b1= i * 0.125
            zb.append(b1)
        As= [2.8460, 2.7105, 2.6242, 2.5257, 2.4271, 2.3409, 2.2546, 2.1437, 2.0575, 1.9589, 1.8973, 1.8111, 1.7372, 1.6632, 1.5893, 1.5277, 1.4415, 1.3799, 1.3368, 1.2690, 1.2074, 1.1581, 
            1.1211, 1.0780, 1.0349, 1.0164, 0.9979, 0.9733, 0.9610, 0.9487, 0.9363, 0.9117, 0.8994, 0.8994, 0.8871, 0.8871, 0.8809, 0.8809, 0.8809, 0.8809, 0.8809] 
      
      # linear interpolation to define A for intermediate values of depth:diameter ratio
        for i in range(dataNum):
            if zbRatio >= 5.0:
                A= 0.88
            elif zb[i] <= zbRatio and zbRatio <= zb[i+1]:
                A= (As[i+1] - As[i])/(zb[i+1] - zb[i]) * (zbRatio-zb[i]) + As[i]
                
      # define common terms
        alpha= phi / 2
        beta= math.pi / 4 + phi / 2
        K0= 0.4
        
        tan_1= math.tan(math.pi / 4 - phi / 2)        
        Ka= math.pow(tan_1 , 2) 
    
      # terms for Equation (3.44), Reese and Van Impe (2001)
        tan_2= math.tan(phi)
        tan_3= math.tan(beta - phi)
        sin_1= math.sin(beta)
        cos_1= math.cos(alpha)
        c1= K0 * tan_2 * sin_1 / (tan_3*cos_1)
        
        tan_4= math.tan(beta)
        tan_5= math.tan(alpha)
        c2= (tan_4/tan_3)*tan_4 * tan_5
        
        c3= K0 * tan_4 * (tan_2 * sin_1 - tan_5)
        
        c4= tan_4 / tan_3 - Ka
    
        # terms for Equation (3.45), Reese and Van Impe (2001)
        pow_1= math.pow(tan_4,8)
        pow_2= math.pow(tan_4,4)
        c5= Ka * (pow_1-1)
        c6= K0 * tan_2 * pow_2
    
      # Equation (3.44), Reese and Van Impe (2001)
        gamma_kN= rho*g/1e3
        pst= gamma_kN * pyDepth * (pyDepth * (c1 + c2 + c3) + b * c4)
    
      # Equation (3.45), Reese and Van Impe (2001)
        psd= b * gamma_kN * pyDepth * (c5 + c6)
    
      # pult is the lesser of pst and psd. At surface, an arbitrary value is defined
        if pst <=psd:
            if pyDepth == 0:
                pu= 0.01
              
            else:
                pu= A * pst
              
        else:
            pu= A * psd
          
      # PySimple1 material formulated with pult as a force, not force/length, multiply by trib. length
        pult_N= pu * tribLength*1e3
    return pult_N, A

def _get_py50(pult, pyDepth, rho, phi, A, tribLength, kSwitch, gwtSwitch):
    '''  Compute the displacement at 50% lateral capacity (y50).

    :param pult: ultimate lateral resistance of the soil (N).
    :param pyDepth: depth of the poing of interest (m).
    :param rho: soil unit mass (kg/m3).
    :param phi: soil internal friction angle (radians).
    :param A: loading-type coefficient.
    :param b: pile diameter (m).
    :param kSwitch: a linear variation of k with depth is defined for kSwitch= 1 
                    after API (1987) a parabolic variation of k with depth is 
                    defined for kSwitch= 2 after Boulanger et al. (2003).
    :param gwtSwitch: gwtSwitch= 1, k reflects soil above the groundwater table,
                      gwtSwitch= 2, k reflects soil below the groundwater table,
                      being k the coefficent of subgrade reaction.
    '''
    # compute displacement at 50% lateral capacity, y50
    
    # values of y50 depend of the coefficent of subgrade reaction, k, which
    # can be defined in several ways.
    #  for gwtSwitch= 1, k reflects soil above the groundwater table
    #  for gwtSwitch= 2, k reflects soil below the groundwater table
    #  a linear variation of k with depth is defined for kSwitch= 1 after
    #  API (1987)
    #  a parabolic variation of k with depth is defined for kSwitch= 2 after
    #  Boulanger et al. (2003)
    
    # API (1987) recommended subgrade modulus for given friction angle, values obtained from figure (approximate)
    
    ph= [28.8, 29.5, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0]    
   
    # subgrade modulus above the water table
    if gwtSwitch == 1:
        k= [10, 23, 45, 61, 80, 100, 120, 140, 160, 182, 215, 250, 275] 
    else: # subgrade modulus below the water table
        k= [10, 20, 33, 42, 50, 60, 70, 85, 95, 107, 122, 141, 155]
    
    dataNum= len(k) # 13
    phiDegree= math.degrees(phi)
    for i in range(dataNum):
        if ph[i] <= phiDegree and phiDegree <= ph[i+1]:
            khat= (k[i+1]-k[i])/(ph[i+1]-ph[i])*(phiDegree - ph[i]) + k[i]            
            
    # change units from (lb/in^3) to (kN/m^3)
    k_SIunits= khat * 271.45
    
    # define parabolic distribution of k with depth if desired (i.e. lin_par switch == 2)
    gamma_kN= rho*g/1e3
    sigV= pyDepth * gamma_kN
    
    if sigV == 0:
        sigV= 0.01
         
    if kSwitch == 2:
       # Equation (5-16), Boulanger et al. (2003)
        cSigma= math.pow(50 / sigV , 0.5)
       # Equation (5-15), Boulanger et al. (2003)
        k_SIunits= cSigma * k_SIunits
    
    # define y50 based on pult and subgrade modulus k
    
    # based on API (1987) recommendations, p-y curves are described using tanh functions.
    #  tcl does not have the atanh function, so must define this specifically
    
    #  i.e.  atanh(x)= 1/2*ln((1+x)/(1-x)), |x| < 1
    
    # when half of full resistance has been mobilized, p(y50)/pult= 0.5
    x= 0.5
    log_1= math.log((1+x)/(1-x))
    atanh_value= 0.5 * log_1
    
    # need to be careful at ground surface (don't want to divide by zero)
    if pyDepth == 0.0:
        pyDepth= 0.01

    pult_kN= pult/1e3
    pu= pult_kN/tribLength
    y50= 0.5 * (pu/ A)/(k_SIunits * pyDepth) * atanh_value
    return y50

def get_py_param(pyDepth, rho, phi, b, tribLength, puSwitch, kSwitch, gwtSwitch):
    '''  Compute the ultimate resistance (pult) according to API 
         recommendations (Reese and Van Impe, 2001 or API, 1987) for puSwitch= 1
         OR per the method of Brinch Hansen (1961) for puSwitch= 2 and the
         displacement at 50% lateral capacity (y50).

    :param pyDepth: depth of the poing of interest (m).
    :param rho: soil unit mass (kg/m3).
    :param phi: soil internal friction angle (radians).
    :param b: pile diameter (m).
    :param puSwitch: if (puSwitch= 1) compute pult according to API 
                     recommendations (Reese and Van Impe, 2001 or API, 1987).
                     if (puSwitch= 2) compute pult according to the method 
                     of Brinch Hansen (1961).
    :param kSwitch: a linear variation of k with depth is defined for kSwitch= 1 
                    after API (1987) a parabolic variation of k with depth is 
                    defined for kSwitch= 2 after Boulanger et al. (2003).
    :param gwtSwitch: gwtSwitch= 1, k reflects soil above the groundwater table,
                      gwtSwitch= 2, k reflects soil below the groundwater table,
                      being k the coefficent of subgrade reaction.
    '''
    # define ultimate lateral resistance, pult 
    zbRatio= pyDepth / b

    pult_N, A= _get_pult(pyDepth= pyDepth, rho= rho, phi= phi, b= b, tribLength= tribLength, puSwitch= puSwitch)

    # compute displacement at 50% lateral capacity, y50
    y50= _get_py50(pult= pult_N, pyDepth= pyDepth, rho= rho, phi= phi, A= A, tribLength= tribLength, kSwitch= kSwitch, gwtSwitch= gwtSwitch)
    # return pult and y50 parameters
    return pult_N, y50


###########################################################
#                                                         #
# Procedure to compute ultimate tip resistance, qult, and #
#  displacement at 50% mobilization of qult, z50, for     #
#  use in q-z curves for cohesionless soil.               #
#                                                         #
###########################################################

# references
#  Meyerhof G.G. (1976). "Bearing capacity and settlement of pile foundations." 
#   J. Geotech. Eng. Div., ASCE, 102(3), 195-228.
#
#  Vijayvergiya, V.N. (1977). "Load-movement characteristics of piles."
#   Proc., Ports 77 Conf., ASCE, New York.
#
#  Kulhawy, F.H. ad Mayne, P.W. (1990). Manual on Estimating Soil Properties for 
#   Foundation Design. Electrical Power Research Institute. EPRI EL-6800, 
#   Project 1493-6 Final Report.

def get_qz_param(phi, b, sigV, G):
    '''
    Procedure to compute ultimate tip resistance, qult, and displacement at 50%
    mobilization of qult, z50, for use in q-z curves for cohesionless soil.

    :param phi: soil internal friction angle (radians).
    :param b: pile diameter (m).
    :param sigV: vertical stress (Pa).
    :param G: soil shear modulus at pile tip (Pa).
    '''
    # define required constants; pi, atmospheric pressure (kPa), pa, and coeff. of lat earth pressure, Ko
    pa= 101
    sin_4= math.sin(phi)
    Ko= 1 - sin_4

  # ultimate tip pressure can be computed by qult= Nq*sigV after Meyerhof (1976)
  #  where Nq is a bearing capacity factor, phi is friction angle, and sigV is eff. overburden
  #  stress at the pile tip.

  # rigidity index
    tan_7= math.tan(phi)
    sigV_kPa= sigV/1e3
    G_kPa= G/1e3
    Ir= G_kPa/(sigV_kPa * tan_7)
  # bearing capacity factor
    tan_8= math.tan(math.pi/4+phi/2)
    sin_5= math.sin(phi)
    pow_4= math.pow(tan_8,2)
    pow_5= math.pow(Ir,(4*sin_5)/(3*(1+sin_5)))
    exp_4= math.exp(math.pi/2-phi)
    
    Nq= (1+2*Ko)*(1/(3-sin_5))*exp_4*(pow_4)*(pow_5)  
  # tip resistance
    qu= Nq * sigV_kPa
  # QzSimple1 material formulated with qult as force, not stress, multiply by area of pile tip
    pow_6= math.pow(b, 2)
    qult_N= qu * math.pi*pow_6/4*1e3

  # the q-z curve of Vijayvergiya (1977) has the form, q(z)= qult*(z/zc)^(1/3)
  #  where zc is critical tip deflection given as ranging from 3-9% of the
  #  pile diameter at the tip.  

  # assume zc is 5% of pile diameter
    zc= 0.05 * b

  # based on Vijayvergiya (1977) curve, z50= 0.125*zc
    z50= 0.125 * zc

  # return values of qult and z50 for use in q-z material
    return qult_N, z50

##########################################################
#                                                         #
# Procedure to compute ultimate resistance, tult, and     #
#  displacement at 50% mobilization of tult, z50, for     #
#  use in t-z curves for cohesionless soil.               #
#                                                         #
###########################################################

def get_tz_param(phi, b, sigV, tribLength):
    '''
    Procedure to compute ultimate resistance, tult, and displacement at 50% 
    mobilization of tult, z50, for use in t-z curves for cohesionless soil.

    References:
     Mosher, R.L. (1984). "Load transfer criteria for numerical analysis of
      axial loaded piles in sand." U.S. Army Engineering and Waterways
      Experimental Station, Automatic Data Processing Center, Vicksburg, Miss.

     Kulhawy, F.H. (1991). "Drilled shaft foundations." Foundation engineering
     handbook, 2nd Ed., Chap 14, H.-Y. Fang ed., Van Nostrand Reinhold, New York


      Compute tult based on tult= Ko*sigV*pi*dia*tan(delta), where
        Ko    is coeff. of lateral earth pressure at rest, 
              taken as Ko= 0.4
        delta is interface friction between soil and pile,
              taken as delta= 0.8*phi to be representative of a 
              smooth precast concrete pile after Kulhawy (1991)

    :param phi: soil internal friction angle (radians).
    :param b: pile diameter (m).
    :param sigV: vertical stress (Pa).
    :param tribLength: tributary length (m).
    '''  
    delta= 0.8 * phi

  # if z= 0 (ground surface) need to specify a small non-zero value of sigV
  
    if sigV == 0.0:
        sigV= 10.0 # (N)
    
    tan_9= math.tan(delta)
    tu= 0.4 * sigV * math.pi * b * tan_9
    
  # TzSimple1 material formulated with tult as force, not stress, multiply by tributary length of pile
    tult= tu * tribLength

  # Mosher (1984) provides recommended initial tangents based on friction angle
	# values are in units of psf/in
    kf= [6000, 10000, 10000, 14000, 14000, 18000]
    fric= [math.radians(a) for a in [28, 31, 32, 34, 35, 38]]

    dataNum= len(fric)
    
    # determine kf for input value of phi, linear interpolation for intermediate values
    if phi < fric[0]:
        k= kf[0]
    elif phi > fric[5]:
        k= kf[5]
    else:
        for i in range(dataNum):
            if fric[i] <= phi and phi <= fric[i+1]:
                k= ((kf[i+1] - kf[i])/(fric[i+1] - fric[i])) * (phi - fric[i]) + kf[i]
        

  # need to convert kf to units of kN/m^3
    kSIunits=  k * 1.885e3

  # based on a t-z curve of the shape recommended by Mosher (1984), z50= tult/kf
    z50= tult / kSIunits

  # return values of tult and z50 for use in t-z material
    return tult, z50
