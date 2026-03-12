# Uniaxial materials for concrete modelization

## Concrete parameters

- `beta`: tension softening parameter. Higher beta means more softening (tending towards brittle cracking). The value recommended by Tamai et al. is 0.4.


## Creep related parameters

- `age`: concrete age at first loading. The age of concrete at first loading significantly impacts creep, with younger concrete (e.g., 1–14 days) exhibiting substantially higher creep strain than, for instance, 28-day old concrete.
- `tcast`: is the analysis time corresponding to concrete casting in days (note: concrete will not be able to take on loads until the age of 2 days).

### Mechanical and total strains for creep materials.
- `eps_m`, `epsP_m`: mechanical strain (trial and commited values).
- `eps_total`, `epsP_total`: total strain (trial and commited values).

### ACI creep and shrinkage strains.
- `eps_cr`, `epsP_cr`: creep strain (trial and commited values).
- `eps_sh`, `epsP_sh`: shrinkage strain (trial and commited values).

### Model Code 2010 creep and shrinkage strains.
- `eps_crb`, `epsP_crb`: basic creep strain (trial and commited values).
- `eps_crd`, `epsP_crd`: drying creep strain (trial and commited values).
- `eps_shb`, `epsP_shb`: basic shrinkage strain (trial and commited values).
- `eps_shd`, `epsP_shd`: drying shrinkage strain (trial and commited values).

### Model Code 2010 creep and shrinkage parameters.
- `epsba`: ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010.
- `epsbb`: fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
- `epsda`: product of εcds,0 and βRH, as per Model Code 2010.
- `epsdb`: fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
- `phiba`: parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.
- `phibb`: fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
- `phida`: product of βdc(fcm) and β(RH), as per Model Code 2010.
- `phidb`: fitting constant within the drying creep time evolution function as per Model Code 2010.
- `cem`: coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.


## References

 - [Concrete02 Material -- Linear Tension Softening](https://opensees.berkeley.edu/wiki/index.php/Concrete02_Material_--_Linear_Tension_Softening)
 - [Making Sense Out of Concrete02](https://portwooddigital.com/2021/08/22/making-sense-out-of-concrete02/)
 - [Concrete02 with Control of the Initial Stiffness](https://portwooddigital.com/2021/11/06/concrete02-with-control-of-the-initial-stiffness/)
 
### Creep and shrinkage
 - [Minimal Creep and Shrinkage Example](https://portwooddigital.com/2023/05/28/minimal-creep-and-shrinkage-example/)
 - [Supporting documentation for time-dependent concrete material models in OpenSees](https://data.mendeley.com/datasets/z4gxnhchky/4)
 - [Long Term Column Loading](https://portwooddigital.com/2024/11/24/long-term-column-loading/)
 
### Design codes
 - [Eurocode 2](https://eurocodes.jrc.ec.europa.eu/showpage.php?id=132)
 - [ACI concrete codes](https://www.concrete.org/store/productdetail.aspx?ItemID=COLOL&Format=SUBSCRIPTION&Language=English&Units=US_Units)
 - [EHE](https://www.mitma.gob.es/organos-colegiados/mas-organos-colegiados/comision-permanente-del-hormigon/cph/instrucciones/ehe-08-version-en-ingles)

