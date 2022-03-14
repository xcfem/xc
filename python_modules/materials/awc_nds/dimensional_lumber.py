# -*- coding: utf-8 -*-
''' Dimensional lumber reference design values according to chapter 4
    orh the .'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2020, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import scipy.interpolate
from materials.awc_nds import AWCNDS_materials
from materials import typical_materials
from misc_utils import units_utils


def getDressedThickness(nominalThickness, dressedSize= 'Dry'):
    ''' Return the dressed thickness of dimension lumber according to
        table 1A of 2018 National Design Specification Supplement
        (page 13).

    :param nominalThickness: nominal thickness of the member (m).
    :param dressedSize: dressed size to use (defaults to 'Dry').
    '''
    retval= nominalThickness-25.4e-3/2.0
    if(dressedSize!='Dry'): # Green
        retval= nominalThickness-25.4e-3*7/16.0
    return retval

def getNominalThickness(dressedThickness, dressedSize= 'Dry'):
    ''' Return the nominal thickness of dimension lumber according to
        table 1A of 2018 National Design Specification Supplement
        (page 13).

    :param dressedThickness: dressed thickness of the member (m).
    :param dressedSize: dressed size to use (defaults to 'Dry').
    '''
    retval= dressedThickness+25.4e-3/2.0
    if(dressedSize!='Dry'): # Green
        retval= dressedThickness+7/16.0*25.4e-3
    return retval

def getDressedWidth(nominalWidth, dressedSize= 'Dry'):
    ''' Return the dressed width of dimension lumber according to
        table 1A of 2018 National Design Specification Supplement
        (page 13).

    :param nominalWidth: nominal width of the member (m).
    :param dressedSize: dressed size to use (defaults to 'Dry').
    '''
    retval= nominalWidth-25.4e-3/2.0 # <=6 inches
    if(nominalWidth>152.4e-3): # >6 inches
        retval= nominalWidth-0.75*25.4e-3
    if(dressedSize!='Dry'): # Green
        retval= nominalWidth-7/16.0*25.4e-3  # <=4 inches
        if(nominalWidth>152.4e-3): # >6 inches
            retval= nominalWidth-0.5*25.4e-3
        elif(nominalWidth>101.6e-3): # >4 inches
            retval= nominalWidth-3/8.0*25.4e-3
    return retval

def getNominalWidth(dressedWidth, dressedSize= 'Dry'):
    ''' Return the nominal width of dimension lumber according to
        table 1A of 2018 National Design Specification Supplement
        (page 13).

    :param dressedWidth: dressed width of the member (m).
    :param dressedSize: dressed size to use (defaults to 'Dry').
    '''
    retval= dressedWidth+25.4e-3/2.0 # <=6 inches
    if(dressedWidth>139.7e-3): # nominal width >6 inches
        retval= dressedWidth+0.75*25.4e-3
    if(dressedSize!='Dry'): # Green
        retval= dressedWidth+7/16.0*25.4e-3  # nominal width <=4 inches
        if(dressedWidth>142.88e-3): # nominal width >6 inches
            retval= dressedWidth+0.5*25.4e-3
        elif(dressedWidth>90.49e-3): # nominal width >4 inches
            retval= dressedWidth+3/8.0*25.4e-3
    return retval
 

class DimensionLumberWood(AWCNDS_materials.Wood):
    ''' Dimensional lumber material.

    :ivar dressedSize: dressed size to use (defaults to 'Dry').
    '''

    # Flat use factor data.
    flat_use_x=  [1.0,2.0,3.0,4.0,5.0,6.00,8.00,10.0,1000.0]
    flat_use_y3= [1.0,1.0,1.0,1.1,1.1,1.15,1.15,1.20,1.2]
    flat_use_y4= [1.0,1.0,1.0,1.0,1.05,1.05,1.05,1.05,1.1]
    flat_use_interp_3= scipy.interpolate.interp1d(flat_use_x,flat_use_y3)
    flat_use_interp_4= scipy.interpolate.interp1d(flat_use_x,flat_use_y4)
    # Fb size factor data
    fb_size_factor_x=  [1.0,2.0,3.0,4.0,5.0,6.00,8.00,10.0,12.0,14.0,1000.0]
    fb_size_factor_y3= [1.5,1.5,1.5,1.5,1.4,1.30,1.20,1.10,1.00,0.90,0.9]
    fb_size_factor_y4= [1.5,1.5,1.5,1.5,1.4,1.30,1.30,1.20,1.10,1.00,1.0]
    fb_size_factor_interp_3= scipy.interpolate.interp1d(fb_size_factor_x,fb_size_factor_y3)
    fb_size_factor_interp_4= scipy.interpolate.interp1d(fb_size_factor_x,fb_size_factor_y4)
    stud_fb_size_factor_x= [1.0,2.0,3.0,4.0,5.0,6.0,8.00]
    stud_fb_size_factor_y= [1.1,1.1,1.1,1.1,1.0,1.0,1.00]
    stud_fb_size_factor_interp= scipy.interpolate.interp1d(stud_fb_size_factor_x,stud_fb_size_factor_y)
    # Fc size factor data
    fc_size_factor_x= [1.00,2.00,3.00,4.00,5.0,6.0,8.00,10.0,12.0,14.0,1000.0]
    fc_size_factor_y= [1.15,1.15,1.15,1.15,1.1,1.1,1.05,1.00,1.00,0.90,0.9]
    stud_fc_size_factor_x= [1.00,2.00,3.00,4.00,5.0,6.0,8.00]
    stud_fc_size_factor_y= [1.05,1.05,1.05,1.05,1.0,1.0,1.00]
    fc_size_factor_interp= scipy.interpolate.interp1d(fc_size_factor_x,fc_size_factor_y)
    stud_fc_size_factor_interp= scipy.interpolate.interp1d(stud_fc_size_factor_x,stud_fc_size_factor_y)
    
    def __init__(self, name= None, grade= '', sub_grade= '', rho= None, wet= False, dressedSize= 'Dry', c= 0.8):
        '''Constructor.

        :param name: material name.
        :param grade: wood grade.
        :param sub_grade: wood sub-grade.
        :param rho: material density.
        :param wet: if true then wet condition.
        :param dressedSize: dressed size to use (defaults to 'Dry').
        :param c: c coefficient for equation 3.7-1 of AWC_NDS2018 (clause 3.7.1 column stability factor) (defaults to 0.8; must be set to 0.85 for round sections).
        '''
        super(DimensionLumberWood,self).__init__(name)
        self.grade= grade
        self.sub_grade= sub_grade
        self.xc_material= None
        self.rho= rho
        self.wet= wet
        self.dressedSize= dressedSize
        self.c= c # c coefficient for equation 3.7-1
        
    def get37_1c(self):
        ''' Return the c coefficient for equation 3.7-1 of
            AWC_NDS2018 (clause 3.7.1 column stability factor).'''
        return 0.8 # structural glued laminated timber,
                   # structural composite lumber, and cross-laminated timber
        
    def getXCMaterialName(self):
        ''' Return the name for create the corresponding
            XC material.'''
        return self.name+'_'+self.grade+'_'+self.sub_grade
    
    def defXCMaterial(self, overrideRho= None):
        '''Defines the material in XC.

        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        rho= self.rho
        if(overrideRho!=None):
            rho= overrideRho
        if(not self.xc_material):
            self.xc_material= typical_materials.MaterialData(name= self.getXCMaterialName(), E= self.E, nu= self.nu, rho= rho)
        return self.xc_material
    
    def getDressedThickness(self, nominalThickness):
        ''' Return the dressed thickness of dimension lumber according to
            table 1A of 2018 National Design Specification Supplement
            (page 13).

        :param nominalThickness: nominal thickness of the member (m).
        '''
        return getDressedThickness(nominalThickness, self.dressedSize)

    def getNominalThickness(self, dressedThickness):
        ''' Return the nominal thickness of dimension lumber according to
            table 1A of 2018 National Design Specification Supplement
            (page 13).

        :param dressedThickness: dressed thickness of the member (m).
        '''
        return getNominalThickness(dressedThickness, self.dressedSize)

    def getDressedWidth(self, nominalWidth):
        ''' Return the dressed width of dimension lumber according to
            table 1A of 2018 National Design Specification Supplement
            (page 13).

        :param nominalWidth: nominal width of the member (m).
        '''
        return getDressedWidth(nominalWidth, self.dressedSize)

    def getNominalWidth(self, dressedWidth):
        ''' Return the nominal width of dimension lumber according to
            table 1A of 2018 National Design Specification Supplement
            (page 13).

        :param dressedWidth: dressed width of the member (m).
        '''
        return getNominalWidth(dressedWidth, self.dressedSize)

    def getBendingFlatUseFactor(self, b, h):
        ''' Return the flat use factor for the bending design
            value Fb according to National Design Specification table 4A.

        :param b: section width.
        :param h: section depth.
        '''
        retval= 1.0
        if(b>h): # flat use
            f= self.flat_use_interp_3
            if(h>3*units_utils.inchToMeter):
                f= self.flat_use_interp_4
            retval= f(b/units_utils.inchToMeter)
        return retval;
    
    def getBendingSizeFactor(self, b, h):
        ''' Return the size factor for the bending design
            value Fb according to National Design Specification Supplement
            table 4A (page 32).

        :param b: nominal section width.
        :param h: nominal section depth.
        '''
        width= max(b,h)
        thickness= min(b,h)
        retval= 1.0
        if(self.grade in ['structural','no_1', 'no_1_&_Btr','no_2','no_3']):
            f= self.fb_size_factor_interp_3
            if(thickness>=4*units_utils.inchToMeter):
                f= self.fb_size_factor_interp_4
            retval= f(width/units_utils.inchToMeter)
        elif(self.grade == 'stud'):
            if(width<8*units_utils.inchToMeter):
                f= self.stud_fb_size_factor_interp
                retval= f(width/units_utils.inchToMeter)
            else:
                lmsg.error('Stud too wide')
        else:
            lmsg.error('Grade: '+grade+' unknown.')
        return retval;
    
    def getTensionSizeFactor(self, b, h):
        ''' Return the size factor for the tension design
            value Ft according to National Design Specification Supplement
            table 4A.

        :param b: section width.
        :param h: section depth.
        '''
        width= max(b,h)
        thickness= min(b,h)
        retval= 1.0
        if(self.grade in ['structural','no_1','no_2','no_3']):
            f= self.fb_size_factor_interp_3 # Same values
            retval= f(width/units_utils.inchToMeter)
        elif(self.grade == 'stud'):
            if(width<8*units_utils.inchToMeter):
                f= self.stud_fb_size_factor_interp_3 # Same values
                retval= f(width/units_utils.inchToMeter)            
            else:
                lmsg.error('Stud too wide')
        else:
            lmsg.error('Grade: '+grade+' unknown.')
        return retval;
    
    def getCompressionSizeFactor(self, b, h):
        ''' Return the size factor for the compression design
            value Fc according to National Design Specification table 4A.

        :param b: nominal section width.
        :param h: nominal section depth.
        '''
        width= max(b,h)
        thickness= min(b,h)
        retval= 1.0
        if(self.grade in ['structural','no_1','no_2','no_3']):
            f= self.fc_size_factor_interp
            retval= f(width/units_utils.inchToMeter)
        elif(self.grade == 'stud'):
            if(width<8*units_utils.inchToMeter):
                f= self.stud_fc_size_factor_interp
                retval= f(width/units_utils.inchToMeter)            
            else:
                lmsg.error('Stud too wide')
        else:
            lmsg.error('Grade: '+grade+' unknown.')
        return retval;
    
    def getFb(self, b, h):
        ''' Return the value of Fb. Used in BeamMember.getBeamStabilityFactor

        :param b: section width.
        :param h: section depth
        '''
        return self.Fb
    
    def getFt(self, b, h):
        ''' Return the value of Ft.

        :param b: section width.
        :param h: section depth
        '''
        return self.Ft
    
    def getFbAdj(self, b, h, Cr= 1.0):
        ''' Return the adjusted value of Fb according
            to AWC-NDS 2018 table 4.3.1.

        :param b: nominal section width.
        :param h: nominal section depth.
        :param Cr: repetitive member factor
        '''
        C= self.CD*Cr # repetitive member factor.
        # Wet service factor
        threshold= 1150.0*AWCNDS_materials.psi2Pa/0.85
        if(self.wet and self.Fb>threshold):
            C*=0.85
        # Nominal dimensions.
        bNom= self.getNominalThickness(b)
        hNom= self.getNominalWidth(h)
        C*= self.getBendingFlatUseFactor(b= bNom, h= hNom) # Flat use factor
        C*= self.getBendingSizeFactor(b= bNom, h= hNom) # Size factor
        return C*self.getFb(b,h)
    
    def getFtAdj(self, b, h):
        ''' Return the adjusted value of Ft according
            to National Design Specification table 4A.

        :param b: section width.
        :param h: section depth.
        '''
        C= self.CD # load duration factor.
        # No flat use far tension
        bNom= self.getNominalThickness(b)
        hNom= self.getNominalWidth(h)
        C*= self.getTensionSizeFactor(b= bNom, h= hNom) # Size factor
        return C*self.getFt(b,h)
    
    def getFvAdj(self):
        ''' Return the adjusted value of Fv according
            to National Design Specification table 4A.'''
        C= 1.0
        # Wet service factor
        if(self.wet):
            C*=0.97
        return C*self.Fv
    
    def getFc_perpAdj(self, Cb= 1.0):
        ''' Return the adjusted value of Fc_perp according
            to National Design Specification table 4A.

        :param Cb: bearing area factor
        '''
        C= Cb # bearing area factor
        # Wet service factor
        if(self.wet):
            C*=0.67
        return C*self.Fct

    def getFc(self,  b, h):
        ''' Return the value of Fc.

        :param b: section width.
        :param h: section depth.
        '''
        return self.Fc
    
    def getFcAdj(self, b, h):
        ''' Return the adjusted value of Fc according
            to National Design Specification table 4A.

        :param b: section width.
        :param h: section depth.
        '''
        C= self.CD # Wet service factor.
        # Wet service factor
        threshold= 750.0*AWCNDS_materials.psi2Pa/0.8
        if(self.wet and self.Fc>threshold):
            C*=0.8
        # Get nominal dimensions.
        bNom= self.getNominalThickness(b)
        hNom= self.getNominalWidth(h)
        C*= self.getCompressionSizeFactor(b= bNom, h= hNom) # Size factor
        return C*self.getFc(b,h)
    
    def getEAdj(self):
        ''' Return the adjusted value of E according
            to National Design Specification table 4A.'''
        C= 1.0
        # Wet service factor
        if(self.wet):
            C*=0.9
        return C*self.E
    
    def getEminAdj(self):
        ''' Return the adjusted value of Emin according
            to National Design Specification table 4A.'''
        C= 1.0
        # Wet service factor
        if(self.wet):
            C*=0.9
        return C*self.Emin


