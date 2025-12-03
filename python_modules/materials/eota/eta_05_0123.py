# -*- coding: utf-8 -*-
''' Mechanical properties of the Dywidag bars according to ETA-05/0123.

Bonded, unbonded, and external post-tensioning kits for prestressing of 
structures with bars, diameter 17,5 - 47 mm
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

from materials import typical_materials

class Bar(object):
    ''' Python representation of a Dywidag bar.

    :ivar designation: designation of the bar.
    :ivar nominalDiameter: nominal diameter of the bar.
    :ivar nominalArea: nominal cross-sectiona area of the bar.
    :ivar maxPrestressingForce: maximum prestressing force.
    :ivar maxOverstressingForce: maximum overstressing force.
    ''' 
    def __init__(self, designation, nominalDiameter, nominalArea, maxPrestressingForce, maxOverstressingForce):
        ''' Constructor.

        :param designation: designation of the bar.
        :param nominalDiameter: nominal diameter of the bar.
        :param nominalArea: nominal cross-sectiona area of the bar.
        :param maxPrestressingForce: maximum prestressing force.
        :param maxOverstressingForce: maximum overstressing force.
        '''
        self.designation= designation
        self.nominalDiameter= nominalDiameter
        self.nominalArea= nominalArea
        self.maxPrestressingForce= maxPrestressingForce
        self.maxOverstressingForce= maxOverstressingForce

    @property
    def elasticModulus(self):
        ''' Returnn the elastic modulus of the bar steel.'''
        return 205e9

    def getInitStressMaterial(self, preprocessor, materialToEncapsulate= None):
        ''' Create a initial stress material for the bar.

        :param preprocessor: pre-processor of the FE problem.
        :param materialToEncapsulate: material model to prestress (if None an
                                      elastic material is used).
        '''
        if(materialToEncapsulate is None):
            materialToEncapsulate= typical_materials.defElasticMaterial(preprocessor= preprocessor, name= 'auto', E= self.elasticModulus)
        newMaterialName= materialToEncapsulate.name+'_init_stress'
        return typical_materials.def_init_stress_material(preprocessor, name= newMaterialName, materialToEncapsulate= materialToEncapsulate.name)

    def getDesignation(self):
        ''' Return the designation of the bar.'''
        return self.designation

    def getNominalDiameter(self):
        ''' Return the nominal diameter of the bar.'''
        return self.nominalDiameter

    def getNominalArea(self):
        ''' Return the nominal area of the bar.'''
        return self.nominalArea
    
    def getMaxPrestressingForce(self):
        ''' Return the maximum prestressing force of the bar.'''
        return self.maxPrestressingForce

    def getMaxOverstressingForce(self):
        ''' Return the maximum overstressing force of the bar.'''
        return self.maxOverstressingForce

# Page 75 of European Technical Assessment ETA-05/0123 of 23.06.2023,
bar_26E= Bar(designation= '26E', nominalDiameter= 26.5e-3, nominalArea= 552e-6, maxPrestressingForce= 415e3, maxOverstressingForce= 438e3)
bar_32E= Bar(designation= '32E', nominalDiameter= 32e-3, nominalArea= 804e-6, maxPrestressingForce= 605e3, maxOverstressingForce= 638e3)
bar_36E= Bar(designation= '36E', nominalDiameter= 36e-3, nominalArea= 1018e-6, maxPrestressingForce= 765e3, maxOverstressingForce= 808e3) 
bar_40E= Bar(designation= '40E', nominalDiameter= 40e-3, nominalArea= 1257e-6, maxPrestressingForce= 944e3, maxOverstressingForce= 997e3)

bar_18WR= Bar(designation= '18WR', nominalDiameter= 17.5e-3, nominalArea= 241e-6, maxPrestressingForce= 204e3, maxOverstressingForce= 219e3)
bar_26WR= Bar(designation= '26WR', nominalDiameter= 26.5e-3, nominalArea= 552e-6, maxPrestressingForce= 464e3, maxOverstressingForce= 499e3) 
bar_32WR= Bar(designation= '32WR', nominalDiameter= 32e-3, nominalArea= 804e-6, maxPrestressingForce= 676e3, maxOverstressingForce= 722e3)
bar_36WR= Bar(designation= '36WR', nominalDiameter= 36e-3, nominalArea= 1018e-6, maxPrestressingForce= 856e3, maxOverstressingForce= 912e3)
bar_40WR= Bar(designation= '40WR', nominalDiameter= 40e-3, nominalArea= 1257e-6, maxPrestressingForce= 1056e3, maxOverstressingForce= 1131e3) 
bar_47WR= Bar(designation= '47WR', nominalDiameter= 47e-3, nominalArea= 1735e-6, maxPrestressingForce= 1457e3, maxOverstressingForce= 1566e3) 

bar_32WS= Bar(designation= '32WS', nominalDiameter= 32e-3, nominalArea= 804e-6, maxPrestressingForce= 676e3, maxOverstressingForce= 722e3)
bar_36WS= Bar(designation= '36WS', nominalDiameter= 36e-3, nominalArea= 1018e-6, maxPrestressingForce= 856e3, maxOverstressingForce= 912e3)

