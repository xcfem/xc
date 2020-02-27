# -*- coding: utf-8 -*-
"""Classes for modelling bridge bearings.

This module provides some classes that allow modelling elastomeric and pot type
bridge bearings. For now they are modelized as linear joints that connect
two nodes with springs that introduce translational and/or rotational stiffness
that are approximately equal to those of the real bearing.


Examples\:
    Examples of the use of these classes are given in the following verification tests\:
        * ./verif/tests/elements/test_elastomeric_bearing_01.py
        * ./verif/tests/elements/test_pot_bearing_01.py
        * ./verif/tests/elements/test_pot_bearing_02.py
        * ./verif/tests/elements/test_pot_bearing_03.py
        * ./verif/tests/materials/test_elastomeric_bearing_stiffness.py

Args:
    * x2 (list): Abcissae of the points that define the values of V2 as a function of b/a.
    * y2 (list): Ordinates of the points that define the values of V2 as a function of b/a.
    * x3 (list): Abcissae of the points that define the values of V3 as a function of b/a.
    * y3 (list): Ordinates of the points that define the values of V3 as a function of b/a.
    * x4 (list): Abcissae of the points that define the values of V4 as a function of b/a.
    * y4 (list): Ordinates of the points that define the values of V4 as a function of b/a.
    * xBeta (list): Abcissae of the points that define the values of beta (alpha in the book) as a function of b/a.
    * yBeta (list): Ordinates of the points that define the values of beta (alpha in the book) as a function of h/b.

Todo:
    * Extend the module to cover other bearing types.

"""

from __future__ import division

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import scipy.interpolate
from itertools import count
from materials import typical_materials
from model import predefined_spaces
import math
import xc


class Bearing(object):
    """Bearings base class.

    """
    _ids = count(0) #Object counter.
    def __init__(self):
      self.materials= list()
      self.materialHandler= None
      self.id= self._ids.next() #Object identifier.
    def getMaterial(self,i):
        ''' Returns the i-th uniaxial material that modelizes the response in the i-th direction.'''
        return self.materialHandler.getMaterial(self.materials[i])
    def getMaterialNames(self):
        '''Return material names for each DOF in a list.'''
        return self.materials
    def setBearingBetweenNodes(self,predefinedSpace, iNodA,iNodB, orientation= None):
      '''Modelize a bearing between the nodes and return newly created zero 
       length element that represents the bearing.

       :param predefinedSpace: model space (object).
       :param iNodA: (int) first node identifier (tag).
       :param iNodB: (int) second node identifier (tag).
      '''
      return predefinedSpace.setBearingBetweenNodes(iNodA,iNodB,self.materials, orientation)
    def setBearing(self,predefinedSpace, iNodA, orientation= None):
      '''Modelize a bearing on X, XY or XYZ directions.

        Args:
             predefinedSpace: model space (object).
             iNod: (int) node identifier (tag).
             bearingMaterials (list): material names for the zero length element.
        Returns:
            :rtype: (int, int) new node tag, new element tag.
      '''
      return predefinedSpace.setBearing(iNodA,self.materials, orientation)
    
class ElastomericBearing(Bearing):
    """Rectangular elastomeric bearing.

    Reference:
        :"Puentes": book from Javier Manterola Armisén page 591 and 592.

    Class-wide members:

        v2table: interpolation function for the V2 shape factor.
        v3table: interpolation function for the V3 shape factor.
        v4table: interpolation function for the V4 shape factor.
        betaTable: interpolation function for the beta shape factor.

    Attributes:

        G: (float) Elastomer shear modulus.
        a: (float) Width of the bearing (parallel to bridge axis).
        b: (float) Length of the bearing (parallel to lintel axis).
        e: (float) Net thickness of the bearing (without steel plates).

    """
    # ENHANCE (FOR ALL THE INTERPOLATIONS): it will be great
    # if we can store the interpolation in a file to avoid repeating
    # computations every time the module is loaded.
    
    # Points that define the values of V2 as a function of b/a
    #   see "Puentes" book from Javier Manterola Armisén page 591
    x2= [1,1.5,2,3,4,6,8,10,10000]
    y2= [0.208,0.231,0.246,0.267,0.282,0.299,0.307,0.313,1/3.0]
    v2table= scipy.interpolate.interp1d(x2,y2)
    
    # Points that define the values of V3 as a function of b/a
    #   see "Puentes" book from Javier Manterola Armisén page 591
    x3= [1,1.5,2,3,4,6,8,10,10000]
    y3= [0.14,0.196,0.229,0.263,0.281,0.299,0.307,0.313,1/3.0]
    v3table=  scipy.interpolate.interp1d(x3,y3)

    # Points that define the values of V4 as a function of b/a
    #   see "Puentes" book from Javier Manterola Armisén page 592
    x4= [1,2,4,8,10000]
    y4= [85.7,71.4,64.5,61.2,60]
    v4table= scipy.interpolate.interp1d(x4,y4)

    # Points that define the values of beta as a function of h/b
    #   see ALPHA in "Hormigón" book from Jiménez Montoya et al.
    #   14th. edition page 405
    xBeta= [1.0,1.25,1.5,2.0,3.0,4.0,6.0,10.0,10000]
    yBeta= [0.14,0.171,0.196,0.229,0.263,0.281,0.299,0.313,1/3.0]
    betaTable= scipy.interpolate.interp1d(xBeta,yBeta)

    def __init__(self,G,a,b,e):
        """Class constructor.

        """
        super(ElastomericBearing,self).__init__()
        self.G= G
        self.a= a
        self.b= b
        self.e= e

    def getKhoriz(self):
        '''Return horizontal stiffness of the bearing.'''
        return self.G*self.a*self.b/self.e
    def getV2(self):
        '''Return the V2 shape factor of the bearing. '''
        return self.v2table(self.b/self.a)
    def getV3(self):
        '''Return the V3 shape factor of the bearing. '''
        return self.v3table(self.b/self.a)
    def getEbearing(self):
        ''' Return the elastic modulus with respect to the vertical
        displacement.
        '''
