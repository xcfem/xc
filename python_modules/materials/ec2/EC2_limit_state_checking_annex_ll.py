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
from typing import NamedTuple
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

# Tolerance for "zero force" [N/m]
FORCE_TOL = 1.0e-3

# ---------------------------------------------------------------------------
# 1) In-plane design of one layer (tension positive, forces per metre)
# ---------------------------------------------------------------------------
class LayerResult(NamedTuple):
    """Result of the in-plane design / check of one layer."""
    f_tx: float          # tensile force for x reinforcement [N/m]
    f_ty: float          # tensile force for y reinforcement [N/m]
    f_c: float           # resultant concrete compressive force [N/m]
    case: str            # text label (diagnostics only, never used in logic)
    biaxial: bool        # True if the layer is in biaxial compression
    both_tension: bool   # True if both reinforcement directions are in tension
    eta: float = 0.0     # steel utilisation (check mode only)
    cot: float = float("nan")  # cot(theta) of the strut vs reference axis
    ref: str = ""        # reference axis of the strut angle ("x" or "y")


def annex_f_layer(nx, ny, nxy):
    """Return a LayerResult (f_tx, f_ty, f_c, case, biaxial, both_tension).

    nx, ny, nxy : in-plane forces of the layer [N/m], tension positive
    f_tx, f_ty  : tensile force to be taken by the x / y reinforcement [N/m]
    f_c         : resultant compressive force in the concrete layer [N/m]
    """
    a = abs(nxy)

    # Case 1: both reinforcement directions in tension, concrete strut at 45 deg
    if nx >= -a and ny >= -a:
        return LayerResult(nx + a, ny + a, 2.0 * a, "1: x+y tension", False, True)

    # Case 2: x direction would be in compression -> no x reinforcement
    if nx < -a:
        fy = ny + nxy ** 2 / abs(nx)
        if fy >= 0.0:
            return LayerResult(0.0, fy, abs(nx) * (1.0 + (nxy / nx) ** 2),
                               "2: x unreinforced", False, False)
        return LayerResult(0.0, 0.0, _principal_compression(nx, ny, nxy),
                           "4: biaxial compr.", True, False)

    # Case 3: y direction would be in compression -> no y reinforcement
    fx = nx + nxy ** 2 / abs(ny)
    if fx >= 0.0:
        return LayerResult(fx, 0.0, abs(ny) * (1.0 + (nxy / ny) ** 2),
                           "3: y unreinforced", False, False)
    return LayerResult(0.0, 0.0, _principal_compression(nx, ny, nxy),
                       "4: biaxial compr.", True, False)


def _principal_compression(nx, ny, nxy):
    n_min = 0.5 * (nx + ny) - math.hypot(0.5 * (nx - ny), nxy)
    return abs(n_min)

