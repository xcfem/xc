# -*- coding: utf-8 -*-
''' Sheet-piles according to chapter 14 of the book "Principles of Foundation 
Engineering" of Braja M. Das. Eight Edition. CENGAGE Learning. 2016.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import numpy

class CantileverSheetPile(object):
    ''' Cantilever sheet pile. according to chapter 14
        of the book "Principles of Foundation Engineering"
        of Braja M. Das. Eight Edition. CENGAGE Learning.
        2016.

    :ivar soil: soil type
    :ivar waterTableDepth: depth of the water table (measured from the
                           sheet-pile top). L1 in the book text.
    :ivar excavationDepth: depth of the excavaion (measured from the
                           sheet-pile top). L1 in the book text.
    '''
    def __init__(self, soil, waterTableDepth, excavationDepth):
        ''' Constructor.

        :param soil: soil type.
        :param waterTableDepth: depth of the water table (measured from the
                               sheet-pile top). L1 in the book text.
        :param excavationDepth: depth of the excavaion (measured from the
                               sheet-pile top). L1 in the book text.
        '''
        self.soil= soil
        self.waterTableDepth= waterTableDepth # L1 in the book text.
        self.excavationDepth= excavationDepth # L1+L2 in the book text.

    def getActivePressureAtDepth(self, z):
        ''' Return the active pressure at depth z.

        :param z: depth of the point to calculate pressure for.
        '''
        return self.soil.getActivePressureAtDepth(z= z, waterTableDepth= self.waterTableDepth)

    def getZeroNetPressureDepth(self):
        ''' Return the depth of the point which has zero net pressure (point E
            in the book text).
        '''
        sigma_p_2= self.getActivePressureAtDepth(z= self.excavationDepth)
        Ka= self.soil.Ka()
        Kp= self.soil.Kp() 
        if(self.waterTableDepth<self.excavationDepth):
            gammaSum= self.soil.submergedGamma()
            retval= sigma_p_2/(gammaSum*(Kp-Ka)) # eq. 14.6 in the book.
        else:
            retval= self.excavationDepth*Ka/(Kp-Ka) # eq. 14.28 in the book.
        return retval

    def getActiveSidePressureDiagram(self):
        ''' Return the pressure diagram at the active side of the sheet-pile.
        '''
        # Right side pressure diagram.
        ptA= geom.Pos2d(0.0, 0.0)
        points= [ptA]
        if(self.waterTableDepth<self.excavationDepth):
            sigma_p_1= self.getActivePressureAtDepth(z= self.waterTableDepth)
            ptC= geom.Pos2d(self.waterTableDepth, sigma_p_1)
            points.append(ptC)
        sigma_p_2= self.getActivePressureAtDepth(z= self.excavationDepth)
        ptD= geom.Pos2d(self.excavationDepth, sigma_p_2)
        points.append(ptD)
        L3= self.getZeroNetPressureDepth()
        ptE= geom.Pos2d(self.excavationDepth+L3, 0.0)
        points.append(ptE)
        return geom.Polygon2d(points)

    def getActiveSidePressure(self):
        ''' Return the pressure and the lever arm of the active side
            pressure diagram.'''
        pressureDiagram= self.getActiveSidePressureDiagram()
        ## Area of the pressure diagram.
        P= pressureDiagram.getArea()
        ## Lever arm of the pressure diagram
        zP= pressureDiagram.getCenterOfMass().x
        L3= self.getZeroNetPressureDepth()
        z_bar= self.excavationDepth+L3-zP
        return P, z_bar

    def getL4PolynomialCoeffs(self):
        ''' Return the polynomial corresponding to the expression 14.16
            of the book.'''
        gamma= self.soil.gamma()
        Ka= self.soil.Ka()
        Kp= self.soil.Kp()
        L3= self.getZeroNetPressureDepth()
        if(self.waterTableDepth<self.excavationDepth):
            L2= self.excavationDepth-self.waterTableDepth
            gammaSum= self.soil.submergedGamma()
            sigma_p_5= (gamma*self.waterTableDepth+gammaSum*L2)*Kp+gammaSum*L3*(Kp-Ka)
            denom= gammaSum*(Kp-Ka)
        else:
            sigma_p_5= gamma*(self.excavationDepth*Kp+L3*(Kp-Ka))
            denom= gamma*(Kp-Ka)
        denom2= denom**2
        P, z_bar= self.getActiveSidePressure()
        A1= sigma_p_5/denom
        A2= 8*P/denom
        A3= 6*P*(2*z_bar*denom+sigma_p_5)/denom2
        A4= P*(6*z_bar*sigma_p_5+4*P)/denom2
        return A1, A2, A3, A4
        
    def getL4(self):
        ''' Return the distance from the zero pressure point to the bottom
            end of the sheet-pile.'''
        # Polynomial coefficients.
        A1, A2, A3, A4= self.getL4PolynomialCoeffs()
        # Compute L4
        coeff= [-A4, -A3, -A2, A1, 1]
        roots= numpy.polynomial.polynomial.polyroots(coeff)
        results= list()
        for r in roots:
            if(r.imag<1e-6):
                if(r.real>0):
                    results.append(r.real)
        return float(results[0])

    def getDepth(self, depthSafetyFactor= 1.3):
        ''' Return the sheet-pile depth according to equation (14.12) of the
            book.

        :param depthSafetyFactor: factor that multiplies the theoretical depth.
        '''
        L3= self.getZeroNetPressureDepth()
        L4= self.getL4()
        return depthSafetyFactor*(L3+L4)

    def getTotalLength(self, depthSafetyFactor= 1.3):
        ''' Return the total length of the sheet-pile depth.

        :param depthSafetyFactor: factor that multiplies the theoretical depth.
        '''
        retval= self.excavationDepth
        retval+= self.getDepth(depthSafetyFactor= depthSafetyFactor)
        return retval

    def getMaxBendingMoment(self):
        ''' Return the maximum bending moment according to equation
            (14.22) of the book.'''
        Ka= self.soil.Ka()
        Kp= self.soil.Kp()
        P, z_bar= self.getActiveSidePressure()
        if(self.waterTableDepth<self.excavationDepth):
            gmm= self.soil.submergedGamma()
        else:
            gmm= self.soil.gamma()
        z_p= math.sqrt(2*P/(Kp-Ka)/gmm) # Equation (14.21)
        return P*(z_bar+z_p)-(0.5*gmm*z_p**2*(Kp-Ka))*z_p/3.0# Equation (14.22)
