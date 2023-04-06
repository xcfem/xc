from materials.ehe import EHE_limit_state_checking
from materials.sia262 import SIA262_limit_state_checking

# Rebar types
def define_types(reinfSteel, cover):
    global A08_10; A08_10= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 8e-3, spacing= 0.10, concreteCover= cover)
    global A10_10; A10_10= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 10e-3, spacing= 0.10, concreteCover= cover)
    global A12_10; A12_10= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 12e-3, spacing= 0.10, concreteCover= cover)
    global A14_10; A14_10= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 14e-3, spacing= 0.10, concreteCover= cover)
    global A16_10; A16_10= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 16e-3, spacing= 0.10, concreteCover= cover)
    global A18_10; A18_10= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 18e-3, spacing= 0.10, concreteCover= cover)
    global A20_10; A20_10= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 20e-3, spacing= 0.10, concreteCover= cover)
    global A22_10; A22_10= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 22e-3, spacing= 0.10, concreteCover= cover)
    global A25_10; A25_10= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 25e-3, spacing= 0.10, concreteCover= cover)
    global A26_10; A26_10= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 26e-3, spacing= 0.10, concreteCover= cover)

    global A08_15; A08_15= SIA262_limit_state_checking.SIARebarFamily(steel= reinfSteel, diam=8e-3, spacing= 0.15, concreteCover= cover)
    global A10_15; A10_15= SIA262_limit_state_checking.SIARebarFamily(steel= reinfSteel, diam=10e-3, spacing= 0.15, concreteCover= cover)
    global A12_15; A12_15= SIA262_limit_state_checking.SIARebarFamily(steel= reinfSteel, diam=12e-3, spacing= 0.15, concreteCover= cover)
    global A14_15; A14_15= SIA262_limit_state_checking.SIARebarFamily(steel= reinfSteel, diam=14e-3, spacing= 0.15, concreteCover= cover)
    global A16_15; A16_15= SIA262_limit_state_checking.SIARebarFamily(steel= reinfSteel, diam=16e-3, spacing= 0.15, concreteCover= cover)
    global A18_15; A18_15= SIA262_limit_state_checking.SIARebarFamily(steel= reinfSteel, diam=18e-3, spacing= 0.15, concreteCover= cover)
    global A20_15; A20_15= SIA262_limit_state_checking.SIARebarFamily(steel= reinfSteel, diam=20e-3, spacing= 0.15, concreteCover= cover)
    global A22_15; A22_15= SIA262_limit_state_checking.SIARebarFamily(steel= reinfSteel, diam=22e-3, spacing= 0.15, concreteCover= cover)
    global A25_15; A25_15= SIA262_limit_state_checking.SIARebarFamily(steel= reinfSteel, diam=25e-3, spacing= 0.15, concreteCover= cover)
    global A26_15; A26_15= SIA262_limit_state_checking.SIARebarFamily(steel= reinfSteel, diam=26e-3, spacing= 0.15, concreteCover= cover)

    global A08_20; A08_20= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 8e-3, spacing= 0.20, concreteCover= cover)
    global A10_20; A10_20= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 10e-3, spacing= 0.20, concreteCover= cover)
    global A12_20; A12_20= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 12e-3, spacing= 0.20, concreteCover= cover)
    global A14_20; A14_20= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 14e-3, spacing= 0.20, concreteCover= cover)
    global A16_20; A16_20= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 16e-3, spacing= 0.20, concreteCover= cover)
    global A18_20; A18_20= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 18e-3, spacing= 0.20, concreteCover= cover)
    global A20_20; A20_20= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 20e-3, spacing= 0.20, concreteCover= cover)
    global A22_20; A22_20= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 22e-3, spacing= 0.20, concreteCover= cover)
    global A25_20; A25_20= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 25e-3, spacing= 0.20, concreteCover= cover)
    global A26_20; A26_20= EHE_limit_state_checking.EHERebarFamily(steel= reinfSteel, diam= 26e-3, spacing= 0.20, concreteCover= cover)