#        return 3*self.G*(self.a/self.e)**2*self.getV3() #29/04/2018
#       this formula was extracted from Manterola book but is wrong
#       We adopt a more rational value of E_vertical=600e6 Pa until we
#       find a better approximation
        return 600e6
    
    def getKvert(self):
        ''' Return the vertical stiffness.'''
        return self.getEbearing()*self.a*self.b/self.e
    def getV4(self):
        ''' Return the v4 shape factor of the bearing.'''
        return self.v4table(self.b/self.a)
    def getKrotationLintelAxis(self):
        ''' Stiffness with respect to the rotation around an axis
            parallel to the lintel by the center of the bearing.
        '''
        return self.G*self.a*pow(self.b,5.0)/(self.getV4()*pow(self.e,3.0))
    def getKrotationBridgeAxis(self):
        ''' Stiffness with respect to the rotation around an axis
            parallel to the bridge by the center of the bearing.
        '''
        return self.G*self.b*pow(self.a,5.0)/(self.getV4()*pow(self.e,3.0))
    def getBeta(self):
        ''' Return the value of the beta factor.'''
        retval= 0.0
        if(self.a<self.b):
            retval= self.betaTable(self.b/self.a)
        else:
            retval= self.betaTable(self.a/self.b)
        return retval
    def getKrotationVerticalAxis(self):
        ''' Stiffness  with respect to the rotation around a vertical axis.'''
        return self.getBeta()*self.G*self.a*pow(self.b,3.0)/self.e
    def defineMaterials(self,preprocessor):
        '''Define the materials to modelize the elastomeric bearing.

        Args:
            :preprocessor: (:obj:'Preprocessor') preprocessor to use.
        '''
        self.materialHandler= preprocessor.getMaterialHandler
        # Material names.
        nameRoot= 'neop'+str(self.id)
        self.matXName= nameRoot+'X'
        self.matYName= nameRoot+'Y'
        self.matZName= nameRoot+'Z'
        self.matTHXName= nameRoot+'THX'
        self.matTHYName= nameRoot+'THY'
        self.matTHZName= nameRoot+'THZ'
        self.materials.extend([self.matXName, self.matYName, self.matZName])
        self.materials.extend([self.matTHXName, self.matTHYName, self.matTHZName])
        # Material objects.
        self.matKX= typical_materials.defElasticMaterial(preprocessor, self.matXName, self.getKhoriz())
        self.matKY= typical_materials.defElasticMaterial(preprocessor, self.matYName, self.getKhoriz())
        self.matKZ= typical_materials.defElasticMaterial(preprocessor, self.matZName, self.getKvert())
        self.matKTHX= typical_materials.defElasticMaterial(preprocessor, self.matTHXName, self.getKrotationLintelAxis())
        self.matKTHY= typical_materials.defElasticMaterial(preprocessor, self.matTHYName, self.getKrotationBridgeAxis())
        self.matKTHZ= typical_materials.defElasticMaterial(preprocessor, self.matTHZName, self.getKrotationVerticalAxis())

    def putBetweenNodes(self,modelSpace,iNodA, iNodB, orientation= None):
        ''' Puts the bearing between the nodes.

        Args:
            modelSpace (:obj:'PredefinedSpace'): space dimension and number of DOFs.
            iNodA (int): first node identifier (tag).
            iNodB (int): second node identifier (tag).

        '''
        return modelSpace.setBearingBetweenNodes(iNodA,iNodB,self.materials, orientation)

