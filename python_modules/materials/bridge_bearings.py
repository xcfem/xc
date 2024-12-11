# -*- coding: utf-8 -*-
'''Classes for modelling bridge bearings.

This module provides some classes that allow modelling elastomeric and pot type
bridge bearings. For now they are modelized as linear joints that connect
two nodes with springs that introduce translational and/or rotational stiffness
that are approximately equal to those of the real bearing.


Examples:
    Examples of the use of these classes are given in the following verification tests:
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

'''

from __future__ import division

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import sys
import math
import scipy.interpolate
from itertools import count
from materials import typical_materials
import xc


class Bearing(object):
    '''Bearings base class.

    :ivar materials: material list (one material for each degree of freedom).
    :ivar materialHandler: XC material handler.
    :ivar id: object identifier (auto).
    '''
    _ids = count(0) #Object counter.
    def __init__(self, bearing_type= None):
        ''' Constructor.

        :param bearing_type: string that identifies the type of the bearing in the problem.
        '''
        self.materials= list()
        self.materialHandler= None
        self.id= next(self._ids) # Object identifier.
        self.bearing_type= bearing_type
        

    def getTypeId(self):
        ''' Return the string that identifies the bearing type.'''
        return self.bearing_type
    
    def getMaterial(self,i):
        ''' Returns the i-th uniaxial material that modelizes the response in the i-th direction.'''
        return self.materialHandler.getMaterial(self.materials[i])
    
    def getMaterialNames(self):
        '''Return material names for each DOF in a list.'''
        return self.materials
    
    # def setBearingBetweenNodes(self,predefinedSpace, iNodA,iNodB, orientation= None):
    #   '''Modelize a bearing between the nodes and return newly created zero 
    #    length element that represents the bearing.

    #    :param predefinedSpace: model space (object).
    #    :param iNodA: (int) first node identifier (tag).
    #    :param iNodB: (int) second node identifier (tag).
    #   '''
    #   return predefinedSpace.setBearingBetweenNodes(iNodA,iNodB,self.materials, orientation)
  
    # def setBearing(self,predefinedSpace, iNodA, orientation= None):
    #   '''Modelize a bearing on X, XY or XYZ directions.

    #      :param predefinedSpace: model space (object).
    #      :param iNod: (int) node identifier (tag).
    #      :param bearingMaterials (list): material names for the zero length element.
    #      :return rtype: (int, int) new node tag, new element tag.
    #   '''
    #   return predefinedSpace.setBearing(iNodA,self.materials, orientation)
    
