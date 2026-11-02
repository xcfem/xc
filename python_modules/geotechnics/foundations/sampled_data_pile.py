# -*- coding: utf-8 -*-
'''Definition of pile spring supports based on data sampled along the pile
   depth.

   Reference document: "Guía de cimentaciones para obras de carretera», by 
   «Ministerio de Fomento" (Spain) 2009.
'''

from geotechnics.foundations import pile
from misc_utils import log_messages as lmsg
from materials.sections import section_properties as sp

class Pile(pile.Pile):
    ''' Pile on spring supports based on data sampled along the pile depth.

    '''
    def __init__(self, E, crossSection, soilData, pileSet= None, rho= 2.5e3):
        ''' Constructor.

        :param E: elastic modulus of pile material
        :param crossSection: cross-section of the pile.
        :param soilData:  properties of the soil along depth.
        :param pileSet: set of nodes and elements defining a single pile.
        :param rho: material density.
        '''
        super().__init__(E= E, crossSection= crossSection, pileType= None, soilLayers= soilData, pileSet= pileSet, rho= rho)
        self.zMin= None
        self.zMax= None

    def _compute_z_min_and_max(self):
        ''' Compute the maximum and minimum z coordinates of the nodes along the
            pile.
        '''
        if(self.zMin is None):
            lstNodPile= self.getNodeZs() # node identifiers and depths.
            self.zMin= lstNodPile[-1][1]
            self.zMax= lstNodPile[0][1]
        
    def getDepth(self, z:float):
        ''' Return the depth corresponding to the given z coordinate.

        :param z: value of the z coordinate.
        '''
        if(self.zMin is None):
            self._compute_z_min_and_max()
        retval= self.zMax-z 
        return retval
            
    def getBearingCapacity(self, characteristic):
        ''' Return the value of the total bearing capacity of the pile.

        :param characteristic: if true return the characteristic value, 
                               otherwise return the design one.
        '''
        if(self.zMin is None):
            self._compute_z_min_and_max()
        depth= self.getDepth(self.zMin)
        retval= self.soilLayers.getPileBearingCapacityAtDepth(depth, characteristic= characteristic)
        return retval

    def getVerticalStiffnessSinglePile(self):
        '''Return the vertical stiffness of a single pile according to clause
           5.13.1.1 of the reference document (see the docstring of this file).
           The stiffness of the pile itself is already computed by the finite
           element model, so it is ignorede here.
        '''
        bearingCharacteristicCapacity= self.getBearingCapacity(characteristic= True)
        Kv=1.0/(self.getDiameter()/(40*bearingCharacteristicCapacity))
        return Kv

    def getLinearSpringsConstants3D(self, alphaKh_x= 1.0, alphaKh_y= 1.0, alphaKv_z= 1.0):
        '''Compute the spring contants that simulate the soils along the pile

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKh_y: coefficient to be applied to the horizontal stiffness
                          of a single pile in Y direction
        :param alphaKh_Z: coefficient to be applied to the vertical stiffness of
                          a single pile in Z direction
        '''
        # Pile surface factor.
        if self.getDiameter() <= 1:
            coefKh= 1
        else:
            coefKh= self.getDiameter()
        self.computeTributaryLengths(initialGeometry= True) # Tributary lengths.
        retval= dict()
        lstNodPile= self.getNodeZs() # node identifiers and depths.
        zMin= lstNodPile[-1][1]
        zMax= lstNodPile[0][1]
        # Springs horizontal stiffness
        for n, zNode in lstNodPile:
            depth= self.getDepth(zNode)
            kh= self.soilLayers.getKhAtDepth(depth)
            lnTribNod= self.getTributaryLength(n)
            kh*= lnTribNod*coefKh
            if(kh>0.0):
                Kh_x= alphaKh_x*kh
                Kh_y= alphaKh_y*kh
                # print(depth, Kh_x, Kh_y, zMin)
                if(zNode>zMin):
                    kv= 1e-5
                else:
                    kv= alphaKv_z*self.getVerticalStiffnessSinglePile()
                    # pierCode= self.pileSet.getProp('supportCode')
                    # print(pierCode, 'kv= ', kv/1e6)
                retval[n.tag]= [Kh_x, Kh_y, kv]
        return retval

class CircularPile(Pile):
    ''' Pile with circular cross-section.'''
    def __init__(self, E, diameter, soilData, pileSet):
        ''' Constructor.

        :param E: elastic modulus of pile material
        :param diameter: diameter of the pile cross-section.
        :param soilData:  properties of the soil along depth.
        :param pileSet: set of nodes and elements defining a single pile.
        '''
        cs= sp.CircularSection(name= None, Rext= diameter/2.0, Rint=0.0)

        super(CircularPile, self).__init__(E= E, crossSection= cs, soilData= soilData, pileSet= pileSet)

    def getDiameter(self):
        ''' Return the pile diameter.'''
        return self.crossSection.getDiameter()
            
