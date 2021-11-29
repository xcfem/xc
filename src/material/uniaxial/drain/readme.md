# DRAIN materials
Explanation for the contents of this directory has been taken from
this [post](https://portwooddigital.com/2019/12/08/an-update-of-the-imk-models/).

The degrading bilinear, pinching, and peak-oriented models that would become the IMK (Ibarra-Medina-Krawinkler) family of histeretic models were first developed by Rahmana and Krawinkler for NLDYNA. The models were also implemented in DRAIN-2DX. There is a little known UniaxialMaterial class called DrainMaterial that wraps FORTRAN subroutines that fit the DRAIN-2DX call signatures. If you look in the src/material/uniaxial/drain directory now, you’ll see only one proof-of-concept implementation for a bilinear hardening material. For whatever reason, those DRAIN-2DX hysteretic models never made their way in to OpenSees.

## References

- [DRAIN-2DX User Guide](https://ntrl.ntis.gov/NTRL/dashboard/searchResults/titleDetail/PB91212530.xhtml)
- [Modernizing old FORTRAN](https://fortranwiki.org/fortran/show/Modernizing+Old+Fortran)