class ElastomericBearing(Bearing):
    '''Rectangular elastomeric bearing.

    Reference:
        :"Puentes": book from Javier Manterola Armisén pagea 591 and 592.

    Class-wide members:

        v2table: interpolation function for the V2 shape factor.
        v3table: interpolation function for the V3 shape factor.
        v4table: interpolation function for the V4 shape factor.
        betaTable: interpolation function for the beta shape factor.

    Attributes:

    :ivar G: (float) Elastomer shear modulus.
    :ivar a: (float) Width of the bearing (parallel to bridge longitudinal axis).
    :ivar b: (float) Length of the bearing (parallel to the bridge transverse axis)
    :ivar e: (float) Net thickness of the bearing (without steel plates).
    '''
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

    def __init__(self,G: float,a: float,b: float ,e: float, bearing_type= None):
        '''Class constructor.

        :param G: elastomer shear modulus.
        :param a: width of the bearing (parallel to bridge longitudinal axis).
        :param b: length of the bearing (parallel to the bridge transverse axis)
        :param e: net thickness of the bearing (without steel plates).
        :param bearing_type: string that identifies the type of the bearing in the problem.
        '''
        super(ElastomericBearing,self).__init__(bearing_type= bearing_type)
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
    
    def getKrotationTransvBridgeAxis(self):
        ''' Stiffness with respect to the rotation around an axis
            parallel to the transverse bridge direction by the center of the bearing.
        '''
        return self.G*self.a*pow(self.b,5.0)/(self.getV4()*pow(self.e,3.0))
    
    def getKrotationLongBridgeAxis(self):
        ''' Stiffness with respect to the rotation around the longitudinal bridge axis
            by the center of the bearing.
        '''
        return self.G*self.b*pow(self.a,5.0)/(self.getV4()*pow(self.e,3.0))
    
    def getBeta(self):
        ''' Return the value of the beta factor.'''
        if(self.a<self.b):
            retval= self.betaTable(self.b/self.a)
        else:
            retval= self.betaTable(self.a/self.b)
        return retval
    
    def getKrotationVerticalAxis(self):
        ''' Stiffness  with respect to the rotation around a vertical axis.'''
        return self.getBeta()*self.G*self.a*pow(self.b,3.0)/self.e
    
    def defineMaterials(self,preprocessor):
        '''Define the following six materials to modelize the elastomeric bearing:
        KX: translation along the element X axis, that must match the 
               bridge longitudinal axis direction
        KY: translation along the element Y axis, that must match the 
               bridge transverse axis direction
        KZ: translation along vertical direction
        THX: rotation about the element X axis, that must match the 
               bridge longitudinal axis direction
        THY: rotation about the element Y axis, that must match the 
               bridge transverse axis direction
        THZ: rotation about vertical direction

        :param preprocessor: (:obj:'Preprocessor') preprocessor to use.
        '''
        if(len(self.materials)==0): # if not defined yet.
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
            self.matKTHX= typical_materials.defElasticMaterial(preprocessor, self.matTHXName, self.getKrotationLongBridgeAxis())
            self.matKTHY= typical_materials.defElasticMaterial(preprocessor, self.matTHYName, self.getKrotationTransvBridgeAxis()) 
            self.matKTHZ= typical_materials.defElasticMaterial(preprocessor, self.matTHZName, self.getKrotationVerticalAxis())
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; materials already defined, command ignored.')
            

    def putBetweenNodes(self, modelSpace, iNodA:int, iNodB:int, orientation= None):
        ''' Puts the bearing between the nodes. The element must be oriented so that its local x axis is in the direction of the longitudinal axis of the bridge and its local y axis parallel to the transverse axis of the transverse axis of the bridge.


        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        :param iNodA (int): first node identifier (tag).
        :param iNodB (int): second node identifier (tag).
        :param orientation: (list) of two vectors [x,yp] used to orient 
                    the zero length element, where: 
                    x: are the vector components in global coordinates defining 
                        local x-axis, that must be parallel to the bridge longitudinal axis (optional)
                    yp: vector components in global coordinates defining a  vector
                          that lies in the local x-y plane of the element (optional).
                          If the optional orientation vector are not specified, the local
                          element axes coincide with the global axes
        '''
        if(len(self.materials)==0): # No materials defined yet.
            self.defineMaterials(modelSpace.preprocessor)
        newElement= modelSpace.setBearingBetweenNodes(iNodA,iNodB, self.materials, orientation)
        newElement.setProp('bearing_type', self.getTypeId())
        return newElement

    def putAsSupport(self,modelSpace, iNod:int , orientation= None):
        ''' Puts the bearing between the nodes.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        :param iNod (int): node to support.
         :param orientation: (list) of two vectors [x,yp] used to orient 
            the zero length element, where: 
            x: are the vector components in global coordinates defining 
               local x-axis (optional)
            yp: vector components in global coordinates defining a  vector
                 that lies in the local x-y plane of the element (optional).
          If the optional orientation vector are not specified, the local
          element axes coincide with the global axes. Otherwise, the local
          z-axis is defined by the cross product between the vectors x 
          and yp specified in the command line.
        '''
        nodeHandler= modelSpace.getNodeHandler()
        newNode= nodeHandler.duplicateNode(iNod) # new node.
        
        if(len(self.materials)==0): # No materials defined yet.
            self.defineMaterials(modelSpace.preprocessor)
        newElement= modelSpace.setBearingBetweenNodes(newNode.tag,iNod,self.materials, orientation)
        newElement.setProp('bearing_type', self.getTypeId()) 
        # Boundary conditions
        numDOFs= nodeHandler.numDOFs
        for i in range(0,numDOFs):
            spc= modelSpace.newSPConstraint(newNode.tag,i,0.0)
            if(__debug__):
                if(not spc):
                    AssertionError('Can\'t create the constraint.')
        return newNode, newElement

# Points that define the Teflon coefficient of friction of
# from the mean compressive stress
# See the book «Aparatos de apoyo en puentes» from AIPCR page 46

# Table 59.8.2 of clase 59.8.2 of EAE (page 256)
xT= [0,5e6,10e6,20e6,30e6,45e6,1000e6]
yT= [0.08,0.08,0.06,0.04,0.03,0.025,0.024]


