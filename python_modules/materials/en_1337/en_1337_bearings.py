# -*- coding: utf-8 -*-
''' EN 1337-3:2005 (E) Table 3 — Standard sizes for bearings type B.

The function get_rectangular_bearings_table in the en_1337_bearings
module can be used to obtain a table with all the available bearings.
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2025, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from materials.en_1337 import en_1337_3 as en
from materials.en_1337 import en_1337_bearings_dictionaries

rectangular_bearings= en_1337_bearings_dictionaries.rectangular_bearings
round_bearings= en_1337_bearings_dictionaries.round_bearings

class RectangularLaminatedBearing(en.RectangularLaminatedBearing):
    ''' Reinforced rectangular elastomeric bearing according to EN 1337-3:2005.

    '''
    def __init__(self, bearingCode, C, ted, tso= 0.0, G= 0.9e6, gammaM= 1.0, fy= 235e6, bearing_type= None):
        ''' Constructor.

        :param bearingCode: code of the bearing in the data dictionary.
        :param C: side cover.
        :param ted: edge cover.
        :param tso: thickness of outer steel reinforcing plate.
        :param G: nominal value of conventional shear modulus of elastomeric 
                  bearing (seel table 1 of EN 1337-3:2005).
        :param gammaM: partial safety factor which value may be chosen in the 
                       National Annex. The recommended value is γm = 1.00.
        :param fy: yield stress of the reinforcing steel.
        :param bearing_type: string that identifies the type of the bearing 
               in the problem.
        '''
        bearingData= rectangular_bearings[bearingCode]
        a= bearingData['a'] # bearing length.
        b= bearingData['b'] # bearing width.
        ti= bearingData['ti'] # thickness of individual elastomer layer.
        ts= bearingData['ts'] # thickness of steel reinforcing plate.
        n= bearingData['number_of_layers'] # number of elastomer layers.
        Te= n*ti # total elastomer thickness.
        total_steel_thickness= (n+1)*ts
        # Two layers of 2.5 mm one at the top, one at the bottom.
        unloaded_thickness= total_steel_thickness+Te+5e-3
        tb= unloaded_thickness # total height.
        Tb= unloaded_thickness # total nominal thickness of bearing.
        
        super().__init__(a= a, b= b, tb= tb, ti= ti, ts= ts, tso= tso, Te= Te, Tb= Tb, n= n, C= C, ted= ted, G= G, gammaM= gammaM, fy= fy, bearing_type= bearing_type)
        self.code= bearingCode


    def getDict(self):
        ''' Return a dictionary with the values of the object members.'''
        retval= super().getDict()
        retval.update({'code':self.code})
        return retval

    def setFromDict(self, dct):
        ''' Set the member values from those in the given dictionary.'''
        self.code= dct['code']
        super().setFromDict(dct)
        
    @staticmethod
    def getDescriptions():
        ''' Return the descriptions of the object members.'''
        retval= super(RectangularLaminatedBearing, RectangularLaminatedBearing).getDescriptions()
        retval['code']= 'code of the bearing.'
        return retval


def get_rectangular_bearings_table(keys, units_conv_factors):
    ''' Return a table (list of lists) populated with the data of the
        rectangular bearings.

    :param keys: keys corresponding to table columns.
    :param units_conv_factors: unit conversion factors.
    '''
    units_text= list()
    for ucf in units_conv_factors:
        if(ucf is not None):
            if(ucf==1e3):
                units_text.append('(mm)')
            elif(ucf==1e2):
                units_text.append('(cm)')
            elif(ucf==1):
                units_text.append('(m)')
            else:
                units_text.append('?')
        else:
            units_text.append(None)
    key_descr= RectangularLaminatedBearing.getDescriptions()
    descriptions= list()
    for key in keys:
        descriptions.append(key_descr[key])

    retval= list()
    retval.append(descriptions)
    retval.append(keys)
    retval.append(units_text)
    for key in rectangular_bearings:
        bearing= RectangularLaminatedBearing(bearingCode= key, tso= 0.0, C= 5e-3, ted= 7e-3)
        dct= bearing.getDict()
        row= list()
        for key, unit in zip(keys, units_conv_factors):
            value= dct[key]
            if(unit is not None):
                value*= unit
                value= int(value)
            row.append(value)
        retval.append(row)
    return retval
