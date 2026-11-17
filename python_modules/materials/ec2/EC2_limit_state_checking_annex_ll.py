# -*- coding: utf-8 -*-
'''
Reinforcement design of RC shell elements from FE internal forces
using the three-layer "sandwich" model (EN 1992-2 Annex LL approach,
which relies on the in-plane stress design of EN 1992-1-1 Annex F).

'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import sys
import math
import json
import statistics
from collections import defaultdict
from postprocess import limit_state_data as lsd
from postprocess.reports import export_internal_forces as eif
from postprocess.config import file_names as fn
from materials.sections import internal_forces
from materials import limit_state_checking_base as lscb
from materials.ec2 import EC2_limit_state_checking
from postprocess.control_variables import ec2_annex_ll_control_vars as cv

# +1: positive m produces tension at bottom face (n_bot = n/2 + m/z)
# -1: positive m produces tension at top face
SIGN_M_BOTTOM= +1
# Same idea for the twisting moment: n_xy_bot = n_xy/2 + SIGN_MXY * m_xy/z
SIGN_MXY= +1

# ---------------------------------------------------------------------------
# 1) In-plane design of one layer (tension positive, forces per metre)
# ---------------------------------------------------------------------------
def design_layer(nx, ny, nxy):
    ''' Compute the tensile forces in the x/y reinforcement, the compressive
    force in the concrete layer and the description of the load state:

        - 1: x+y tension.
        - 2: x unreinforced.
        - 3: y unreinforced.
        - 4: biaxial compr.

    Return (f_tx, f_ty, f_c, case).

    f_tx, f_ty : tensile force to be taken by the x / y reinforcement [N/m]
    f_c        : resultant compressive force in the concrete layer [N/m]
    '''
    a= abs(nxy)

    # Case 1: both reinforcement directions in tension, concrete strut at 45 deg
    if nx >= -a and ny >= -a:
        return nx + a, ny + a, 2.0 * a, "1: x+y tension"

    # Case 2: x direction would be in compression -> no x reinforcement
    if nx < -a:
        fy= ny + nxy ** 2 / abs(nx)
        if fy >= 0.0:
            return 0.0, fy, abs(nx) * (1.0 + (nxy / nx) ** 2), "2: x unreinforced"
        return 0.0, 0.0, _principal_compression(nx, ny, nxy), "4: biaxial compr."

    # Case 3: y direction would be in compression -> no y reinforcement
    fx= nx + nxy ** 2 / abs(ny)
    if fx >= 0.0:
        return fx, 0.0, abs(ny) * (1.0 + (nxy / ny) ** 2), "3: y unreinforced"
    return 0.0, 0.0, _principal_compression(nx, ny, nxy), "4: biaxial compr."


def _principal_compression(nx, ny, nxy):
    n_min= 0.5 * (nx + ny) - math.hypot(0.5 * (nx - ny), nxy)
    return abs(n_min)


# # ---------------------------------------------------------------------------
# # 2) Sandwich model for one element
# # ---------------------------------------------------------------------------
# def layer_forces(n, m, mxy_like, z):
#     return n / 2.0 + m / z, n / 2.0 - m / z

def design_element(iForces, h, d1, concrete, steel, ts_step=0.005, ts_max_frac=1.0 / 3.0, nationalAnnex= None):
    '''Design the two outer layers. Iterates the layer thickness ts so that the
    concrete stress stays below nu*fcd. Returns dict with results.

    :param iForces: internal forces of the shell element (N, m).
    :param h: cross section thickness.
    :param d1: concrete cover for bars in direction x.
    :param concrete: concrete material.
    :param steel: steel material.
    :param ts_step: step between iterations.
    :param ts_max_frac: maximum thickness of the layers expressed as a fraction
                        of the total thickness.
    :param nationalAnnex: identifier of the national annex.
    '''
    ts= 2.0 * d1 # initial outer layer thickness
    ts_max= ts_max_frac * h
    nu= concrete.getShearStrengthReductionFactor(nationalAnnex)
    fcd= -concrete.fcd()/1e6
    ok= True
    while True:
        z= h - ts
        # bottom / top layer membrane forces
        nx_b= iForces["n_x"] / 2 + SIGN_M_BOTTOM * iForces["m_x"] / z
        nx_t= iForces["n_x"] / 2 - SIGN_M_BOTTOM * iForces["m_x"] / z
        ny_b= iForces["n_y"] / 2 + SIGN_M_BOTTOM * iForces["m_y"] / z
        ny_t= iForces["n_y"] / 2 - SIGN_M_BOTTOM * iForces["m_y"] / z
        nxy_b= iForces["n_xy"] / 2 + SIGN_MXY * iForces["m_xy"] / z
        nxy_t= iForces["n_xy"] / 2 - SIGN_MXY * iForces["m_xy"] / z

        bot= design_layer(nx_b, ny_b, nxy_b)
        top= design_layer(nx_t, ny_t, nxy_t)

        sig_b= bot[2] / 1e6 / ts   # N/m / m -> MPa  (kN/m^2= kPa -> /1000)
        sig_t= top[2] / 1e6 / ts
        sig_max= max(sig_b, sig_t)
        if sig_max <= nu * fcd:
            break
        ts += ts_step
        if ts > ts_max:
            ok= False
            ts= ts_max
            z= h - ts
            break

    fydMPa= steel.fyd()/1e6
    to_m2= 1e-6 / fydMPa # N/m -> m2/m
    retval= {"ts": ts, "z": z, "crush_ok": ok,
             "concrete_cf": sig_max / (nu * fcd),
             "Asx_bot": bot[0] * to_m2, "Asy_bot": bot[1] * to_m2,
             "Asx_top": top[0] * to_m2, "Asy_top": top[1] * to_m2,
             "case_bot": bot[3], "case_top": top[3],
            }
    return retval


# ---------------------------------------------------------------------------
# 3) Out-of-plane shear (central layer) - simplified, no shear reinforcement
# ---------------------------------------------------------------------------
def out_of_plane_shear_check(vx, vy, h, d1, concrete, res, nationalAnnex= None):
    '''Compare resultant shear with V_Rd,c (EN 1992-1-1, 6.2.2(1)).
    Simplifications (conservative): sigma_cp = 0; rho_l = smallest of the
    x / y reinforcement (max of top/bottom in each direction).

    :param vx: sheax along x axis (N/m).
    :param vy: shear along y axis (N/m).
    :param h: section thickness (m).
    :param d1: concret cover (distance face -> centroid of reinforcement).
    :param concrete: concrete material.
    :param res: section reinforcement areas.
    :param nationalAnnex: identifier of the national annex.
    '''
    v_ed= math.hypot(vx, vy) # major principal shear.
    d= (h - d1) # effective depth of the cross-section.
    asx= max(res['Asx_top'], res['Asx_bot']) # m2/m
    asy= max(res['Asy_top'], res['Asy_bot']) 
    rho= min(0.02, min(asx, asy)/d)
    v_rdc_n= EC2_limit_state_checking.getShearResistanceCrackedNoShearReinf(concrete, NEd= 0.0, Ac=d, Asl=rho*d, bw= 1.0, d=d, nationalAnnex= nationalAnnex)
    return v_ed, v_rdc_n, (v_ed / v_rdc_n if v_rdc_n > 0 else float('inf'))


class Ec2AnnexLLController(lscb.LimitStateControllerBase):
    '''Object that controls reinforced concrete ULS using the
       expressions given in Annex LL of EN 1992-2:2005.

    :ivar nationalAnnex: identifier of the national annex.
    '''

    ControlVars= cv.Ec2AnnexLLControlVars
    def __init__(self, limitStateLabel, nationalAnnex= None):
        ''' Constructor.

        :param limitStateLabel: limit state identifier.
        :param crossSections: cross sections on each element.
        :param nationalAnnex: identifier of the national annex.
        '''
        super(Ec2AnnexLLController, self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= None)
        self.nationalAnnex= nationalAnnex

    def initControlVars(self, elements):
        '''Initialize control variables on the given elements.

        :param elements: elements in which the control variables 
                         will be defined.
        '''
        for e in elements:
            e.setProp(self.limitStateLabel, self.ControlVars())
            
    def readInternalForces(self, internalForcesCombFileName, setCalc= None):
        '''Read internal forces from file.

        :param internalForcesCombFileName: Name of the file containing the 
                                           internal forces on the element gauss
                                           points.
        '''
        # Read internal forces results.
        intForcItems= lsd.read_shell_raw_internal_forces_dict(internalForcesCombFileName, setCalc)
        return intForcItems[2]
            
    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the element material subjected to the 
            given internal forces and update its value if its bigger than 
            the previous one.

        :param elem: finite element whose reinforcement will be checked.
        :param elementInternalForces: internal forces on the element.
        '''
        # Get available strengths.
        rcSections= elem.getProp('crossSections') # RC sections of the element.
        rcSection1= rcSections[0]
        rcSection2= rcSections[1]
        h= rcSection1.h # Assume element constant thickness.
        concrete= rcSection1.getConcreteType()
        steel= rcSection1.getReinfSteelType()
        as1_bottom_avail= rcSection1.getAsPos()
        as2_bottom_avail= rcSection2.getAsPos()
        as1_top_avail= rcSection1.getAsNeg()
        as2_top_avail= rcSection2.getAsNeg()
        concreteCover= statistics.mean(rcSection1.getCoverPos()+rcSection2.getCoverPos())
        # Check element section.
        for internalForces in elementInternalForces:
            iForces= {'n_x': internalForces.n1,
                      'n_xy': internalForces.n12,
                      'n_y': internalForces.n2,
                      'm_x': internalForces.m1,
                      'm_xy': internalForces.m12,
                      'm_y': internalForces.m2,
                      'v_x': internalForces.q13,
                      'v_y': internalForces.q23
                      }
            # Normal stresses check.
            bendingResults= design_element(iForces= iForces, h= h, d1= concreteCover, concrete= concrete, steel= steel, ts_max_frac= 1.0 / 10.0, nationalAnnex= self.nationalAnnex)
            as1_top_req= bendingResults["Asx_top"]
            as1_bottom_req= bendingResults["Asx_bot"]
            as2_top_req= bendingResults["Asy_top"]
            as2_bottom_req= bendingResults["Asy_bot"]
            ts= bendingResults["ts"]
            z= bendingResults["z"]
            concrete_cf= bendingResults["concrete_cf"]
            concrete_OK= bendingResults["crush_ok"]
            case_bottom= bendingResults["case_bot"]
            case_top= bendingResults["case_top"]
            
            # Out of plane shear check.
            v_ed, v_rdc, shear_cf = out_of_plane_shear_check(vx= iForces["v_x"], vy= iForces["v_y"], h= h, d1= concreteCover, concrete= concrete, res= bendingResults, nationalAnnex= self.nationalAnnex)


            comb= internalForces.idComb
            cv= elem.getProp(self.limitStateLabel)
            needUpdate= False
            # Update values of control variables when appropriate.
            if(as1_top_req>cv.as1_top_req):
                cv.as1_top_req= as1_top_req
                cv.as1_top_comb= comb
                cv.as1_top_cf= as1_top_req/max(as1_top_avail, 1e-6)
                cv.case_top= case_top
                needUpdate= True
            if(as1_bottom_req>cv.as1_bottom_req):
                cv.as1_bottom_req= as1_bottom_req
                cv.as1_bottom_comb= comb
                cv.as1_bottom_cf= as1_bottom_req/max(as1_bottom_avail, 1e-6)
                cv.case_bottom= case_bottom
                needUpdate= True
            if(as2_top_req>cv.as2_top_req):
                cv.as2_top_req= as2_top_req
                cv.as2_top_comb= comb
                cv.as2_top_cf= as2_top_req/max(as2_top_avail, 1e-6)
                cv.case_top= case_top
                needUpdate= True
            if(as2_bottom_req>cv.as2_bottom_req):
                cv.as2_bottom_req= as2_bottom_req
                cv.as2_bottom_comb= comb
                cv.as2_bottom_cf= as2_bottom_req/max(as2_bottom_avail, 1e-6)
                cv.case_bottom= case_bottom
                needUpdate= True
            if(concrete_cf>cv.concrete_cf):
                cv.concrete_cf= concrete_cf
                cv.concrete_cf_comb= comb
                cv.crush_ok= concrete_OK
                needUpdate= True
            if(shear_cf>cv.shear_cf):
                cv.shear_cf= shear_cf
                cv.v_ed= v_ed
                cv.v_rdc= v_rdc
                cv.shear_cf_comb= comb
                needUpdate= True
            if(needUpdate):
                as1_top_cf= as1_top_req/max(as1_top_avail, 1e-6)
                as1_bottom_cf= as1_bottom_req/max(as1_bottom_avail, 1e-6)
                as2_top_cf= as2_top_req/max(as2_top_avail, 1e-6)
                as2_bottom_cf= as2_bottom_req/max(as2_bottom_avail, 1e-6)
                CF= max(as1_top_cf, as1_bottom_cf, as2_top_cf, as2_bottom_cf, concrete_cf, shear_cf)
                if(CF>cv.CF):
                    cv.CF= CF
                elem.setProp(self.limitStateLabel, cv)
                