# Points that define the Teflon coefficient of friction of
# from the mean compressive stress
# See the book «Aparatos de apoyo en puentes» from AIPCR page 46

# Table 59.8.2 of clase 59.8.2 of EAE (page 256)
xT= [5e6,10e6,20e6,30e6,45e6,1000e6]
yT= [0.08,0.06,0.04,0.03,0.025,0.024]


class PTFEPotBearing(Bearing):
    """PTFE slide bearing.

        Attibutes:
            :d: (float) Pot diameter.

    """
    teflonMuTable= scipy.interpolate.interp1d(xT,yT)

    def __init__(self,d):
        """Class constructor.


        """
        super(PTFEPotBearing,self).__init__()
        self.d= d
    def getHorizontalStiffness(self):
        '''Returns the fictitious stiffness with respect to the horizontal displacement of a PTFE slide bearing.

        Stiffness is calculated so that when the displacement reach 20 mm
        the spring reaction equals the force of friction.

        sg_media= 35 MPa mean compressive stress.
        mov= 20e-3 Displacement when the friction force is reached.

        '''
        return self.teflonMuTable(35e6)*math.pi*(self.d/2.0)**2*35e6/20e-3

    def defineMaterials(self,preprocessor):
        '''Define the materials to modelize the pot (Teflon).

        Args:
            :preprocessor: (:obj:'Preprocessor') preprocessor to use.
            :matKX: (str) name for the uniaxial material in direction X.
            :matKY: (str) name for the uniaxial material in direction Y.
        '''
        self.materialHandler= preprocessor.getMaterialHandler
        # Material names.
        nameRoot= 'pot'+str(self.id)
        self.matXName= nameRoot+'X'
        self.matYName= nameRoot+'Y'
        self.materials.extend([self.matXName, self.matYName])
        # Material objects.
        self.matKX= typical_materials.defElasticMaterial(preprocessor, self.matXName, self.getHorizontalStiffness())
        self.matKY= typical_materials.defElasticMaterial(preprocessor, self.matYName, self.getHorizontalStiffness())

    def putBetweenNodes(self,modelSpace,iNodA, iNodB):
        ''' Puts the bearing between the nodes.

        Args:
            :modelSpace: (:obj:'PredefinedSpace') space dimension and number
                of DOFs.
            :iNodA: (int) first node identifier (tag).
            :iNodB: (int) second node identifier (tag).
        '''
        newElement= modelSpace.setBearingBetweenNodes(iNodA,iNodB,self.materials)
        eDofs= modelSpace.constraints.newEqualDOF(iNodA,iNodB,xc.ID([2]))
        return newElement

    def putOnXBetweenNodes(self,modelSpace,iNodA, iNodB):
        ''' Puts the bearing between the nodes only in direction X.

        Args:
            :modelSpace: (:obj:'PredefinedSpace') space dimension and number
                of DOFs.
            :iNodA: (int) first node identifier (tag).
            :iNodB: (int) second node identifier (tag).
        '''
        newElement= modelSpace.setBearingBetweenNodes(iNodA,iNodB,[self.matXName])
        eDofs= modelSpace.constraints.newEqualDOF(iNodA,iNodB,xc.ID([1,2]))
        return newElement

    def putOnYBetweenNodes(self,modelSpace,iNodA, iNodB):
        ''' Puts the bearing between the nodes only in direction Y.

        Args:
            :modelSpace: (:obj:'PredefinedSpace') space dimension and number
                of DOFs.
            :iNodA: (int) first node identifier (tag).
            :iNodB: (int) second node identifier (tag).
        '''
        newElement= modelSpace.setBearingBetweenNodes(iNodA,iNodB,[None,self.matYName])
        eDofs= modelSpace.constraints.newEqualDOF(iNodA,iNodB,xc.ID([0,2]))
        return newElement

def get_reaction_on_pot(preprocessor,iElem,inclInertia= False):
    ''' Return the element reaction.

        Args:
            :preprocessor: (:obj:'Preprocessor') preprocessor to use.
            :iElem: (int) new zero length elem identifier (tag).
            :inclInertia: (bool) true if the reaction must include inertia forces.
    '''
    nodes= preprocessor.getNodeHandler
    nodes.calculateNodalReactions(inclInertia, 1e-7)
  
    elem= preprocessor.getElementHandler.getElement(iElem)
    reac0= elem.getNodes[0].getReaction
    return xc.Vector([reac0[0],reac0[1],reac0[2]])