# Southern Pine reference design values based on SPIB grading rules
# and AWC National Design Specification taken from the document
# Southern Pine Use Guide Metric Edition of Southern Forest 
# Products Association.
# https://members.southernpine.com/publications

southern_pine_data= dict()

southern_pine_data['wide_values']= [0.000, 0.038, 0.140, 0.184, 0.235, 0.286]
southern_pine_data['no_1_Fb']= [10.30E+06, 10.30E+06, 9.30E+06, 8.60E+06, 7.20E+06, 6.90E+06]
southern_pine_data['no_1_Ft']= [6.90E+06, 6.90E+06, 6.00E+06, 5.50E+06, 4.80E+06, 4.50E+06]
southern_pine_data['no_1_Fv']= 1.2e6
southern_pine_data['no_1_Fct']= 3.9e6
southern_pine_data['no_1_Fc']= [11.40E+06, 11.40E+06, 10.70E+06, 10.30E+06, 10.00E+06, 9.70E+06]
southern_pine_data['no_1_E']= 11e9
southern_pine_data['no_1_Emin']= 4e9

southern_pine_data['no_1_dense_Fb']= [11.40E+06, 11.40E+06, 10.30E+06, 9.30E+06, 8.30E+06, 7.60E+06]
southern_pine_data['no_1_dense_Ft']= [7.60E+06, 7.60E+06, 6.90E+06, 6.20E+06, 5.50E+06, 5.20E+06]
southern_pine_data['no_1_dense_Fv']= 1.2e6
southern_pine_data['no_1_dense_Fct']= 4.6e6
southern_pine_data['no_1_dense_Fc']= [12.10E+06, 12.10E+06, 11.40E+06, 11.00E+06, 10.70E+06, 10.30E+06]
southern_pine_data['no_1_dense_E']= 12.4e9
southern_pine_data['no_1_dense_Emin']= 4.6e9

