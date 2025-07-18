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
from scipy import interpolate
from misc_utils import log_messages as lmsg

def get_pile_wall_rotation_muzas(soil, wallHeight, excavationDepth):
    ''' Return an estimation of the rotation of the wall according to the 
        article: "El Coeficiente de Balasto  en el Cálculo de Pantallas." 
        of Fernando Muzás Labad. ETSAM UPM.

    :param soil: soil type.
    :param wallHeight: total wall height.
    :param excavationDepth: depth of the excavation.
    '''
    Kar= soil.Ka() # Rankine active pressure coefficient. 
    K0= soil.K0Jaky() # at rest pressure coefficient. 
    gamma= soil.gamma() # unit weight.
    E= soil.E # soil deformation modulus.
    t= wallHeight-excavationDepth
    return gamma*201.4*math.pow(Kar, 3.36)/E*pow(excavationDepth/t,4.3) # rotation

def get_pile_wall_initial_translation_muzas(soil, wallHeight, excavationDepth):
    ''' Return an estimation of the initial translation of the wall according
        to the  article: "El Coeficiente de Balasto  en el Cálculo de 
        Pantallas." of Fernando Muzás Labad. ETSAM UPM.

    :param soil: soil type.
    :param wallHeight: total wall height.
    :param excavationDepth: depth of the excavation.
    '''
    K0= soil.K0Jaky() # at rest pressure coefficient. 
    gamma= soil.gamma() # unit weight.
    E= soil.E # soil deformation modulus.
    return 2.77778*gamma*excavationDepth/E*K0 # initial translation


def get_pile_wall_displacements_muzas(soil, wallHeight, excavationDepth):
    ''' Return an estimation of the displacements at the top and bottom
        of the wall according to the article: "El Coeficiente de Balasto 
        en el Cálculo de Pantallas." of Fernando Muzás Labad. ETSAM UPM.

    :param soil: soil type.
    :param wallHeight: total wall height.
    :param excavationDepth: depth of the excavation.
    '''
    Kar= soil.Ka() # Rankine active pressure coefficient. 
    K0= soil.K0Jaky() # at rest pressure coefficient. 
    gamma= soil.gamma()
    E= soil.E # soil deformation modulus.
    t= wallHeight-excavationDepth
    Uo= 2.77778*gamma*excavationDepth/E*K0 # initial translation
    #P= 72.5*math.pow(Ka, 3.36)*pow(excavationDepth/t,4.3)
    G= gamma*201.4*math.pow(Kar, 3.36)/E*pow(excavationDepth/t,4.3) # rotation

    Umax= 1.7*excavationDepth*G+Uo
    Umin= -(t-0.7*excavationDepth)*G+Uo
    return Umax, Umin


def get_unload_subgrade_reaction_from_rest_muzas(soil, wallHeight, excavationDepth):
    ''' Return an estimation of the unload subgrade reaction from rest
        to active earth pressure according to the expression of Ka at the end of 
        the  section 6.A of the article: "El Coeficiente de Balasto en el Cálculo
        de  Pantallas."  of Fernando Muzás Labad. ETSAM UPM.

    :param soil: soil type.
    :param wallHeight: total wall height.
    :param excavationDepth: depth of the excavation.
    '''
    E= soil.E # soil deformation modulus.
    sqKar= soil.Ka()**2 # Squared Rankine active pressure coefficient. 
    G= get_pile_wall_rotation_muzas(soil, wallHeight, excavationDepth)
    return 200*math.sqrt(E)*sqKar*math.sqrt(1.0/G)

def get_load_subgrade_reaction_from_rest_muzas(soil, wallHeight, excavationDepth):
    ''' Return an estimation of the load subgrade reaction from rest
        to passive earth pressure according to the expression of Kp at the end of 
        the  section 6.A of the article: "El Coeficiente de Balasto en el Cálculo
        de  Pantallas."  of Fernando Muzás Labad. ETSAM UPM.

    :param soil: soil type.
    :param wallHeight: total wall height.
    :param excavationDepth: depth of the excavation.
    '''
    return 6.0*get_unload_subgrade_reaction_from_rest_muzas(soil, wallHeight, excavationDepth)

