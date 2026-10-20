# -*- coding: utf-8 -*-
''' Response of piles in weak rock.'''

def alpha_r_reese_1977(RQD):
    ''' Return the strength reduction factor for the ultimate reaction (force 
        per unit length) of the rock according to Reese, L. C. 1997. “Analysis 
        of laterally loaded piles in weak rock,” Journal of Geotechnical and 
        Geoenvironmental Engineering, Vol.123(11), ASCE, Reston, VA.

    Under this formulation, the reduction factor varies from 1.0 (at 0% RQD) 
    down to 0.33 (at 100% RQD). It is a counter-intuitive factor meant to 
    discount the high intact unconfined compressive strength ($q_u$) because 
    completely intact rock fails in a sudden, brittle manner laterally, whereas
    highly fractured weak rock acts as a more predictable, smeared frictional 
    mass.

    :param RQD: Rock Quality Designation expressed as a percentage.
    '''
    return 1-2/3.0*(RQD/100)

def pu_weak_rock_reese_1977(zr, sigma_ci, D, RQD):
    ''' Return the ultimate reaction (force per unit length) of the rock
        according to Reese, L. C. 1997. “Analysis of laterally loaded piles in 
        weak rock,” Journal of Geotechnical and Geoenvironmental Engineering, 
        Vol.123(11), ASCE, Reston, VA.

    :param zr: depth below rock surface.
    :param sigma_ci: uniaxial unconfined compressive strength of intact rock.
    :param D: diameter of the drilled shaft.
    :param RQD: Rock Quality Designation expressed as a percentage.
    '''
    retval= 0.0
    if(zr>0.0):
        # Compute the reduction factor according to Reese artible.
        alpha_r= alpha_r_reese_1977(RQD)
        retval= alpha_r*sigma_ci*D
        if(zr<3*D):
            retval*=(1+1.4*zr/D)
        else:
            retval*= 5.2
    return retval