southern_pine_data['no_1_non_dense_Fb']= [9.00E+06, 9.00E+06, 8.30E+06, 7.60E+06, 6.60E+06, 6.20E+06]
southern_pine_data['no_1_non_dense_Ft']= [6.00E+06, 6.00E+06, 5.30E+06, 4.80E+06, 4.30E+06, 4.00E+06]
southern_pine_data['no_1_non_dense_Fv']= 1.2e6
southern_pine_data['no_1_non_dense_Fct']= 3.3e6
southern_pine_data['no_1_non_dense_Fc']= [10.70E+06, 10.70E+06, 10.00E+06, 9.70E+06, 9.70E+06, 9.30E+06]
southern_pine_data['no_1_non_dense_E']= 9.7e9
southern_pine_data['no_1_non_dense_Emin']= 3.5e9

southern_pine_data['no_2_Fb']= [7.60E+06, 7.60E+06, 6.90E+06, 6.40E+06, 5.50E+06, 5.20E+06]
southern_pine_data['no_2_Ft']= [4.70E+06, 4.70E+06, 4.10E+06, 3.80E+06, 3.30E+06, 3.10E+06]
southern_pine_data['no_2_Fv']= 1.2e6
southern_pine_data['no_2_Fct']= 3.9e6
southern_pine_data['no_2_Fc']= [10.00E+06, 10.00E+06, 9.70E+06, 9.30E+06, 9.00E+06, 8.60E+06]
southern_pine_data['no_2_E']= 9.7e9
southern_pine_data['no_2_Emin']= 3.5e9

