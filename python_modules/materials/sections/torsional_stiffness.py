# -*- coding: utf-8 -*-
''' Calculation of cross-section torsional stiffness.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (A_OO) "
__copyright__= "Copyright 2026, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com "

import math
import numpy as np

##   Return the torsion constant of a box 
##   according to the book "Puentes (apuntes para su diseño
##   y construcción)" by Javier Manterola Armisén (section 5.2.3 page 251)
##
##                        bs
##             |----------------------|
##
##                        ts
##    -    ---------------------------------
##    |         \                    /
##    |h         \td                /
##    |           \       ti       /
##    -            ----------------
##
##                        bi
##                 |--------------|
##

def get_box_girder_torsional_modulus(bs,bi,h,ts,ti,td):
    '''
    Return torsional section modulus of the section.

    :param bs: Upper deck width (without the overhangs)
    :param bi: Lower deck width.
    :param ts: Upper deck thickness.
    :param ti: Lower deck thickness.
    :param td: Thickness of the webs.
    :param h: Box depth (between mid-planes).
    '''
    longAlma=math.sqrt(h**2+((bs-bi)/2)**2)
    return (bs+bi)**2*h**2/(bs/ts+2*longAlma/td+bi/ti)

def calculate_bredt_torsion(enclosed_area, segment_lengths, thickness):
    '''
    Calculates torsional constant J_t based on Bredt's formula.
    
    :param enclosed_area (float): Area enclosed by the mean perimeter (A_e).
    :param segment_lengths (list or np.array): Lengths of each segment (s)
    :param thickness (list or np.array): Thickness of each segment (t)
        
    :return: Torsional constant (J_t)
    '''
    # Summation of s_i / t_i
    perimeter_sum = np.sum(np.array(segment_lengths) / np.array(thickness))
    
    # J_t formula: 4 * A_e^2 / sum(s/t)
    j_t = (4 * (enclosed_area ** 2)) / perimeter_sum
    
    return j_t

# https://cubus-software.zendesk.com/hc/en-us/articles/360001982873-Torsional-constant-Ix-of-cross-sections-in-FAGUS
def estimate_bred_torsion_for_polygonal_section(extPolygon, intPolygon):
    '''
    Estimates torsional constant J_t based on Bredt's formula.
    
    :param extPolygon: external contour of the section.
    :param intPolygon: internal contour of the section.
        
    :return: Estimated value of the torsional constant (J_t)
    '''
    # Estimation of the perimeter of the equivalent box section.
    u= (extPolygon.getPerimeter()+intPolygon.getPerimeter())/2.0
    # Computation of the area.
    A= extPolygon.getArea()-intPolygon.getArea()
    # Estimation of the thickness.
    t= A/u
    # Estimation of the area enclosed by the equivalent box section.
    Ao= extPolygon.getArea()-0.5*u*t

    return 4*Ao**2/(u/t)