class Ec2AnnexLLLimitStateData(lsd.ULS_LimitStateData):
    ''' Limit state checking according to Annex LL of EN 1992-2:2005.

    :ivar nationalAnnex: identifier of the national annex.
    '''
    def __init__(self, designSituations= lsd.default_uls_design_situations, nationalAnnex= None):
        '''Constructor.

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_earthquake, etc.
        '''
        super(Ec2AnnexLLLimitStateData,self).__init__(limitStateLabel= 'ULS_Ec2AnnexLLResistance', outputDataBaseFileName= fn.ec2AnnexLLVerificationResultsFile, designSituations= designSituations)
        self.nationalAnnex= nationalAnnex
       
    def getInternalForcesDict(self, nmbComb, elems):
        '''Creates a dictionary with the element's internal forces.

        :param nmbComb: combination name.
        :param elems: element set.
        '''
        retval= eif.get_internal_forces_dict(nmbComb,elems, shellRawInternalForces= True)
        return retval
   
    def readInternalForces(self, setCalc):
        ''' Read the internal forces for the elements in the set argument.

        :param setCalc: elements to read internal forces for.
        '''
        return lsd.read_shell_raw_internal_forces_dict(self.getInternalForcesFileName(), setCalc)
    
    def getController(self):
        ''' Return a controller corresponding to this limit state.

        :param crossSections: cross sections on each element.
        '''
        return Ec2AnnexLLController(limitStateLabel= self.label, nationalAnnex= self.nationalAnnex)
        
    def checkElements(self, elementsToCheck, outputCfg= lsd.VerifOutVars()):
        '''Checking of internal forces in ultimate limit states
            (see self.dumpCombinations).

        :param elementsToCheck: elements to check.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
        '''
        return elementsToCheck.check(limitStateData= self, outputCfg= outputCfg, threeDim= True)
    
    def runChecking(self, crossSections ,outputCfg):
        '''This method reads, for the elements in setCalc, the internal forces 
        previously calculated and saved in the corresponding file.
        Using the 'initControlVars' and 'updateEfficiencyForSet' methods of 
        the controller, the appropriate attributes are assigned to the 
        elements and the associated limit state verification is run.
        The results are written to a file in order to be displayed or listed.

        :param crossSections: cross sections on each element.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to the 
               result file [defatults to False], generation or not
               of list file [defatults to False, ...)
        '''
        retval= super(Ec2AnnexLLLimitStateData,self).runChecking(outputCfg, sections= [''])
        return retval
    
    def check(self, setCalc, crossSections, controller, appendToResFile=False, listFile=False, calcMeanCF=False):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param crossSections: cross sections on each element.
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  True if results are appended to 
               existing file of results (defaults to False)
        :param listFile: True if latex listing file of results 
               is desired to be generated (defaults to False)
        :param calcMeanCF: True if aveelem.getProp('crossSection')rage capacity factor is
               meant to be calculated (defaults to False)
        '''
        if appendToResFile not in [True,False]:
            lmsg.error("Argument 'appendToResFile' must be True or False")
            exit(1)
        if listFile not in [True,False]:
            lmsg.error("Argument 'listFile' must be True or False")
            exit(1)
        if calcMeanCF not in [True,False]:
            lmsg.error("Argument 'calcMeanCF' must be True or False")
            exit(1)
        # Define 'crossSections' property for elements.
        for elem in setCalc.elements:
            eTag= elem.tag
            sections= crossSections.getSectionDefinitionsForElement(eTag)
            elem.setProp('crossSections', sections)
        outputCfg= lsd.VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return self.runChecking(crossSections= crossSections, outputCfg= outputCfg)

ec2AnnexLLResistance= Ec2AnnexLLLimitStateData()