southern_pine_data['no_2_dense_Fb']= [8.30E+06, 8.30E+06, 7.20E+06, 6.70E+06, 5.90E+06, 5.50E+06]
southern_pine_data['no_2_dense_Ft']= [5.20E+06, 5.20E+06, 4.50E+06, 4.10E+06, 3.60E+06, 3.40E+06]
southern_pine_data['no_2_dense_Fv']= 1.2e6
southern_pine_data['no_2_dense_Fct']= 4.6e6
southern_pine_data['no_2_dense_Fc']= [10.30E+06, 10.30E+06, 10.00E+06, 9.70E+06, 9.30E+06, 9.00E+06]
southern_pine_data['no_2_dense_E']= 11e9
southern_pine_data['no_2_dense_Emin']= 4e9

southern_pine_data['no_2_non_dense_Fb']= [7.20E+06, 7.20E+06, 6.60E+06, 6.00E+06, 5.20E+06, 4.80E+06]
southern_pine_data['no_2_non_dense_Ft']= [4.10E+06, 4.10E+06, 3.60E+06, 3.40E+06, 2.90E+06, 2.80E+06]
southern_pine_data['no_2_non_dense_Fv']= 1.2e6
southern_pine_data['no_2_non_dense_Fct']= 3.3e6
southern_pine_data['no_2_non_dense_Fc']= [10.00E+06, 10.00E+06, 9.30E+06, 9.00E+06, 8.60E+06, 8.60E+06]
southern_pine_data['no_2_non_dense_E']= 9e9
southern_pine_data['no_2_non_dense_Emin']= 3.2e9

southern_pine_data['no_3_Fb']= [4.50E+06, 4.50E+06, 4.00E+06, 3.60E+06, 3.30E+06, 3.10E+06]
southern_pine_data['no_3_Ft']= [2.80E+06, 2.80E+06, 2.40E+06, 2.20E+06, 1.90E+06, 1.70E+06]
southern_pine_data['no_3_Fv']= 1.2e6
southern_pine_data['no_3_Fct']= [3.90E+06, 3.90E+06, 3.90E+06, 3.90E+06, 3.90E+06, 3.90E+06]
southern_pine_data['no_3_Fc']= [5.90E+06, 5.90E+06, 5.50E+06, 5.30E+06, 5.20E+06, 5.00E+06]
southern_pine_data['no_3_E']= 9e9
southern_pine_data['no_3_Emin']= 3.2e9

southern_pine_data['stud_Fb']= southern_pine_data['no_3_Fb']
southern_pine_data['stud_Ft']= southern_pine_data['no_3_Ft']
southern_pine_data['stud_Fv']= southern_pine_data['no_3_Fv']
southern_pine_data['stud_Fct']= southern_pine_data['no_3_Fct']
southern_pine_data['stud_Fc']= southern_pine_data['no_3_Fc']
southern_pine_data['stud_E']= southern_pine_data['no_3_E']
southern_pine_data['stud_Emin']= southern_pine_data['no_3_Emin']