def check_layer(nx, ny, nxy, Fx, Fy, cot_min=1.0, cot_max=2.5):
    """CHECK mode: assess PROVIDED reinforcement Fx, Fy [N/m per layer].

    The strut angle is optimised so that the largest steel utilisation
    max(f_x/Fx, f_y/Fy) is as small as possible. The angle is not tied to
    one direction: neither of the two reinforcements is favoured.

    Lower-bound (equilibrium) model, a = |n_xy|, p = f_v - n_v > 0:
        minimum force of the other direction (h) for a given force f_v
            f_h(f_v) = max(0, n_h + a**2 / p)
        concrete stress state  [[q, -a], [-a, p]],  q = f_h - n_h
            f_c = (p + q)/2 + sqrt(((p - q)/2)**2 + a**2)
        for a pure strut (q*p = a**2):  cot(theta) = p/a  (vs reference axis).
    f_h decreases and f_v increases with p, so the min-max utilisation is the
    crossing point (found by bisection). If at that point both steels are
    active, cot(theta) is clamped to [cot_min, cot_max] (EC2 6.2.3); layers
    with an unreinforced direction are not clamped.

    REFERENCE AXIS: cot(theta) is measured from the direction with the larger
    provided reinforcement (direction "v" in the model above); if Fx == Fy
    the x direction is the reference. It is chosen independently for each
    layer.

    Returns a LayerResult: f_tx, f_ty = steel force USED at the optimum
    angle [N/m], eta = max utilisation (<= 1 means the provided steel is
    enough), ref = reference axis used.
    """
    a = abs(nxy)
    ref = "x" if Fx >= Fy else "y"
    if ref == "y": # reference (v) = y, other direction (h) = x
        nh, nv, Fh, Fv = nx, ny, Fx, Fy
    else: # reference (v) = x, other direction (h) = y
        nh, nv, Fh, Fv = ny, nx, Fy, Fx
    tol = FORCE_TOL
    INF = float("inf")

    def eta_of(f, F):
        if F > tol:
            return f / F
        return 0.0 if f <= tol else INF

    def steel_h(fv):
        p = fv - nv
        if p <= 0.0:
            return INF
        return max(0.0, nh + a * a / p)

    def h_dominates(fv):
        eh, ev = eta_of(steel_h(fv), Fh), eta_of(fv, Fv)
        return eh >= ev
    if a < tol: # no in-plane shear
        fh, fv = max(0.0, nh), max(0.0, nv)
    else:
        lo = nv + 1e-9 * max(1.0, a) if nv >= 0.0 else 0.0
        if not h_dominates(lo):
            fv = lo
        else:
            hi = 1.0e12
            for _ in range(100):
                mid = 0.5 * (lo + hi)
                if h_dominates(mid):
                    lo = mid
                else:
                    hi = mid
            fv = hi
        fh = steel_h(fv)
        if fh > tol and fv > tol: # both steels active: strut
            p = fv - nv
            pc = min(max(p, a * cot_min), a * cot_max)
            if abs(pc - p) > 1e-9 * max(1.0, a):
                if nv + pc < 0.0:
                    pc = -nv # vertical steel not needed
                fv = max(0.0, nv + pc)
                fh = steel_h(fv)

    eta = max(eta_of(fh, Fh), eta_of(fv, Fv))
    c = 0.5 * ((fh - nh) + (fv - nv)) + math.hypot(
        0.5 * ((fh - nh) - (fv - nv)), a)
    active = fh > tol and fv > tol and a >= tol
    cot = (fv - nv) / a if active else float("nan")
    if active:
        case = f"check: strut cot={cot:.2f}"
    elif fh <= tol and fv <= tol:
        case = "check: no steel needed"
    elif fh <= tol:
        case = "check: other dir. unreinforced"
    else:
        case = "check: reference dir. unreinforced"
    fx, fy = (fh, fv) if ref == "y" else (fv, fh)
    return LayerResult(fx, fy, c, case, fh <= tol and fv <= tol, active,
                       eta=eta, cot=cot, ref=ref)






# ---------------------------------------------------------------------------
# 2) Sandwich model for one element
# ---------------------------------------------------------------------------

PROV_KEYS = ("asx_bot", "asy_bot", "asx_top", "asy_top")