class PTFEPotBearingMat(Bearing):
    '''PTFE slide bearing material.

     Attibutes:
    :ivar d: (float) Pot diameter.
    :ivar unidirX: unidirectional POT in local-X direction (uY constrained) (defaults to False)
    :ivar unidirY: unidirectional POT in local-Y direction (uX constrained) (defaults to False)
    :ivar factStiff: factor to increase stiffness in constrained locad directions  (defaults to 1e5)
    '''
    teflonMuTable= scipy.interpolate.interp1d(xT,yT)

    def __init__(self,d,unidirX=False,unidirY=False,factStiff=1e5,deltaFrict=20e-3,Fperp=None, bearing_type= None, rotStiff=1e-5):
        '''Class constructor.

        :param d: pot diameter.
        :param unidirX: unidirectional POT in local-X direction (uY constrained) (defaults to False)
        :param unidirY: unidirectional POT in local-Y direction (uX constrained) (defaults to False)
        :param factStiff: factor to increase stiffness in constrained locad directions (defaults to 1e5)
        :param deltaFrict: Displacement when the friction force is reached (defaults to 20 mm).
        :parma Fperp: mean compressive force perperdicular to the pot surface. If Fperp is not given, the 
                      compressive stress is taken as 35Mpa
        :param bearing_type: string that identifies the type of the bearing in the problem.
        :param rotStiff: rotational stiffness 
        '''
        super(PTFEPotBearingMat,self).__init__(bearing_type= bearing_type)
        self.d= d
        self.unidirX=unidirX
        self.unidirY=unidirY
        self.factStiff=factStiff
        self.deltaFrict=deltaFrict
        self.Fperp=Fperp
        self.rotStiff=rotStiff

    def getMeanStress(self):
        Apot=math.pi*(self.d/2.0)**2
        if self.Fperp is None: self.Fperp=35e6*Apot
        sigma=self.Fperp/Apot
        return sigma

    def getMu(self):
        sigma=self.getMeanStress()
        mu=1.0*self.teflonMuTable(sigma)
        return mu
        
    def getHorizontalStiffness(self):
        '''Returns the fictitious stiffness with respect to the horizontal displacement of a PTFE slide bearing.
        '''
        mu=self.getMu()
        K=mu*self.Fperp/self.deltaFrict
        return K

    def defineMaterials(self, preprocessor):
        '''Define the materials to modelize the pot (Teflon).

            :param preprocessor: (:obj:'Preprocessor') preprocessor to use.
        '''
        if(len(self.materials)==0):
            self.materialHandler= preprocessor.getMaterialHandler
            # Material names.
            nameRoot= 'pot'+str(self.id)
            self.matXName= nameRoot+'X'
            self.matYName= nameRoot+'Y'
            self.matZName= nameRoot+'Z'
            self.matTHXName= nameRoot+'THX'
            self.matTHYName= nameRoot+'THY'
            self.materials.extend([self.matXName, self.matYName,self.matZName])
            self.materials.extend([self.matTHXName, self.matTHYName])
            # Material objects.
            horizStiff=self.getHorizontalStiffness()
            if self.unidirY:
                self.matKX= typical_materials.defElasticMaterial(preprocessor, name=self.matXName, E=self.factStiff*horizStiff)
            else:
                self.matKX= typical_materials.defElasticMaterial(preprocessor, name=self.matXName,E=horizStiff)
            if self.unidirX:
                self.matKY= typical_materials.defElasticMaterial(preprocessor, name=self.matYName, E=self.factStiff*horizStiff)
            else:
                 self.matKY= typical_materials.defElasticMaterial(preprocessor, name=self.matYName, E=horizStiff)
            self.matKZ= typical_materials.defElasticMaterial(preprocessor, name=self.matZName, E=self.factStiff*horizStiff)
            self.matKTHX= typical_materials.defElasticMaterial(preprocessor, self.matTHXName, E=self.rotStiff)
            self.matKTHY= typical_materials.defElasticMaterial(preprocessor, self.matTHYName, E=self.rotStiff)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; materials already defined, command ignored.')


