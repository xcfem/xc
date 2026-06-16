# -*- coding: utf-8 -*-
'''Implementation of EN 10210-2 annex A formulas with a little help of Gemini
   (and a lot of bugs fixed afterwards).

See https://eurocodeapplied.com/design/en1993/rhs-design-properties.
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
import math
from misc_utils import log_messages as lmsg

# Section axis: 

#   EN 10210-2       XC
#  Blue book

#       ^ Z           ^ Y                    
#       |             |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----


# The axis used in the EN 10210-2 standard are different from those used
# in XC (strong axis parallel to z axis) in other words: values for Y
# and Z axis are swapped with respect to those in the standard.

class RHSSHSectionCalculator(object):
    def __init__(self, density: float = 7.85e3, hot_finished= True):
        """ Sectinal properties for rectangular, including square, hollow
        section. 

        :param density: density (default: 7.85 kg/dm³ or g/cm³)
        :param hot_finised: if true, assume that is a hot finished RHS section,
                            if not, assume that is a cold formed one.
        """
        self.rho = density
        self.hot_finished= hot_finished

    def _get_rhs_radii(self, t: float) -> tuple:
        """Computes external and internal radii based on EN 10210-2 (hot
           finished and EN 10219-2 (Cold formed):

        :param t: wall thickness.
        """
        if(self.hot_finished):
            r_out= 1.5*t
            r_in= t
        else:
            if t <= 6.0e-3:
                r_out = 2.0 * t
            elif t <= 10.0e-3:
                r_out = 2.5 * t
            else:
                r_out = 3.0 * t
            r_in = max(0.0, r_out - t)
        return r_out, r_in

    def classify_rhs(self, h: float, b: float, t: float, fy: float) -> dict:
        """
        Calculates geometric properties (EN 10210-2) and Classifies the section (EN 1993-1-1).
        
        :param h: Outside depth of longer side (m)
        :param b: Outside width of shorter side (m)
        :parm t: Wall thickness (m)
        :param fy : Yield strength of the steel grade (MPa or N/mm²), e.g., 235, 275, 355
        """
        # 1. Radius calculations
        r_out, r_in = self._get_rhs_radii(t)
        
        # 2. Extract straight/flat lengths of walls (c) for slenderness ratios (EN 1993-1-1 Table 5.2)
        # Internal corner radii deduct from the straight parts.
        c_h = h - 2*r_out
        c_b = b - 2*r_out
        
        # 3. Calculate Eurocode 3 material modifier epsilon (ε)
        epsilon = math.sqrt(235.0 / fy)
        
        # 4. Slenderness ratios (c/t)
        ratio_h = c_h / t
        ratio_b = c_b / t
        
        # --- Classification Limits (EN 1993-1-1 Table 5.2 - Internal compression parts) ---
        # Pure Compression Limits
        comp_limit_1 = 33.0 * epsilon
        comp_limit_2 = 38.0 * epsilon
        comp_limit_3 = 42*epsilon
        
        # Pure Bending Limits (Webs / Flanges loaded symmetrically)
        bend_limit_1 = 72*epsilon
        bend_limit_2 = 83.0 * epsilon
        bend_limit_3 = 124.0 * epsilon

        # --- Evaluate Compression Class ---
        # Both axes must be checked; the worst/most slender side controls compression classification
        max_comp_ratio = max(ratio_h, ratio_b)
        if max_comp_ratio <= comp_limit_1:
            class_comp = 1
        elif max_comp_ratio <= comp_limit_2:
            class_comp = 2
        elif max_comp_ratio <= comp_limit_3:
            class_comp = 3
        else:
            class_comp = 4

        # --- Evaluate Bending Class (Major Axis - yy) ---
        # Flange (b) is under compression, Web (h) is under bending
        flange_class_yy = 1 if ratio_b <= comp_limit_1 else (2 if ratio_b <= comp_limit_2 else (3 if ratio_b <= comp_limit_3 else 4))
        web_class_yy = 1 if ratio_h <= bend_limit_1 else (2 if ratio_h <= bend_limit_2 else (3 if ratio_h <= bend_limit_3 else 4))
        class_bending_yy = max(flange_class_yy, web_class_yy)

        # --- Evaluate Bending Class (Minor Axis - zz) ---
        # Flange (h) is under compression, Web (b) is under bending
        flange_class_zz = 1 if ratio_h <= comp_limit_1 else (2 if ratio_h <= comp_limit_2 else (3 if ratio_h <= comp_limit_3 else 4))
        web_class_zz = 1 if ratio_b <= bend_limit_1 else (2 if ratio_b <= bend_limit_2 else (3 if ratio_b <= bend_limit_3 else 4))
        class_bending_zz = max(flange_class_zz, web_class_zz)

        return {"class_comp": class_comp,
                "class_bending_zz": class_bending_yy,
                "class_bending_yy": class_bending_zz
                }
    
    def calculate_rhs(self, h: float, b: float, t: float) -> dict:
        """
        Rectangular/Square Hollow Sections (RHS/SHS) - Annex A.3
        h: Outside depth of longer side (m)
        b: Outside width of shorter side (m)
        t: Wall thickness (m)
        """
        # 1. Determine corner radii (m)
        r_out, r_in = self._get_rhs_radii(t)
        
        # 3. Cross-sectional Area (cm2)
        a= 2*t * (b + h - 2*t) - (4-math.pi) * (r_out**2 - r_in**2)
        if(a<0.0):
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= "; negative area for dimensions: b= "+str(b)
            errorMsg+= ", h= "+str(h)+", t= "+str(t)
            lmsg.error(methodName+errorMsg)
            exit(1)
        
        # 4. Mass per unit length (kg/m)
        m = self.rho * a
        
        # 5. --- Corner Property Correction Factors for Inertia (Major and Minor) ---
        a_g= (1.0 - math.pi/4.0) * r_out**2
        a_si= (1.0 - math.pi/4.0) * r_in**2
        # Distance to centroid of corner properties for Major Axis (yy)
        threePi= 3*math.pi
        pi1012factor= (10-threePi)/(12-threePi)
        h_g_yy = h/2.0 - r_out * pi1012factor
        h_si_yy = (h - 2*t)/2 - r_in*pi1012factor
        
        # Distance to centroid of corner properties for Minor Axis (zz)
        b_g_zz = b/2.0 - r_out * pi1012factor
        b_si_zz = (b - 2*t)/2 - r_in*pi1012factor
        
        # Parallel axis corrections
        i_g_si_factor= 1/3 - math.pi/16 -1.0/(3*(12 - threePi))
        i_g= i_g_si_factor*r_out**4
        i_si= i_g_si_factor*r_in**4
        
        
        # 6. Second Moment of Area Calculations (cm4)
        i_yy = (b * h**3 - (b - 2*t) * (h - 2*t)**3)/12.0 - 4*(i_g+a_g*h_g_yy**2) + 4*(i_si + a_si*h_si_yy**2)
        i_zz = (h * b**3 - (h - 2*t) * (b - 2*t)**3)/12.0 - 4*(i_g+a_g*b_g_zz**2) + 4*(i_si + a_si*b_si_zz**2)

        # Plastic modulus.
        Wypl= b*h**2/4.0-(b-2*t)*(h-2*t)**2/4.0-4*a_g*h_g_yy+4*a_si*h_si_yy
        Wzpl= h*b**2/4.0-(h-2*t)*(b-2*t)**2/4.0-4*a_g*b_g_zz+4*a_si*b_si_zz
        
        # 7. --- Torsional Calculations (Bredt's Thin-Wall Theory with Corner Adjustments) ---
        # Auxiliary quantnties defined in EN 10210-2 §A.3 and EN 10219-2 §B.3
        Rc= (r_out+r_in)/2.0
        h0 = 2*((b - t) + (h - t)) - 2*Rc*(4-math.pi)
        Ah= (b - t) * (h - t) - Rc**2 * (4-math.pi)
        K= 2*Ah*t/h0
        
        # Torsional Inertia Constant (It) [cm4]
        # Standard loop equation variation for closed sections under pure shear torsion
        i_t = t**3*h0/3.0+2*K*Ah
        
        # Torsional Modulus Constant (Ct) [cm3]
        c_t = i_t/(t+K/t)

        t_mm= t*1e3
        if(t_mm.is_integer()):
            thickness_str= str(int(t_mm))
        else:
            thickness_str= str(round(t_mm,1)).replace('.', '_')
        shape_name= 'RHS'+str(int(h*1e3))+'x'+str(int(b*1e3))+'x'+thickness_str
        nu= 0.3
        E= 210e9
        G= E/(2*(1+nu))
        b_plus_h= b+h
        Avy= a*h/b_plus_h
        Avz= a*b/b_plus_h
        Iz= i_yy # Major axis.
        assert(Iz>0.0)
        iz= math.sqrt(Iz/a)
        Wzel= 2*Iz/h
        Iy= i_zz # Major axis.
        assert(Iy>0.0)
        iy= math.sqrt(Iy/a)
        Wyel= 2*Iy/b
        retval= {'nmb':shape_name,
                 'b': b,
                 'h': h,
                 'e': t,
                 'P': m,
                 'A': a,
                 'Iz': Iz, # Major axis.
                 'iz': iz,
                 'Wzel': Wzel,
                 'Wzpl': Wypl,
                 'Iy': Iy, # Minor axis.
                 'iy': iy,
                 'Wyel': Wyel,
                 'Wypl': Wzpl,
                 'It': i_t,
                 'Wt': c_t,
                 'nu': nu,
                 'E': E,
                 'G': G,
                 'Avy': Avy,
                 'Avz': Avz,
                 'alpha': 1.0,
                 }
        return retval

class SHSSHSectionCalculator(RHSSHSectionCalculator):
    
    def classify_shs(self, b: float, t: float, fy: float) -> dict:
        """
        Calculates geometric properties (EN 10210-2) and Classifies the section (EN 1993-1-1).
        
        :param b: Side dimension (m)
        :parm t: Wall thickness (m)
        :param fy : Yield strength of the steel grade (MPa or N/mm²), e.g., 235, 275, 355
        """
        return super().calculate_rhs(h= b, b= b, t= t, fy= fy)
        
    def calculate_shs(self, b: float, t: float) -> dict:
        """
        Rectangular/Square Hollow Sections (RHS/SHS) - Annex A.3

        :param b: Side dimension (m)
        :param t: Wall thickness (m)
        """
        retval= super().calculate_rhs(h= b, b= b, t= t)
        name= retval['nmb']
        retval['nmb']= name.replace('RHS', 'SHS')
        return retval

class CHSSHSectionCalculator(RHSSHSectionCalculator):
    
    def classify_chs(self, d: float, t: float, fy: float) -> dict:
        """
        Calculates geometric properties (EN 10210-2) and classifies the section (EN 1993-1-1).
        
        :param d: diameter (m)
        :parm t: Wall thickness (m)
        :param fy : Yield strength of the steel grade (MPa or N/mm²), e.g., 235, 275, 355
        """
        pass

        
    def calculate_chs(self, d: float, t: float) -> dict:
        """
        Circular Hollow Sections (CHS) - Annex A.3

        :param d: diameter (m)
        :param t: Wall thickness (m)
        """
        # 1. Determine inside diameter.
        d_in= d-2*t
        
        # 3. Cross-sectional Area (cm2)
        a= math.pi*(d**2-d_in**2)/4.0
        if(a<0.0):
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= "; negative area for diameter: d= "+str(d)
            errorMsg+= " and thickness: t= "+str(t)
            lmsg.error(methodName+errorMsg)
            exit(1)
        
        # 4. Mass per unit length (kg/m)
        m = self.rho * a
        
        # 6. Second Moment of Area Calculations (cm4)
        i_yy= math.pi*(d**4-d_in**4)/64.0
        i_zz= i_yy

        # Elastic modulus.
        Wyel= 2*i_yy/d
        Wzel= Wyel

        # Plastic modulus.
        Wypl= (d**3-d_in**3)/6.0
        Wzpl= Wypl
        
        # Torsional Inertia Constant (It) [cm4]
        i_t = 2*i_yy
        
        # Torsional Modulus Constant (Ct) [cm3]
        c_t = 2*Wyel

        t_mm= t*1e3
        if(t_mm.is_integer()):
            thickness_str= str(int(t_mm))
        else:
            thickness_str= str(round(t_mm,1))
        d_mm= d*1e3
        if(d_mm.is_integer()):
            diameter_str= str(int(d_mm))
        else:
            diameter_str= str(round(d_mm,1))
        shape_name= 'CHS_'+diameter_str+'_'+thickness_str
        nu= 0.3
        E= 210e9
        G= E/(2*(1+nu))
        Avy= 2*a/math.pi
        Avz= Avy
        Iz= i_yy # Major axis.
        assert(Iz>0.0)
        iz= math.sqrt(Iz/a)
        Iy= i_zz # Major axis.
        assert(Iy>0.0)
        iy= iz
        retval= {'nmb':shape_name,
                 'D': d,
                 't': t,
                 'P': m,
                 'A': a,
                 'Iz': Iz, # Major axis.
                 'iz': iz,
                 'Wzel': Wzel,
                 'Wzpl': Wypl,
                 'Iy': Iy, # Minor axis.
                 'iy': iy,
                 'Wyel': Wyel,
                 'Wypl': Wzpl,
                 'It': i_t,
                 'Wt': c_t,
                 'nu': nu,
                 'E': E,
                 'G': G,
                 'Avy': Avy,
                 'Avz': Avz,
                 'alpha': 1.0,
                 }
        return retval

