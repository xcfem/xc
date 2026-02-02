# Verification tests concerning limit state checking according to EHE-08

## Shear strength checking
Suppose one or more reinforced concrete sections of our model are exhausted under normal stresses (bending and/or tension or compression). In that case, if we perform a nonlinear analysis to obtain the equilibrium of those sections under the same loads, the analysis will crash. Said otherwise, there is no deformation plane that could equilibrate those loads.

If we want to check the shear limit state in such a model, the nonlinear analysis used to obtain the equilibrium of the section and the check the shear its shear strength will crash due to those exhausted sections. Sometimes, it is useful to have an idea of the shear response of our structure despite the fact that some part of it is exhausted. In that case we can instruct the limit state controller to ignore the exhausted sections using the `exhaustedSectionsThresholdCF` attribute. The sections whose capacity factor under normal stresses are greater that the value stored in this attribute are ignored when performing the shear strength checking. By default its value is 1.0.

This attribute can also be used to be a bit more tolerant with the value of the capacity factor. In example [`test_shear_uls_checking_08.py`](https://github.com/xcfem/xc/blob/master/verif/tests/postprocess/limit_state_checking/ehe08/test_shear_uls_checking_08.py), this attribute is also used to check the shear strength of the sections that have a capacity factor under normal stresses equal to or smaller than 1.03.

## Crack control
The value obtained for the 87th element of the phantom model of the example: [test_crack_control_sls_checking_EHE_01.py](https://github.com/xcfem/xc/blob/master/verif/tests/postprocess/limit_state_checking/ehe08/test_crack_control_sls_checking_EHE_01.py) has been tested against the results obtained for the same data with this calculation tool: [Cálculo civil](https://web.archive.org/web/20241008065525/http://calculocivil.com/es/ehe08/fisura/calculo)

The computed results were as follows:

- Calculo civil: $w_k= 0.18$
- XC: $w_k= 0.19$



## References.
- [EHE](https://es.wikipedia.org/wiki/Instrucci%C3%B3n_Espa%C3%B1ola_del_Hormig%C3%B3n_Estructural)
- [Cálculo civil](https://calculocivil.com/es/ehe08/fisura/calculo)
- [Design of concrete slab elements in biaxial bending](https://www.researchgate.net/publication/304624036_DESIGN_OF_CONCRETE_SLAB_ELEMENTS_IN_BIAXIAL_BENDING)