southern_pine_data['structural_Fb']= [16.20E+06, 16.20E+06, 14.50E+06, 13.40E+06, 11.70E+06, 11.00E+06]
southern_pine_data['structural_Ft']= [11.40E+06, 11.40E+06, 10.00E+06, 9.30E+06, 7.90E+06, 7.60E+06]
southern_pine_data['structural_Fv']= 1.2e6
southern_pine_data['structural_Fct']= 3.9e6
southern_pine_data['structural_Fc']= [13.10E+06, 13.10E+06, 12.40E+06, 11.70E+06, 11.40E+06, 11.40E+06]
southern_pine_data['structural_E']= 12.4e9
southern_pine_data['structural_Emin']= 4.6e9

southern_pine_data['structural_dense_Fb']= [18.60E+06, 18.60E+06, 16.50E+06, 15.20E+06, 13.40E+06, 12.40E+06]
southern_pine_data['structural_dense_Ft']= [13.10E+06, 13.10E+06, 11.40E+06, 10.70E+06, 9.00E+06, 8.60E+06]
southern_pine_data['structural_dense_Fv']= 1.2e6
southern_pine_data['structural_dense_Fct']= 4.6e6
southern_pine_data['structural_dense_Fc']= [14.10E+06, 14.10E+06, 13.10E+06, 12.80E+06, 12.40E+06, 12.10E+06]
southern_pine_data['structural_dense_E']= 13.1e9
southern_pine_data['structural_dense_Emin']= 4.8e9

southern_pine_data['structural_non_dense_Fb']= [14.10E+06, 14.10E+06, 12.80E+06, 11.70E+06, 10.30E+06, 9.70E+06]
southern_pine_data['structural_non_dense_Ft']= [10.00E+06, 10.00E+06, 9.00E+06, 8.30E+06, 7.20E+06, 6.70E+06]
southern_pine_data['structural_non_dense_Fv']= 1.2e6
southern_pine_data['structural_non_dense_Fct']= 3.3e6
southern_pine_data['structural_non_dense_Fc']= [12.40E+06, 12.40E+06, 11.70E+06, 11.40E+06, 11.00E+06, 10.70E+06]
southern_pine_data['structural_non_dense_E']= 11e9
southern_pine_data['structural_non_dense_Emin']= 4e9

southern_pine_data['no_1_Fb_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_1_Fb'])
southern_pine_data['no_1_Ft_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_1_Ft'])
southern_pine_data['no_1_Fc_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_1_Fc'])

southern_pine_data['no_1_dense_Fb_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_1_dense_Fb'])
southern_pine_data['no_1_dense_Ft_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_1_dense_Ft'])
southern_pine_data['no_1_dense_Fc_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_1_dense_Fc'])

southern_pine_data['no_1_non_dense_Fb_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_1_non_dense_Fb'])
southern_pine_data['no_1_non_dense_Ft_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_1_non_dense_Ft'])
southern_pine_data['no_1_non_dense_Fc_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_1_non_dense_Fc'])

southern_pine_data['no_2_Fb_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_2_Fb'])
southern_pine_data['no_2_Ft_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_2_Ft'])
southern_pine_data['no_2_Fc_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_2_Fc'])

southern_pine_data['no_2_dense_Fb_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_2_dense_Fb'])
southern_pine_data['no_2_dense_Ft_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_2_dense_Ft'])
southern_pine_data['no_2_dense_Fc_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_2_dense_Fc'])

southern_pine_data['no_2_non_dense_Fb_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_2_non_dense_Fb'])
southern_pine_data['no_2_non_dense_Ft_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_2_non_dense_Ft'])
southern_pine_data['no_2_non_dense_Fc_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_2_non_dense_Fc'])

