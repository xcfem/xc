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
import sys
from postprocess.reports import common_formats as fmt

class CantileverSheetPileWall(object):
    ''' Cantilever sheet pile. according to chapter 14
        of the book "Principles of Foundation Engineering"
        of Braja M. Das. Eight Edition. CENGAGE Learning.
        2016.

    :ivar title: problem title.
    :ivar soil: soil type
    :ivar waterTableDepth: depth of the water table (measured from the
                           sheet-pile top). L1 in the book text.
    :ivar excavationDepth: depth of the excavation (measured from the
                           sheet-pile top). L1 in the book text.
    '''
    def __init__(self, title, soil, waterTableDepth, excavationDepth):
        ''' Constructor.

        :param title: problem title
        :param soil: soil type.
        :param waterTableDepth: depth of the water table (measured from the
                               sheet-pile top). L1 in the book text.
        :param excavationDepth: depth of the excavation (measured from the
                               sheet-pile top). L1 in the book text.
        '''
        self.title= title
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
        A0= 1.0 # L4^4 coefficient
        A1= sigma_p_5/denom # L4^3 coefficient
        A2= 8*P/denom # L4^2 coefficient
        A3= 6*P*(2*z_bar*denom+sigma_p_5)/denom2 # L4^1 coefficient
        A4= P*(6*z_bar*sigma_p_5+4*P)/denom2 # L4^0 coefficient
        return A0, A1, -A2, -A3, -A4
        
    def getL4(self):
        ''' Return the distance from the zero pressure point to the bottom
            end of the sheet-pile.'''
        # Polynomial coefficients.
        A0, A1, A2, A3, A4= self.getL4PolynomialCoeffs()
        # Compute L4
        coeff= [A4, A3, A2, A1, A0]
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

    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the design results.'''
        # Active side pressure.
        P, z_bar= sheetPile.getActiveSidePressure()
        # Theoretical depth
        Dtheor= sheetPile.getDepth(depthSafetyFactor= 1.0)
        # Actual depth.
        Dactual= sheetPile.getDepth(depthSafetyFactor= 1.3)
        # Total length
        L= sheetPile.getTotalLength(depthSafetyFactor= 1.3)
        Mmax= sheetPile.getMaxBendingMoment()
        os.write(indentation+'problem title: '+str(self.title)+'\n')
        os.write(indentation+'  excavation depth: H= '+fmt.Length.format(self.excavationDepth)+' m\n')
        os.write(indentation+'  total pressure: P= '+ fmt.Esf.format(P)+' kN/m\n')
        os.write(indentation+'  theoretical depth: Dth= '+fmt.Length.format(Dtheor)+' m\n')
        os.write(indentation+'  actual depth: Dact= '+fmt.Length.format(Dactual)+' m\n')
        os.write(indentation+'  total length: L= '+fmt.Length.format(L)+' m\n')
        os.write(indentation+'  maximum bending moment: Mmax= '+ fmt.Esf.format(Mmax)+' kN.m/m\n')
    
class AnchoredSheetPileWall(CantileverSheetPileWall):
    ''' Anchored sheet pile. according to chapter 14
        of the book "Principles of Foundation Engineering"
        of Braja M. Das. Eight Edition. CENGAGE Learning.
        2016.

    :ivar anchorDepth: depth of the anchor (measured from the
                       sheet-pile top). l1 in the book text.
    '''
    def __init__(self, title, soil, waterTableDepth, excavationDepth, anchorDepth):
        ''' Constructor.

        :param title: problem title.
        :param soil: soil type.
        :param waterTableDepth: depth of the water table (measured from the
                               sheet-pile top). L1 in the book text.
        :param excavationDepth: depth of the excavation (measured from the
                               sheet-pile top). L1 in the book text.
        :param anchorDepth: depth of the anchor (measured from the
                            sheet-pile top). l1 in the book text.
        '''
        super().__init__(title= title, soil= soil, waterTableDepth= waterTableDepth, excavationDepth= excavationDepth)
        self.anchorDepth= anchorDepth
        
    def getL4PolynomialCoeffs(self):
        ''' Return the polynomial corresponding to the expression 14.16
            of the book.'''
        gamma= self.soil.gamma()
        Ka= self.soil.Ka()
        Kp= self.soil.Kp()
        L3= self.getZeroNetPressureDepth()
        l1= self.anchorDepth
        if(self.waterTableDepth<self.excavationDepth):
            L1= self.waterTableDepth
            L2= self.excavationDepth-L1
            l2= self.waterTableDepth-l1
            gammaSum= self.soil.submergedGamma()
            denom= gammaSum*(Kp-Ka)
        else:
            L1= self.excavationDepth
            L2= 0.0
            l2= self.excavationDepth-l1
            denom= gamma*(Kp-Ka)
        P, z_bar= self.getActiveSidePressure()
        A0= 0.0 # L4^4 coefficient
        A1= 1.0 # L4^3 coefficient
        A2= 1.5*(l2+L2+L3) # L4^2 coefficient
        A3= 0.0 # L4^1 coefficient
        A4= 3*P*((L1+L2+L3)-(z_bar+l1))/denom # L4^0 coefficient
        return A0, A1, A2, A3, -A4

    def getAnchorForcePerUnitLength(self):
        ''' Return the anchor force per unit length of the wall.'''
        Ka= self.soil.Ka()
        Kp= self.soil.Kp()
        P, z_bar= self.getActiveSidePressure()
        if(self.waterTableDepth<self.excavationDepth):
            gmm= self.soil.submergedGamma()
        else:
            gmm= self.soil.gamma()
        L4= self.getL4()
        return P-0.5*gmm*(Kp-Ka)*L4**2

    def getZeroShearDepth(self):
        ''' Return the depth for zero shear according to equation 14.69
            of the book.'''
        if(self.waterTableDepth<self.excavationDepth):
            L1= self.waterTableDepth
            gmm= self.soil.submergedGamma()
        else:
            L1= self.excavationDepth
            gmm= self.soil.gamma()
        sigma_p_1= self.getActivePressureAtDepth(z= L1)
        Ka= self.soil.Ka()
        F= self.getAnchorForcePerUnitLength()
        # Using x= z-L1
        c= 0.5*sigma_p_1*L1-F
        b= sigma_p_1
        a= 0.5*gmm*Ka
        x= (-b+math.sqrt(b**2-4*a*c))/(2.0*a)
        z= x+L1
        return z

    def getMaxBendingMoment(self):
        ''' Return the maximum bending moment according to equation
            (14.22) of the book.'''
        l1= self.anchorDepth
        if(self.waterTableDepth<self.excavationDepth):
            L1= self.waterTableDepth
            gmm= self.soil.submergedGamma()
            l2= self.waterTableDepth-l1
        else:
            L1= self.excavationDepth
            gmm= self.soil.gamma()
            l2= self.excavationDepth-l1
        Ka= self.soil.Ka()
        z= self.getZeroShearDepth()
        x= z-L1
        sigma_p_1= self.getActivePressureAtDepth(z= L1)
        F= self.getAnchorForcePerUnitLength()
        m1= -0.5*sigma_p_1*L1*(x+L1/3.0)
        m2= F*(x+l2)
        m3= -(sigma_p_1*x**2/2)
        m4= -(0.5*Ka*gmm*x**3/3.0)
        return m1+m2+m3+m4