def design_element(iForces, h, d1, concrete, steel, ts_step=0.005, ts_max_frac=1.0 / 3.0,
                   nu_biaxial=1.0, cot_min=1.0, cot_max=2.5, prov=None, nationalAnnex= None):
    """Analyse the two outer layers of one element.

    :param iForces: internal forces of the shell element (N, m).
                     n_x, n_y, n_xy [N/m] and m_x, m_y, m_xy [N*m/m].
    :param h: cross section thickness.
    :param d1: concrete cover for bars.
    :param concrete: concrete material.
    :param steel: steel material.
    :param ts_step: step between iterations.
    :param ts_max_frac: maximum thickness of the layers expressed as a fraction
                        of the total thickness.
    :param prov: if None  -> DESIGN mode: required reinforcement (Annex F, 45 
                 deg), else dict -> CHECK mode: provided reinforcement [m2/m]
                 with keys asx_bot, asy_bot, asx_top, asy_top; the strut angle 
                 is optimised (cot_min..cot_max) and the steel utilisation
                 eta is returned.
    :param nationalAnnex: identifier of the national annex.

    In both modes the layer thickness ts is iterated so that the concrete
    stress stays below nu*fcd (nu_biaxial*fcd for biaxial compression).

    Returns a dict; reinforcement areas Asx_*/Asy_* in m2/m.
    """
    ts = 2.0 * d1  # initial outer layer thickness
    ts_max = ts_max_frac * h
    fcd= -concrete.fcd()
    fyd= steel.fyd()
    ec2_nu= concrete.getShearStrengthReductionFactor(nationalAnnex)
    while True:
        z = h - ts
        # bottom / top layer membrane forces [N/m]
        nx_b = iForces["n_x"] / 2 + SIGN_M_BOTTOM * iForces["m_x"] / z
        nx_t = iForces["n_x"] / 2 - SIGN_M_BOTTOM * iForces["m_x"] / z
        ny_b = iForces["n_y"] / 2 + SIGN_M_BOTTOM * iForces["m_y"] / z
        ny_t = iForces["n_y"] / 2 - SIGN_M_BOTTOM * iForces["m_y"] / z
        nxy_b = iForces["n_xy"] / 2 + SIGN_MXY * iForces["m_xy"] / z
        nxy_t = iForces["n_xy"] / 2 - SIGN_MXY * iForces["m_xy"] / z

        if prov is None:
            bot = annex_f_layer(nx_b, ny_b, nxy_b)
            top = annex_f_layer(nx_t, ny_t, nxy_t)
        else:
            kw = dict(cot_min=cot_min, cot_max=cot_max)
            # provided area [m2/m] * fyd [Pa] = force [N/m]
            bot = check_layer(nx_b, ny_b, nxy_b, prov["asx_bot"] * fyd,
                              prov["asy_bot"] * fyd, **kw)
            top = check_layer(nx_t, ny_t, nxy_t, prov["asx_top"] * fyd,
                              prov["asy_top"] * fyd, **kw)

        sig_b = bot.f_c / ts # N/m / m = Pa
        sig_t = top.f_c / ts
        # strength limit per layer: nu*fcd (cracked / transverse tension) or
        # nu_biaxial*fcd (biaxial compression)
        lim_b = (nu_biaxial if bot.biaxial else ec2_nu) * fcd
        lim_t = (nu_biaxial if top.biaxial else ec2_nu) * fcd
        util = max(sig_b / lim_b, sig_t / lim_t)
        if util <= 1.0:
            crush_ok = True
            break
        if ts >= ts_max: # cannot thicken the layers any more
            crush_ok = False
            break
        ts = min(ts + ts_step, ts_max)

    return {
        "ts": ts, "z": z, "crush_ok": crush_ok,
        "util_c": util,
        # required (DESIGN) or used (CHECK) area: N/m / Pa = m2/m
        "Asx_bot": bot.f_tx / fyd, "Asy_bot": bot.f_ty / fyd,
        "Asx_top": top.f_tx / fyd, "Asy_top": top.f_ty / fyd,
        "case_bot": bot.case, "case_top": top.case,
        "eta_bot": bot.eta, "eta_top": top.eta, "eta_s": max(bot.eta, top.eta),
        "cot_bot": bot.cot, "cot_top": top.cot,
        "ref_bot": bot.ref, "ref_top": top.ref,
        "layer_forces": {"nx_bot": nx_b, "ny_bot": ny_b, "nxy_bot": nxy_b,
                         "nx_top": nx_t, "ny_top": ny_t, "nxy_top": nxy_t},
    }


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
    def __init__(self, limitStateLabel, ts_max_frac=1.0 / 3.0, nationalAnnex= None):
        ''' Constructor.

        :param limitStateLabel: limit state identifier.
        :param ts_max_frac: maximum thickness of the layers expressed as a 
                            fraction of the total thickness.
        :param nationalAnnex: identifier of the national annex.
        '''
        super(Ec2AnnexLLController, self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= None)
        self.ts_max_frac= ts_max_frac
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
        prov= {'asx_bot': as1_bottom_avail,
               'asy_bot': as2_bottom_avail,
               'asx_top': as1_top_avail,
               'asy_top': as2_top_avail}
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
            bendingResults= design_element(iForces= iForces, h= h, d1= concreteCover, concrete= concrete, steel= steel, ts_max_frac= self.ts_max_frac, prov= prov, nationalAnnex= self.nationalAnnex)
            ts= bendingResults["ts"]
            z= bendingResults["z"]
            concrete_cf= bendingResults["util_c"]
            concrete_OK= bendingResults["crush_ok"]
            # required (DESIGN) or used (CHECK) area: N/m / Pa = m2/m
            as1_top_req= bendingResults["Asx_top"]
            as1_bottom_req= bendingResults["Asx_bot"]
            as2_top_req= bendingResults["Asy_top"]
            as2_bottom_req= bendingResults["Asy_bot"]
            case_bottom= bendingResults["case_bot"]
            case_top= bendingResults["case_top"]
            # Steel utilisation (check mode only)
            eta_bottom= bendingResults["eta_bot"]
            eta_top= bendingResults["eta_top"]
            eta_s= max(eta_bottom, eta_top)
            # Reference axes.
            ref_bot= bendingResults["ref_bot"]
            ref_top= bendingResults["ref_top"]
            layer_forces= bendingResults["layer_forces"]
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
    def __init__(self, designSituations= lsd.default_uls_design_situations, ts_max_frac=1.0 / 3.0, nationalAnnex= None):
        '''Constructor.

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_earthquake, etc.
        :param ts_max_frac: maximum thickness of the layers expressed as a 
                            fraction of the total thickness.
        :param nationalAnnex: identifier of the national annex.
        '''
        super(Ec2AnnexLLLimitStateData,self).__init__(limitStateLabel= 'ULS_Ec2AnnexLLResistance', outputDataBaseFileName= fn.ec2AnnexLLVerificationResultsFile, designSituations= designSituations)
        self.ts_max_frac= ts_max_frac
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
        return Ec2AnnexLLController(limitStateLabel= self.label, ts_max_frac= self.ts_max_frac, nationalAnnex= self.nationalAnnex)
        
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