def get_reload_subgrade_reaction_from_excavated_rest_muzas(soil, wallHeight, excavationDepth):
    ''' Return an estimation of the load subgrade reaction from the excavated
        rest to initial rest according to the expression of Kr of the section
        6.A of the article: "El Coeficiente de Balasto en el Cálculo
        de  Pantallas."  of Fernando Muzás Labad. ETSAM UPM.

    :param soil: soil type.
    :param wallHeight: total wall height.
    :param excavationDepth: depth of the excavation.
    '''
    Et= soil.E # soil deformation modulus.
    return Et/2.778

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
        sg_v= self.soil.getVerticalStressAtDepth(z= z, waterTableDepth= self.waterTableDepth)
        return self.soil.getActivePressure(sg_v= sg_v)

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

    def getActiveSidePressure(self, withDiagram= False):
        ''' Return the pressure and the lever arm of the active side
            pressure diagram.'''
        pressureDiagram= self.getActiveSidePressureDiagram()
        ## Area of the pressure diagram.
        P= pressureDiagram.getArea()
        ## Lever arm of the pressure diagram
        zP= pressureDiagram.getCenterOfMass().x
        L3= self.getZeroNetPressureDepth()
        z_bar= self.excavationDepth+L3-zP
        if(withDiagram):
            return P, z_bar, pressureDiagram
        else:
            return P, z_bar

    def getSigmaP5(self):
        ''' Return the valu of sigma_p_5 according to the expression (14.11)
            of the book.'''
        Ka= self.soil.Ka()
        Kp= self.soil.Kp()
        L3= self.getZeroNetPressureDepth()
        gamma= self.soil.gamma()
        if(self.waterTableDepth<self.excavationDepth):
            L2= self.excavationDepth-self.waterTableDepth
            gammaSum= self.soil.submergedGamma()
            sigma_p_5= (gamma*self.waterTableDepth+gammaSum*L2)*Kp+gammaSum*L3*(Kp-Ka)
            gmmKpKa= gammaSum*(Kp-Ka)
        else:
            sigma_p_5= gamma*(self.excavationDepth*Kp+L3*(Kp-Ka))
            gmmKpKa= gamma*(Kp-Ka)
        return sigma_p_5, gmmKpKa

    def getL4PolynomialCoeffs(self):
        ''' Return the polynomial corresponding to the expression 14.16
            of the book.'''
        sigma_p_5, denom= self.getSigmaP5()
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

    def getPressureDiagram(self):
        ''' Return the pressure diagram along the wall.'''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; needs debugging. Don\'t use it')
        zi= list() # depths.
        pi= list() # pressures.
        P, z_bar, plgActiveSide= self.getActiveSidePressure(withDiagram= True)
        for pt in plgActiveSide.getVertices():
            zi.append(-pt.x)
            pi.append(pt.y)
        L3= self.getZeroNetPressureDepth()
        L4= self.getL4()
        sigma_p_5, gmmKpKa= self.getSigmaP5()
        sigma_p_3= L4*gmmKpKa # equation (14.7)
        sigma_p_4= sigma_p_5+sigma_p_3 # equation (14.10)
        L5= (sigma_p_3*L4-2.0*P)/(sigma_p_3+sigma_p_4)
        totalDepth= self.excavationDepth+L3+L4
        zi.append(-(totalDepth-L5))
        pi.append(-sigma_p_3)
        zi.append(-totalDepth)
        pi.append(sigma_p_4)
        return interpolate.interp1d(zi, pi, kind='linear', fill_value="extrapolate"), zi

    def getDisplacements(self, wallHeight):
        ''' Return the an estimation of the displacements at the top and bottom
        of the wall according to the article: "El Coeficiente de Balasto 
        en el Cálculo de Pantallas." of Fernando Muzás Labad. ETSAM UPM.

        :param wallHeight: wall total height.
        :param E: soil deformation modulus.
        '''
        return get_pile_wall_displacements_muzas(soil= self.soil, wallHeight= wallHeight, excavationDepth= self.excavationDepth)

    def getRotation(self, wallHeight):
        ''' Return an estimation of the rotation of the wall according to the 
            article: "El Coeficiente de Balasto  en el Cálculo de Pantallas." 
            of Fernando Muzás Labad. ETSAM UPM.

        :param wallHeight: total wall height.
        '''
        return get_pile_wall_rotation_muzas(soil= self.soil, wallHeight= wallHeight, excavationDepth= self.excavationDepth)

    def getInitialTranslation(self, wallHeight):
        ''' Return an estimation of the initial translation of the wall according
            to the  article: "El Coeficiente de Balasto  en el Cálculo de 
            Pantallas." of Fernando Muzás Labad. ETSAM UPM.

        :param wallHeight: total wall height.
        '''
        return get_pile_wall_initial_translation_muzas(soil= self.soil, wallHeight= wallHeight, excavationDepth= self.excavationDepth)


    def getUnloadSubgradeReactionFromRest(self, wallHeight):
        ''' Return an estimation of the unload subgrade reaction from rest
            to active earth pressure according to the expression of Ka at the end of 
            the  section 6.A of the article: "El Coeficiente de Balasto en el Cálculo
            de  Pantallas."  of Fernando Muzás Labad. ETSAM UPM.

        :param wallHeight: total wall height.
        '''
        return get_unload_subgrade_reaction_from_rest_muzas(soil= self.soil, wallHeight= wallHeight, excavationDepth= self.excavationDepth)

    def getLoadLoadSubgradeReactionFromRest(self, wallHeight):
        ''' Return an estimation of the load subgrade reaction from rest
            to passive earth pressure according to the expression of Kp at the end of 
            the  section 6.A of the article: "El Coeficiente de Balasto en el Cálculo
            de  Pantallas."  of Fernando Muzás Labad. ETSAM UPM.

        :param wallHeight: total wall height.
        '''
        return get_load_subgrade_reaction_from_rest_muzas(soil= self.soil, wallHeight= wallHeight, excavationDepth= self.excavationDepth)

    def getReloadSubgradeReactionFromExcavatedRest(self, wallHeight):
        ''' Return an estimation of the load subgrade reaction from the excavated
            rest to initial rest according to the expression of Kr of the section
            6.A of the article: "El Coeficiente de Balasto en el Cálculo
            de  Pantallas."  of Fernando Muzás Labad. ETSAM UPM.

        :param wallHeight: total wall height.
        '''
        return get_reload_subgrade_reaction_from_excavated_rest_muzas(soil= self.soil, wallHeight= wallHeight, excavationDepth= self.excavationDepth)
        
    
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the design results.'''
        # Active side pressure.
        P, z_bar= self.getActiveSidePressure()
        # Theoretical depth
        Dtheor= self.getDepth(depthSafetyFactor= 1.0)
        # Actual depth.
        Dactual= self.getDepth(depthSafetyFactor= 1.3)
        # Total length
        L= self.getTotalLength(depthSafetyFactor= 1.3)
        Mmax= self.getMaxBendingMoment()
        Umax, Umin= self.getDisplacements(wallHeight= L)
        os.write(indentation+'problem title: '+str(self.title)+'\n')
        os.write(indentation+'  excavation depth: H= '+fmt.Length.format(self.excavationDepth)+' m\n')
        if(self.waterTableDepth<L):
            os.write(indentation+'  water table depth: W= '+ fmt.Esf.format(self.waterTableDepth)+' m\n')
        else:
            os.write(indentation+'  water table depth: W= - m\n')
            
        os.write(indentation+'  total pressure: P= '+ fmt.Esf.format(P/1e3)+' kN/m\n')
        os.write(indentation+'  theoretical depth: Dth= '+fmt.Length.format(Dtheor)+' m\n')
        os.write(indentation+'  actual depth: Dact= '+fmt.Length.format(Dactual)+' m\n')
        os.write(indentation+'  total length: L= '+fmt.Length.format(L)+' m\n')
        os.write(indentation+'  maximum bending moment: Mmax= '+ fmt.Esf.format(Mmax/1e3)+' kN.m/m\n')
        if(Umax or Umin):
            os.write(indentation+'  Displacement estimation.\n')
            if(Umax):
                os.write(indentation+'    maximum displacement on wall top: Umax= '+fmt.Length.format(Umax*1000)+' mm\n')
            if(Umin):
                os.write(indentation+'    minimum displacement on wall bottom: Umin= '+fmt.Length.format(Umin*1000)+' mm\n')
            G= self.getRotation(wallHeight= L)
            os.write('    rotation G= '+fmt.Length.format(G*1e3)+' mrad\n')
            Uo= self.getInitialTranslation(wallHeight= L)
            os.write('    initial uniform translation Uo= '+fmt.Length.format(Uo*1e3)+' mm\n')
            Ka= self.getUnloadSubgradeReactionFromRest(wallHeight= L)
            os.write('    unload subgrade reaction from rest Ka= '+fmt.Pressure.format(Ka/1e6)+' MN/m3\n')
            Kr= self.getReloadSubgradeReactionFromExcavatedRest(wallHeight= L)
            os.write('    reload subgrade reaction from excavated rest Kr= '+fmt.Pressure.format(Kr/1e6)+' MN/m3\n')
            Kp= self.getLoadLoadSubgradeReactionFromRest(wallHeight= L)
            os.write('    unload subgrade reaction from rest  Kp= '+fmt.Pressure.format(Kp/1e6)+' MN/m3\n')

    
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

    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the design results.'''
        super().report(os= os, indentation= indentation)
        # Anchor force per unit length of the wall
        F= self.getAnchorForcePerUnitLength()
        os.write(indentation+'  anchor force per unit length of the wall: F= '+ fmt.Esf.format(F/1e3)+' kN/m\n')

    def getDisplacements(self, wallHeight):
        ''' Return the an estimation of the displacements at the top and bottom
        of the wall according to the article: "El Coeficiente de Balasto 
        en el Cálculo de Pantallas." of Fernando Muzás Labad. ETSAM UPM.

        :param wallHeight: wall total height.
        :param E: soil deformation modulus.
        '''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return None, None
        