class PotBearing(object):
    '''POT structural bearing

        :ivar potMat: : (:obj:'PTFEPotBearingMat') POT material.
        :ivar nodA: first node.
        :ivar nodB: second node .
        :ivar orientation: (list) of two vectors [x,yp] used to orient 
                    the zero length element, where: 
                    x: are the vector components in global coordinates defining 
                        local x-axis
                    yp: vector components in global coordinates defining a  vector
                          that lies in the local x-y plane of the element.
                    If the optional orientation vector are not specified, the local
                    element axes coincide with the global axes
    '''

    def __init__(self, potMat, nodA, nodB, orientation= None):
        '''Class constructor. Creates the pot element between nodA and nodB

        :param potMat: : (:obj:'PTFEPotBearingMat') POT material.
        :param nodA: first node.
        :param nodB: second node .
        :param orientation: (list) of two vectors [x,yp] used to orient 
                    the zero length element, where: 
                    x: are the vector components in global coordinates defining 
                        local x-axis
                    yp: vector components in global coordinates defining a  vector
                          that lies in the local x-y plane of the element. (local y-axis)
                    If the optional orientation vector are not specified, the local
                    element axes coincide with the global axes
        :param bearing_type: string that identifies the type of the bearing in the problem.
         '''
        self.potMat= potMat
        self.nodA=nodA
        self.nodB=nodB
        self.orientation=orientation
        # generate the pot element
        self.createPotElement()

    def createPotElement(self):
        ''' Create the zero length element that represents the pot bearing.'''
        prep= self.nodA.getDomain.getPreprocessor
        elems= prep.getElementHandler
        elems.dimElem= prep.getNodeHandler.dimSpace
        if(len(self.potMat.materials)==0): # If the materials are not defined yet.
            self.potMat.defineMaterials(prep) # define them.
        defaultMaterial= next((item for item in self.potMat.materials if item is not None), None)
        if(defaultMaterial):
            elems.defaultMaterial= defaultMaterial
            self.potElem= elems.newElement("ZeroLength",xc.ID([self.nodA.tag,self.nodB.tag]))
            self.potElem.setProp('bearing_type', self.potMat.getTypeId())
            self.potElem.clearMaterials()
            if(self.orientation): #Orient element.
                self.potElem.setupVectors(self.orientation[0],self.orientation[1])
            for i, material in enumerate(self.potMat.materials):
                material= self.potMat.materials[i]
                if(material!=None):
                    self.potElem.setMaterial(i,material)
        else: # All are None.
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; all the given materials are None.')
            self.potElem= None
        return self.potElem
       
    def getReaction(self):
        ''' Return the reaction in the fixed node.'''
        return self.potElem.nodes[0].getReaction
    
    def getMatXlocal(self):
        m=self.potElem.getMaterials()[0]
        return m
 
    def getMatYlocal(self):
        m=self.potElem.getMaterials()[1]
        return m
    
    def getMatZlocal(self):
        m=self.potElem.getMaterials()[2]
        return m
    
    def getMatTHXlocal(self):
        m=self.potElem.getMaterials()[3]
        return m
    
    def getMatTHYlocal(self):
        m=self.potElem.getMaterials()[4]
        return m

    def getFXlocal(self):
        ''' Returns the force in local-X direction 
        '''
        m=self.getMatXlocal()
        Fx=m.getStress()
        return Fx
    
    def getFYlocal(self):
        ''' Returns the force in local-Y direction 
        '''
        m=self.getMatYlocal()
        Fy=m.getStress()
        return Fy
    
    def getFZlocal(self):
        ''' Returns the force in local-Z direction 
        '''
        m=self.getMatZlocal()
        Fz=m.getStress()
        return Fz

    def getElongXlocal(self):
        ''' Returns the elongation of the bearing in local-X direction 
        '''
        m=self.getMatXlocal()
        Elx=m.getStrain()
        return Elx
        
    def getElongYlocal(self):
        ''' Returns the elongation of the bearing in local-Y direction 
        '''
        m=self.getMatYlocal()
        Ely=m.getStrain()
        return Ely
        
    def getElongZlocal(self):
        ''' Returns the elongation of the bearing in local-Z direction 
        '''
        m=self.getMatZlocal()
        Elz=m.getStrain()
        return Elz
    
    def getRotXlocal(self):
        ''' Return the rotation around local -X direction
        '''
        m=self.getMatTHXlocal()
        rotX=m.getStrain()
        return rotX
    
    def getRotYlocal(self):
        ''' Return the rotation around local -Y direction
        '''
        m=self.getMatTHYlocal()
        rotY=m.getStrain()
        return rotY
    
    def getVdispNodA(self):
        ''' Return the nodA displacement vector'''
        vDispA= self.nodA.getDisp
        return vDispA

    def getVdispNodB(self):
        ''' Return the nodB displacement vector'''
        vDispB= self.nodB.getDisp
        return vDispB
    
    def getUxUyUzNodA(self):
        ''' Return the displacement components of nodA displacement vector'''
        vDispA= self.nodA.getDisp
        return xc.Vector([vDispA[0], vDispA[1], vDispA[2]])
    
    def getUxUyUzNodB(self):
        ''' Return the displacement components of nodB displacement vector'''
        vDispB= self.nodB.getDisp
        return xc.Vector([vDispB[0], vDispB[1], vDispB[2]])

def get_reaction_on_pot(preprocessor,iElem,inclInertia= False):
    ''' Return the element reaction.

    :param preprocessor: (:obj:'Preprocessor') preprocessor to use.
    :param iElem: (int) new zero length elem identifier (tag).
    :param inclInertia: (bool) true if the reaction must include inertia forces.
    '''
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+"; don't use this function, is very inefficient: TO DEPRECATE.")
    nodes= preprocessor.getNodeHandler
    nodes.calculateNodalReactions(inclInertia, 1e-7) # It has NO sense to compute the reactions
                                                     # each time you call this function.
  
    elem= preprocessor.getElementHandler.getElement(iElem)
    reac0= elem.getNodes[0].getReaction
    return xc.Vector([reac0[0],reac0[1],reac0[2]])
