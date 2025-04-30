# -*- coding: utf-8 -*-
''' Design of hinges under general loading.

   Based on the article:
   Markic T, Kaufmann W.
   Modeling and design of concrete hinges under general loading. 
   Structural Concrete. 2023;24(4):5122–49. 
   https://doi.org/10.1002/suco.202201110
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import sys
from misc_utils import log_messages as lmsg
import scipy.optimize as optimize

class ConcreteHinge(object):
    ''' Concrete hinge design according to the article:

       Markic T, Kaufmann W.
       Modeling and design of concrete hinges under general loading. 
       Structural Concrete. 2023;24(4):5122–49. 
       https://doi.org/10.1002/suco.202201110

    ;ivar phi: concrete internal friction angle.
    :ivar concrete: concrete material.
    :ivar reinfSteel: steel material.
    :ivar b: block length.
    :ivar b1: throath length.
    :ivar b2: length of the block available for load dispersion.
    :ivar d: block width.
    :ivar d1: throath width.
    :ivar d2: width of the block available for load dispersion.
    :ivar ht: height of the hinge throat.
    :ivar hm: height of the hinge "mouth" (distance between the outer edges 
              of opposing blocks).
    :ivar reinforcement: list of area, position and angle tuples
                         [(area, (x,y), angle), ...] of the reinforcing
                         rebars crossing the throat. The position is
                         expressed with respect to the a system with
                         origin in the throath centroid, the x axis
                         parallel to the block length and the y axis
                         parallel to the block width. The angle is
                         measured with respect the xz plane.
    :ivar hf: distance of the first reinforcement layer from the crack base.
    '''
    phi= math.atan(3/4.0) # concrete internal friction angle.

    def __init__(self, concrete, reinfSteel, b, d, b1= None, d1= None, ht= None, hm= None, hf= None, reinforcement= None):
        ''' Constructor.

        :param concrete: concrete material.
        :param reinfSteel: reinforcing steel material.
        :param b: block length.
        :param b1: throath length.
        :param d: block width.
        :param d1: throath width.
        :param ht: height of the hinge throat.
        :param hm: height of the hinge "mouth" (distance between the outer 
                   edges of opposing blocks).
        :param reinforcement: list of area, position and angle tuples
                              [(area, (x,y), angle), ...] of the reinforcing
                              rebars crossing the throat. The position is
                              expressed with respect to the a system with
                              origin in the throath centroid, the x axis
                              parallel to the block length and the y axis
                              parallel to the block width. The angle is
                              measured with respect the xz plane.
        :param hf: distance of the first reinforcement layer from the crack 
                   base.
        '''
        self.concrete= concrete
        self.reinfSteel= reinfSteel
        self.b= b
        self.d= d
        if(d1 is not None):
            if(d1>0.0):
                self.d1= d1
            else:
                self.d1= 1e-4*self.d
        else:
            self.d1= self.getMaxThroathWidth()
        if(b1 is not None):
            self.b1= b1
        else:
            self.b1= self.getMaxThroathLength()
        if(ht is not None): 
            self.ht= ht
        else:
            self.ht= self.getMaxThroathHeight()
        if(hm is not None): 
            self.hm= hm
        else:
            self.hm= self.getMaxMouthOpening()
        if(hf is None):
            self.hf= self.ht+.05  # assuming dimensions in meters.
        else:
            self.hf= hf
        self.reinforcement= reinforcement
 
    def getMaxThroathWidth(self):
        ''' Return the maximum value of the throath width according to the 
            figure 1 (d) of the article.
        '''
        return 0.3*self.d

    def getMaxThroatHeight(self):
        ''' Return the maximum value of the hinge throat height according to
            the figure 1 (d) of the article.
        '''
        return min(0.2*self.d1, .02) # assuming dimensions in meters.
        
    def getMaxMouthOpening(self):
        ''' Return the maximum value of the distance between the outer edges 
            of opposing blocks according to the figure 1 (d) of the article.
        '''
        return 0.1*self.d # tan(beta)= tan(hm/d) < 0.1
    
    def getMaxThroathLength(self):
        ''' Return the maximum value of the hinge throat length according to
            the figure 1 (d) of the article.
        '''
        min_bn= max(0.7*self.d1, .05) # assuming dimensions in meters.
        return self.b-2*min_bn
    
    def getGeometryEfficiency(self, threshold= 1.0, silent= False):
        ''' Check the hinge geometry according to expressions in the figure 1
            (d) of the article. Return a value lesser that one if the geometry
            conditions are fullfilled.

        :param threshold: value of the efficiency that must be exceeded to 
                          be considered an error.
        :param silent: if true, don't issue warning messages.
        '''
        max_d1= self.getMaxThroathWidth()
        cf_d1= self.d1/max_d1
        if(cf_d1>threshold and not silent):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warnMsg= '; throath width d1= '+str(self.d1) + ' too big (greater than '+str(max_d1)+').'
            lmsg.warning(className+'.'+methodName+warnMsg)
        max_ht= self.getMaxThroatHeight()
        cf_ht= self.ht/max_ht
        if(cf_ht>threshold and not silent):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warnMsg= '; throath height ht= '+str(self.ht) + ' too big (greater than '+str(max_ht)+').'
            lmsg.warning(className+'.'+methodName+warnMsg)
        max_hm= self.getMaxMouthOpening()
        cf_hm= self.hm/max_hm
        if(cf_hm>threshold and not silent):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warnMsg= '; mouth opening hm= '+str(self.hm) + ' too big (greater than '+str(max_hm)+').'
            lmsg.warning(className+'.'+methodName+warnMsg)
        max_b1= self.getMaxThroathLength()
        cf_b1= self.b1/max_b1
        if(cf_b1>threshold and not silent):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warnMsg= '; throath length b1= '+str(self.b1) + ' too big (greater than '+str(max_b1)+').'
            lmsg.warning(className+'.'+methodName+warnMsg)
        return max(cf_d1, cf_ht, cf_hm, cf_b1)

    def getThroathArea(self):
        ''' Return the area of the throath.'''
        return self.b1*self.d1
    
    def getMaxReinforcementArea(self):
        ''' Return the maximum reinforcement area according to section 2 of the
            reference document.
        '''
        return .05*self.getThroathArea()
    
    def getEffectiveAs(self):
        ''' Return the effective area of the reinforcement that crosses the 
            throath.'''
        retval= 0.0
        if(self.reinforcement is not None):
            for (a, pos, angle) in self.reinforcement:
                retval+= a*math.cos(angle)
        return retval

    def fsy(self):
        ''' Return the yield stress of the reinforcing steel.'''
        return self.reinfSteel.fyd()

    def fc(self):
        ''' Return the concrete compressive strength.'''
        return self.concrete.fcd()
    
    def getEffectiveUs(self):
        ''' Return the effective tensile capacity of the reinforcement that 
            crosses the throath.'''
        return self.fsy()*self.getEffectiveAs()
    
    def getThetaOpt(self, shearStrongAxis= False):
        ''' Return the value of the angle of the failure surface with the 
            horizontal axis according to expression (38) of the reference
            document.

        :param shearStrongAxis: if true compute the shear strength in the
                                direction of the hinge axis, otherwise compute
                                the shear strength in the direction 
                                perpendicular to the hinge axis.
        '''
        if(shearStrongAxis):
            retval= math.atan2(self.ht, self.d1)
        else:
            retval= math.atan2(self.ht, self.b1)
        return retval

    def getXi(self, Nx):
        ''' Return the value of the xi parameter according to the reference
            document (see its expression in the text following the equation
            (1)).

        :param Nx: axial force (defined negative in compression).
        '''
        Ec= self.concrete.Ecm()
        return 4.5*Ec*self.b1*self.d1/(-Nx)

    def getAc(self, theta):
        ''' Return the area of the concrete failure surface according to
            equation (36) of the reference document.

        :param theta: angle of the failure surface with the horizontal plane.
        '''
        return self.d1*self.b1/math.cos(theta)
    
    def getUc(self, theta):
        ''' Return the compressive capacity of the failure surface.

        :param theta: angle of the failure surface with the horizontal plane.
        '''
        return self.getAc(theta)*self.fc()

    def getDseta(self, Nx, theta):
        ''' Return de value of the dseta parameter according to equation (40)
            of the reference document.

        :param Nx: axial force (defined negative in compression).
        :param theta: angle of the failure surface with the horizontal plane.
        '''
        Us= self.getEffectiveUs()
        F= -Nx+Us
        Uc= self.getUc(theta)
        retval= math.cos(theta)-2*F/Uc
        return retval

    def getPsiOpt(self, Nx, theta):
        ''' Return de value of the psi angle according to equation (39) of the
            reference document.

        :param Nx: axial force (defined negative in compression).
        :param theta: angle of the failure surface with the horizontal plane.
        '''
        retval= self.phi
        # xi= self.getXi(Nx)
        dseta= self.getDseta(Nx= Nx, theta= theta)
        if(dseta>-1):
            retval= max(theta+math.asin(dseta), self.phi)
        return retval

    def getMechanismAShearStrength(self, Nx, theta, psi, shearStrongAxis= False):
        ''' Return the shear strength of the hinge when failure occurs according
            to mechanism A (see equation (39) reference document).

        :param Nx: axial force (defined negative in compression).
        :param theta: angle of the failure surface with the horizontal plane
        :param psi: angle between the failure plane and the displacement 
                    increment vector.
        :param shearStrongAxis: if true compute the shear strength in the
                                direction of the hinge axis, otherwise compute
                                the shear strength in the direction 
                                perpendicular to the hinge axis.
        '''
        Us= self.getEffectiveUs()
        Ac= self.getAc(theta)
        psi_theta= psi-theta
        retval= Us/Ac*math.sin(psi_theta)
        fc= self.fc()
        retval+= fc/2.0*(1-math.sin(psi))
        retval*= Ac/math.cos(psi_theta)
        retval+= -Nx*math.tan(psi_theta)
        return retval
    
    def getMechanismAShearStrengthOpt(self, Nx, shearStrongAxis= False):
        ''' Return the shear strength of the hinge when failure occurs according
            to mechanism A (see equation (39) reference document).

        :param Nx: axial force (defined negative in compression).
        :param shearStrongAxis: if true compute the shear strength in the
                                direction of the hinge axis, otherwise compute
                                the shear strength in the direction 
                                perpendicular to the hinge axis.
        '''
        theta= self.getThetaOpt(shearStrongAxis)
        psi= self.getPsiOpt(Nx= Nx, theta= theta)
        return self.getMechanismAShearStrength(Nx= Nx, theta= theta, psi= psi, shearStrongAxis= shearStrongAxis)

    def getAc1(self, theta, shearStrongAxis= False):
        ''' Return the area of the inclined concrete failure surface according 
            to equation (42) of the reference document.

        :param theta: angle of the failure surface with the horizontal plane.
        :param shearStrongAxis: if true compute the shear strength in the
                                direction of the hinge axis, otherwise compute
                                the shear strength in the direction 
                                perpendicular to the hinge axis.
        '''
        retval= None
        if(shearStrongAxis):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= '; not implemented for shear strong axis yet.'
            lmsg.error(className+'.'+methodName+errMsg)
        else:
            retval= (self.ht*self.b1+(self.hf-self.ht)*self.b)/math.sin(theta)
        return retval
    
    def getAc2(self, theta, shearStrongAxis= False):
        ''' Return the area of the horizontal concrete failure surface according
            to equation (43) of the reference document.

        :param theta: angle of the failure surface with the horizontal plane.
        :param shearStrongAxis: if true compute the shear strength in the
                                direction of the hinge axis, otherwise compute
                                the shear strength in the direction 
                                perpendicular to the hinge axis.
        '''
        retval= None
        if(shearStrongAxis):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= '; not implemented for shear strong axis yet.'
            lmsg.error(className+'.'+methodName+errMsg)
        else:
            retval= ((self.d1+self.d)/2.0-self.hf/math.tan(theta))*self.b
        return retval

    def getMechanismBShearStrength(self, Nx, theta, psi, shearStrongAxis= False):
        ''' Return the shear strength of the hinge when failure occurs according
            to mechanism B (see equation (44) reference document).

        :param Nx: axial force (defined negative in compression).
        :param theta: angle of the failure surface with the horizontal plane
                      (if None, compute the value that gives the minimum).
        :param psi: angle between the failure plane and the displacement 
                    increment vector. Again if None, compute the value that
                    gives the minimum result.
        :param shearStrongAxis: if true compute the shear strength in the
                                direction of the hinge axis, otherwise compute
                                the shear strength in the direction 
                                perpendicular to the hinge axis.
        '''
        # 20250327 LP
        # See: https://github.com/xcfem/xc_studies/blob/main/20250327_concrete_hinge_design/note_about_results_on_figure_11.pdf
        Us= self.getEffectiveUs()
        psi_theta= psi-theta
        tg_psi_theta= math.tan(psi_theta)
        retval= Us*tg_psi_theta
        vc2= 0.2
        Ac1= self.getAc1(theta= theta, shearStrongAxis= shearStrongAxis)
        Ac2= self.getAc2(theta= theta, shearStrongAxis= shearStrongAxis)
        fc= self.fc()
        tmp= ((1-math.sin(psi))+Ac2/Ac1*vc2*(1-math.sin(psi_theta)))
        tmp*= Ac1/math.cos(psi_theta)*fc/2.0
        retval+= tmp
        retval-= Nx*tg_psi_theta
        return retval

    def getMechanismBShearStrengthOpt(self, Nx, shearStrongAxis= False):
        ''' Return the minimum value of the shear strength of the hinge when 
            failure occurs according by computing the values of psi and theta
            that minimizen the resul of equation (44) in the reference document.

        :param Nx: axial force (defined negative in compression).
        :param shearStrongAxis: if true compute the shear strength in the
                                direction of the hinge axis, otherwise compute
                                the shear strength in the direction 
        '''
        def f(params):
            theta= params[0]
            psi= params[1]
            return self.getMechanismBShearStrength(Nx= Nx, theta= theta, psi= psi, shearStrongAxis= shearStrongAxis)
        # Compute initial guess
        theta= self.getThetaOpt(shearStrongAxis)
        psi= self.getPsiOpt(Nx= Nx, theta= theta)
        initial_guess= [theta, psi]
        theta_bounds= (1e-3, math.pi/6.0)
        psi_bounds= (0, math.pi/2.0)
        bounds= [theta_bounds, psi_bounds]
        result= optimize.minimize(fun= f, x0= initial_guess, bounds= bounds)
        if not result.success:
            raise ValueError(result.message)
        fitted_params = result.x
        theta= fitted_params[0]
        psi= fitted_params[1]
        return self.getMechanismBShearStrength(Nx= Nx, theta= theta, psi= psi, shearStrongAxis= shearStrongAxis)
    
    def getShearStrength(self, Nx, shearStrongAxis= False, simplifiedModel= True):
        ''' Return the shear strength of the hinge according to section 5.2
            of the reference document.

        :param Nx: axial force (defined negative in compression).
        :param shearStrongAxis: if true compute the shear strength in the
                                direction of the hinge axis, otherwise compute
                                the shear strength in the direction 
                                perpendicular to the hinge axis.
        :param simplifiedModel: if true use the simplified model explained
                                in the section 5.2.4 of the reference
                                document.
        '''
        retval= None
        if(simplifiedModel):
            # Compute the inclination of the failure surface according
            # to equation (46) of the reference document.
            # 20250327 LP
            # See: https://github.com/xcfem/xc_studies/blob/main/20250327_concrete_hinge_design/note_about_results_on_figure_11.pdf
            theta_opt_simpl= self.getThetaOpt(shearStrongAxis)
            Us= self.getEffectiveUs()
            retval= -Nx+Us
            tg= math.tan(self.phi-theta_opt_simpl)
            retval*= tg
        else:
            # Mechanism A
            shA= self.getMechanismAShearStrengthOpt(Nx= Nx, shearStrongAxis= shearStrongAxis)
            # Mechanism B
            shB= self.getMechanismBShearStrengthOpt(Nx= Nx, shearStrongAxis= shearStrongAxis)
            retval= min(shA, shB)
        return retval
