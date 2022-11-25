# -*- coding: utf-8 -*-
'''Foundation modelling according to the document: "Guía de cimentaciones 
   para obras de carretera», by «Ministerio de Fomento" (Spain) 2009.'''

__author__= " Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT) "
__copyright__= "Copyright 2022, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es "

import sys
from geotechnics.foundations import pile
from misc_utils import log_messages as lmsg

class SoilLayers(pile.SoilLayers):
    ''' Class to compute the value of the soil reaction modulus according 
        to the table A-6.7 of the "Guía..." document.

    :ivar soilProfile: properties of the levels of soil, defined from 
                     top to bottom as a list: 
                     - [[zBottomSoil,typeSoil,propSoil], ...] where,
                     'zBottomSoil' is the global Z coordinate of the 
                     bottom level of the soil,
                     -'typeSoil' is the type od soil: 'sandy' or 'clay'
                     -'propSoil' is the property of the soil:
                            -'nh' for sandy soil, corresponding to the 
                                  compactness of the sandy soil.
                            -'su' for clay soil, corresponding to the 
                                  shear strength of the saturated
                                  cohesive soil.
    '''
    def __init__(self, groundLevel, soilProfile):
        ''' Constructor.

        :param soilProfile: properties of the levels of soil, defined from 
                     top to bottom as a list: 
                     - [[zBottomSoil,typeSoil,propSoil], ...] where,
                     'zBottomSoil' is the global Z coordinate of the 
                     bottom level of the soil,
                     -'typeSoil' is the type od soil: 'sandy' or 'clay'
                     -'propSoil' is the property of the soil:
                            -'nh' for sandy soil, corresponding to the 
                                  compactness of the sandy soil.
                            -'su' for clay soil, corresponding to the 
                                  shear strength of the saturated
                                  cohesive soil.
        :param waterTableZ: global Z coordinate of the water table.
        :param groundLevel: global Z coordinate of the ground level.
        '''
        super().__init__(groundLevel= groundLevel, waterTableZ= None)
        self.soilProfile= soilProfile
    

class PileFoundation(pile.CircularPile):
    '''Pile foundation model according to art. 5.13.1 (single pile) and art. 
    5.13.3. (pile group) of "Guía de cimentaciones para obras de carretera»,  
    by «Ministerio de Fomento" 2009.
    
    :ivar pileDiam: diameter of the pile
    :ivar pileBearingCapacity: total bearing capacity (skin friction + 
                               point bearing capacity ) of the pile.
    :ivar soilLayers:  properties of the levels of soil.
    '''
    def __init__(self, pileSet, pileDiam,E,pileType,pileBearingCapacity,soilLayers):
        ''' Constructor.

        :param pileSet: set of nodes and elements defining a single pile.
        :param pileDiam: diameter of the pile
        :param E: elastic modulus of pile material
        :param pileBearingCapacity: total bearing capacity (skin friction + 
                                   point bearing capacity ) of the pile.
        :param pileType: "endBearing" for end bearing piles 
                        "friction" for friction piles
        :param soilLayers:  properties of the levels of soil.
        '''
        super(PileFoundation, self).__init__(E= E, pileType= pileType, diameter= pileDiam, soilLayers= soilLayers, pileSet= pileSet)
        self.pileBearingCapacity=pileBearingCapacity
                    
    def getCalcLengthEndBearingPile(self):
        '''Return the calculation length for a single end bearing pile. 
        '''
        return self.getTotalLength()

    def getCalcLengthFrictionPile(self):
        '''Return the calculation length for a single friction pile .'''
        aerL= self.getPileAerialLength()
        burL= self.getPileBuriedLength()
        return aerL+2/3.*burL
    
    def getCalcLength(self):
        '''Return the calculation length for a single pile.'''
        if str.lower(self.pileType[:2]) in 'friction':
            return self.getCalcLengthFrictionPile()
        else:
            return self.getCalcLengthEndBearingPile()

    def getVerticalStiffnessSinglePile(self):
        '''Return the vertical stiffness of a single pile
        '''
        Lc=self.getCalcLength()
        A= self.getCrossSectionArea()
        Kv=1/(self.getDiameter()/40/self.pileBearingCapacity+Lc/A/self.E)
        return Kv

    def getLinearSpringsConstants(self, alphaKh_x= 1.0, alphaKh_y= 1.0, alphaKv_z= 1.0):
        '''Compute the spring contants that simulate the soils along the pile

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKh_y: coefficient to be applied to the horizontal stiffness
                          of a single pile in Y direction
        :param alphaKh_Z: coefficient to be applied to the vertical stiffness of
                          a single pile in Z direction
        '''
        self.computeTributaryLengths(initialGeometry= False)
        lstNodPile= self.getNodeZs()
        soilsProp= self.soilLayers.soilProfile
        if soilsProp[-1][0] >= lstNodPile[-1][1]:
            soilsProp[-1][0]=lstNodPile[-1][1]-1
        zval=lstNodPile[0][1]
        while zval > self.soilLayers.groundLevel:  #aerial zone of pile
            lstNodPile.pop(0)
            zval=lstNodPile[0][1]
        #Springs horizontal stiffness
        z=lstNodPile[0][1]
        if self.getDiameter() <= 1:
            coefKh=1
        else:
            coefKh=self.getDiameter()
        retval= dict()
        for sp in soilsProp:
            zBottom= sp[0]
            soilType= sp[1][:2].lower()
            soilPrp= sp[2]
            if soilType not in ('sa','cl'):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; wrong type of soil: '+str(soilType))
            while(z>zBottom):
                n= lstNodPile[0][0]
                lnTribNod= n.getTributaryLength()
                if(soilType == 'sa'): #sandy soil
                    Kh_x= alphaKh_x*soilPrp*(self.soilLayers.groundLevel-z)*lnTribNod*coefKh
                    Kh_y= alphaKh_y*soilPrp*(self.soilLayers.groundLevel-z)*lnTribNod*coefKh
                else: # cohesive soil
                    Kh_x= 75*alphaKh_x*soilPrp*lnTribNod*coefKh
                    Kh_y= 75*alphaKh_y*soilPrp*lnTribNod*coefKh
                if len(lstNodPile)==1:
                    #Spring vertical stiffness (end of pile)
                    Kv_end=alphaKv_z*self.getVerticalStiffnessSinglePile()
                    retval[n.tag]= [Kh_x, Kh_y, Kv_end]
                    break
                else:
                    retval[n.tag]= [Kh_x, Kh_y, 1e-5]
                    z=lstNodPile[1][1]
                    lstNodPile.pop(0)
        return retval