southern_pine_data['no_3_Fb_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_3_Fb'])
southern_pine_data['no_3_Ft_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_3_Ft'])
southern_pine_data['no_3_Fc_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['no_3_Fc'])

southern_pine_data['stud_Fb_interp']= southern_pine_data['no_3_Fb_interp']
southern_pine_data['stud_Ft_interp']= southern_pine_data['no_3_Ft_interp']
southern_pine_data['stud_Fc_interp']= southern_pine_data['no_3_Fc_interp']

southern_pine_data['structural_Fb_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['structural_Fb'])
southern_pine_data['structural_Ft_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['structural_Ft'])
southern_pine_data['structural_Fc_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['structural_Fc'])

southern_pine_data['structural_dense_Fb_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['structural_dense_Fb'])
southern_pine_data['structural_dense_Ft_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['structural_dense_Ft'])
southern_pine_data['structural_dense_Fc_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['structural_dense_Fc'])

southern_pine_data['structural_non_dense_Fb_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['structural_non_dense_Fb'])
southern_pine_data['structural_non_dense_Ft_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['structural_non_dense_Ft'])
southern_pine_data['structural_non_dense_Fc_interp']= scipy.interpolate.interp1d(southern_pine_data['wide_values'],southern_pine_data['structural_non_dense_Fc'])

southern_pine_data['no_1_E']= 11e9
southern_pine_data['no_1_Emin']= 4e9

class SouthernPineWood(DimensionLumberWood):
    ''' Southern Pine reference design values based on SPIB grading rules
        and AWC National Design Specification'''    
    nu= 0.2
    def __init__(self, name='SouthernPine', grade= 'no_2', sub_grade= ''):
        '''Constructor.'''
        super(SouthernPineWood,self).__init__(name, grade, sub_grade, rho= (657+577)/2.0)
        if(grade=='structural'):
            if(sub_grade==''):
                self.Fb= southern_pine_data['structural_Fb_interp']
                self.Ft= southern_pine_data['structural_Ft_interp']
                self.Fv= southern_pine_data['structural_Fv']
                self.Fct= southern_pine_data['structural_Fct']
                self.Fc= southern_pine_data['structural_Fc_interp']
                self.E= southern_pine_data['structural_E']
                self.Emin= southern_pine_data['structural_Emin']
            elif(sub_grade=='dense'):
                self.Fb= southern_pine_data['structural_dense_Fb_interp']
                self.Ft= southern_pine_data['structural_dense_Ft_interp']
                self.Fv= southern_pine_data['structural_dense_Fv']
                self.Fct= southern_pine_data['structural_dense_Fct']
                self.Fc= southern_pine_data['structural_dense_Fc_interp']
                self.E= southern_pine_data['structural_dense_E']
                self.Emin= southern_pine_data['structural_dense_Emin']
            elif(sub_grade=='non_dense'):
                self.Fb= southern_pine_data['structural_non_dense_Fb_interp']
                self.Ft= southern_pine_data['structural_non_dense_Ft_interp']
                self.Fv= southern_pine_data['structural_non_dense_Fv']
                self.Fct= southern_pine_data['structural_non_dense_Fct']
                self.Fc= southern_pine_data['structural_non_dense_Fc_interp']
                self.E= southern_pine_data['structural_non_dense_E']
                self.Emin= southern_pine_data['structural_non_dense_Emin']
        elif(grade=='no_1'):
            if(sub_grade==''):
                self.Fb= southern_pine_data['no_1_Fb_interp']
                self.Ft= southern_pine_data['no_1_Ft_interp']
                self.Fv= southern_pine_data['no_1_Fv']
                self.Fct= southern_pine_data['no_1_Fct']
                self.Fc= southern_pine_data['no_1_Fc_interp']
                self.E= southern_pine_data['no_1_E']
                self.Emin= southern_pine_data['no_1_Emin']
            elif(sub_grade=='dense'):
                self.Fb= southern_pine_data['no_1_dense_Fb_interp']
                self.Ft= southern_pine_data['no_1_dense_Ft_interp']
                self.Fv= southern_pine_data['no_1_dense_Fv']
                self.Fct= southern_pine_data['no_1_dense_Fct']
                self.Fc= southern_pine_data['no_1_dense_Fc_interp']
                self.E= southern_pine_data['no_1_dense_E']
                self.Emin= southern_pine_data['no_1_dense_Emin']
            elif(sub_grade=='non_dense'):
                self.Fb= southern_pine_data['no_1_non_dense_Fb_interp']
                self.Ft= southern_pine_data['no_1_non_dense_Ft_interp']
                self.Fv= southern_pine_data['no_1_non_dense_Fv']
                self.Fct= southern_pine_data['no_1_non_dense_Fct']
                self.Fc= southern_pine_data['no_1_non_dense_Fc_interp']
                self.E= southern_pine_data['no_1_non_dense_E']
                self.Emin= southern_pine_data['no_1_non_dense_Emin']
        elif(grade=='no_2'):
            if(sub_grade==''):
                self.Fb= southern_pine_data['no_2_Fb_interp']
                self.Ft= southern_pine_data['no_2_Ft_interp']
                self.Fv= southern_pine_data['no_2_Fv']
                self.Fct= southern_pine_data['no_2_Fct']
                self.Fc= southern_pine_data['no_2_Fc_interp']
                self.E= southern_pine_data['no_2_E']
                self.Emin= southern_pine_data['no_2_Emin']
            elif(sub_grade=='dense'):
                self.Fb= southern_pine_data['no_2_dense_Fb_interp']
                self.Ft= southern_pine_data['no_2_dense_Ft_interp']
                self.Fv= southern_pine_data['no_2_dense_Fv']
                self.Fct= southern_pine_data['no_2_dense_Fct']
                self.Fc= southern_pine_data['no_2_dense_Fc_interp']
                self.E= southern_pine_data['no_2_dense_E']
                self.Emin= southern_pine_data['no_2_dense_Emin']
            elif(sub_grade=='non_dense'):
                self.Fb= southern_pine_data['no_2_non_dense_Fb_interp']
                self.Ft= southern_pine_data['no_2_non_dense_Ft_interp']
                self.Fv= southern_pine_data['no_2_non_dense_Fv']
                self.Fct= southern_pine_data['no_2_non_dense_Fct']
                self.Fc= southern_pine_data['no_2_non_dense_Fc_interp']
                self.E= southern_pine_data['no_2_non_dense_E']
                self.Emin= southern_pine_data['no_2_non_dense_Emin']
        elif(grade=='no_3' or grade=='stud'):
            if(sub_grade==''):
                self.Fb= southern_pine_data['no_3_Fb_interp']
                self.Ft= southern_pine_data['no_3_Ft_interp']
                self.Fv= southern_pine_data['no_3_Fv']
                self.Fct= southern_pine_data['no_3_Fct']
                self.Fc= southern_pine_data['no_3_Fc_interp']
                self.E= southern_pine_data['no_3_E']
                self.Emin= southern_pine_data['no_3_Emin']
            else:
                lmsg.error('number 3 and stud grades have not sub grades.')
        else:
            lmsg.error('Grade: '+grade+' unknown.')
        
    def getFb(self,  b, h):
        ''' Return the value of Fc.

        :param b: section width.
        :param h: section depth.
        '''
        return self.Fb(max(b,h))
    
    def getFt(self,  b, h):
        ''' Return the value of Ft.

        :param b: section width.
        :param h: section depth.
        '''
        return self.Ft(max(b,h))

    def getFc(self,  b, h):
        ''' Return the value of Fc.

        :param b: section width.
        :param h: section depth.
        '''
        return self.Fc(max(b,h))
    
    def getBendingSizeFactor(self, b, h):
        ''' Return the size factor for the bending design
            value Fb according to National Design Specification Supplement
            table 4B (page 39).

        :param b: nominal section width.
        :param h: nominal section depth.
        '''
        width= max(b,h)
        thickness= min(b,h)
        retval= 1.0
        for s in ['86', '72', '65']:
            if s in self.grade:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; not implemented yet for grade: '+self.grade)
            if s in self.sub_grade:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; not implemented yet for sub-grade: '+self.sub_grade)
        if(width>=12*units_utils.inchToMeter):
            retval= 0.9
        elif((width>=8*units_utils.inchToMeter) and (abs(thickness-4*units_utils.inchToMeter)<1e-3)):
            retval= 1.1
        return retval;
    
    def getTensionSizeFactor(self, b, h):
        ''' Return the size factor for the tension design
            value Ft according to National Design Specification Supplement
            table 4A.

        :param b: section width.
        :param h: section depth.
        '''
        width= max(b,h)
        thickness= min(b,h)
        retval= 1.0
        for s in ['86', '72', '65']:
            if s in self.grade:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; not implemented yet for grade: '+self.grade)
            if s in self.sub_grade:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; not implemented yet for sub-grade: '+self.sub_grade)
        if(width>=12*units_utils.inchToMeter):
            retval= 0.9
        return retval;
    
    def getCompressionSizeFactor(self, b, h):
        ''' Return the size factor for the compression design
            value Fc according to National Design Specification 
            2018 Supplement table 4B (page 39).

        :param b: nominal section width.
        :param h: nominal section depth.
        '''
        width= max(b,h)
        thickness= min(b,h)
        retval= 1.0
        for s in ['86', '72', '65']:
            if s in self.grade:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; not implemented yet for grade: '+self.grade)
            if s in self.sub_grade:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; not implemented yet for sub-grade: '+self.sub_grade)
        if(width>=12*units_utils.inchToMeter):
            retval= 0.9
        return retval;

# Spruce-Pine-Fir reference design values according to table 4A
# of National Design Specification Supplement page 37
class SprucePineFirWood(DimensionLumberWood):
    ''' Spruce-pine-fir refence design values according to
        table 4A of National Design Specification page 37.'''
    nu= 0.2
    def __init__(self, name='SprucePineFir', grade= 'no_2', sub_grade= ''):
        '''Constructor.'''
        super(SprucePineFirWood,self).__init__(name, grade, sub_grade, rho= (657+577)/2.0)
        self.Fv= 135.0*AWCNDS_materials.psi2Pa
        self.Fct= 425.0*AWCNDS_materials.psi2Pa
        if(grade=='structural'):
            self.Fb= 1150.0*AWCNDS_materials.psi2Pa
            self.Ft= 700.0*AWCNDS_materials.psi2Pa
            self.Fc= 1400.0*AWCNDS_materials.psi2Pa
            self.E= 1.5e6*AWCNDS_materials.psi2Pa
            self.Emin= 550e3*AWCNDS_materials.psi2Pa
        elif((grade=='no_1') or (grade=='no_2')):
            self.Fb= 875.0*AWCNDS_materials.psi2Pa
            self.Ft= 450.0*AWCNDS_materials.psi2Pa
            self.Fc= 1150.0*AWCNDS_materials.psi2Pa
            self.E= 1.4e6*AWCNDS_materials.psi2Pa
            self.Emin= 510e3*AWCNDS_materials.psi2Pa
        elif(grade=='no_3'):
            self.Fb= 500.0*AWCNDS_materials.psi2Pa
            self.Ft= 250.0*AWCNDS_materials.psi2Pa
            self.Fc= 650.0*AWCNDS_materials.psi2Pa
            self.E= 1.2e6*AWCNDS_materials.psi2Pa
            self.Emin= 440e3*AWCNDS_materials.psi2Pa
        elif(grade=='stud'):
            self.Fb= 675.0*AWCNDS_materials.psi2Pa
            self.Ft= 350.0*AWCNDS_materials.psi2Pa
            self.Fc= 725.0*AWCNDS_materials.psi2Pa
            self.E= 1.2e6*AWCNDS_materials.psi2Pa
            self.Emin= 440e3*AWCNDS_materials.psi2Pa
        else:
            lmsg.error('Grade: '+grade+' unknown.')

# Douglas Fir-Larch reference design values according to table 4A
# of National Design Specification Supplement page 34
class DouglasFirLarchWood(DimensionLumberWood):
    ''' Douglas Fir-Larch refence design values according to
        table 4A of National Design Specification page 34.'''
    nu= 0.2
    def __init__(self, name='DouglasFirLarch', grade= 'no_2', sub_grade= ''):
        '''Constructor.'''
        super(DouglasFirLarchWood,self).__init__(name, grade, sub_grade, rho= 500)
        self.Fv= 180.0*AWCNDS_materials.psi2Pa
        self.Fct= 625.0*AWCNDS_materials.psi2Pa
        if(grade=='structural'):
            self.Fb= 1500.0*AWCNDS_materials.psi2Pa
            self.Ft= 1000.0*AWCNDS_materials.psi2Pa
            self.Fc= 1700.0*AWCNDS_materials.psi2Pa
            self.E= 1.9e6*AWCNDS_materials.psi2Pa
            self.Emin= 690e3*AWCNDS_materials.psi2Pa
        elif(grade=='no_1_&_Btr'):
            self.Fb= 1200.0*AWCNDS_materials.psi2Pa
            self.Ft= 800.0*AWCNDS_materials.psi2Pa
            self.Fc= 1550.0*AWCNDS_materials.psi2Pa
            self.E= 1.8e6*AWCNDS_materials.psi2Pa
            self.Emin= 660e3*AWCNDS_materials.psi2Pa
        elif(grade=='no_1'):
            self.Fb= 1000.0*AWCNDS_materials.psi2Pa
            self.Ft= 675.0*AWCNDS_materials.psi2Pa
            self.Fc= 1500.0*AWCNDS_materials.psi2Pa
            self.E= 1.7e6*AWCNDS_materials.psi2Pa
            self.Emin= 620e3*AWCNDS_materials.psi2Pa
        elif(grade=='no_2'):
            self.Fb= 900.0*AWCNDS_materials.psi2Pa
            self.Ft= 575.0*AWCNDS_materials.psi2Pa
            self.Fc= 1350.0*AWCNDS_materials.psi2Pa
            self.E= 1.6e6*AWCNDS_materials.psi2Pa
            self.Emin= 580e3*AWCNDS_materials.psi2Pa
        elif(grade=='no_3'):
            self.Fb= 525.0*AWCNDS_materials.psi2Pa
            self.Ft= 325.0*AWCNDS_materials.psi2Pa
            self.Fc= 775.0*AWCNDS_materials.psi2Pa
            self.E= 1.4e6*AWCNDS_materials.psi2Pa
            self.Emin= 510e3*AWCNDS_materials.psi2Pa
        elif(grade=='stud'):
            self.Fb= 700.0*AWCNDS_materials.psi2Pa
            self.Ft= 450.0*AWCNDS_materials.psi2Pa
            self.Fc= 850.0*AWCNDS_materials.psi2Pa
            self.E= 1.4e6*AWCNDS_materials.psi2Pa
            self.Emin= 510e3*AWCNDS_materials.psi2Pa
        else:
            lmsg.error('Grade: '+grade+' unknown.')

# Hem-Fir reference design values according to table 4A
# of National Design Specification Supplement page 35
class HemFirWood(DimensionLumberWood):
    ''' HemFir Fir-Larch refence design values according to
        table 4A of National Design Specification page 34.'''
    nu= 0.2
    def __init__(self, name='HemFir', grade= 'no_2', sub_grade= ''):
        '''Constructor.'''
        super(HemFirWood,self).__init__(name, grade, sub_grade, rho= 500)
        self.Fv= 150.0*AWCNDS_materials.psi2Pa
        self.Fct= 405.0*AWCNDS_materials.psi2Pa
        if(grade=='structural'): # 2" and wider.
            self.Fb= 1400.0*AWCNDS_materials.psi2Pa
            self.Ft= 925.0*AWCNDS_materials.psi2Pa
            self.Fc= 1500.0*AWCNDS_materials.psi2Pa
            self.E= 1.6e6*AWCNDS_materials.psi2Pa
            self.Emin= 580e3*AWCNDS_materials.psi2Pa
        elif(grade=='no_1_&_Btr'): # 2" and wider.
            self.Fb= 1100.0*AWCNDS_materials.psi2Pa
            self.Ft= 725.0*AWCNDS_materials.psi2Pa
            self.Fc= 1350.0*AWCNDS_materials.psi2Pa
            self.E= 1.5e6*AWCNDS_materials.psi2Pa
            self.Emin= 550e3*AWCNDS_materials.psi2Pa
        elif(grade=='no_1'): # 2" and wider.
            self.Fb=975*AWCNDS_materials.psi2Pa
            self.Ft=625*AWCNDS_materials.psi2Pa
            self.Fc=1350*AWCNDS_materials.psi2Pa
            self.E=1500000*AWCNDS_materials.psi2Pa
            self.Emin=550000*AWCNDS_materials.psi2Pa
        elif(grade=='no_2'): # 2" and wider.
            self.Fb=850*AWCNDS_materials.psi2Pa
            self.Ft=525*AWCNDS_materials.psi2Pa
            self.Fc=1300*AWCNDS_materials.psi2Pa
            self.E=1300000*AWCNDS_materials.psi2Pa
            self.Emin=470000*AWCNDS_materials.psi2Pa
        elif(grade=='no_3'): # 2" and wider.
            self.Fb=500*AWCNDS_materials.psi2Pa
            self.Ft=300*AWCNDS_materials.psi2Pa
            self.Fc=725*AWCNDS_materials.psi2Pa
            self.E=1200000*AWCNDS_materials.psi2Pa
            self.Emin=440000*AWCNDS_materials.psi2Pa
        elif(grade=='stud'):  # 2" and wider.
            self.Fb=675*AWCNDS_materials.psi2Pa
            self.Ft=400*AWCNDS_materials.psi2Pa
            self.Fc=800*AWCNDS_materials.psi2Pa
            self.E=1200000*AWCNDS_materials.psi2Pa
            self.Emin=440000*AWCNDS_materials.psi2Pa
        else:
            lmsg.error('Grade: '+grade+' unknown.')

