# Uniaxial materials
A UniaxialMaterial object represents uniaxial stress-strain (or force-deformation) relationships.

## Tangent stiffness for elastic material
The damping tangent (eta parameter) of the elastic uniaxial materials represents a material-level tangent viscosity that creates a viscous stress proportional to the strain rate. Combined, an elastic material with a non-zero $eta$ behaves physically like a Kelvin-Voigt viscoelastic element (a spring and a dashpot connected in parallel), generating a stress-strain relation of: 

$$
\sigma (t)=E\cdot \varepsilon (t)+\eta \cdot \.{\varepsilon }(t)
$$

### Purpose and usage

- It allows you to introduce internal or material-specific viscous damping directly into specific elements, fibers, or springs without relying exclusively on global Rayleigh Damping.
- It is frequently used in geotechnical or structural components (such as dashpot boundary elements or soil-structure interaction springs like PySimple1 or TzSimple1 extensions) to represent localized energy dissipation or radiation damping.

### How to calculate eta
Because the material operates like a Kelvin-Voigt element, its relationship to the structural damping ratio ($\zeta$) at a specific angular frequency $\omega$ (in radians/second) is defined by:

$$
\eta =\frac{2\cdot \zeta \cdot E}{\omega }
$$

Where:

• $\zeta$ (Zeta): The target damping ratio (e.g., $0.02$ for $2\%$ or $0.05$ for $5\%$).
• $E$: The elastic modulus or stiffness assigned to that specific material.
• $\omega $: The targeted natural frequency of the component or structure ($\omega = 2\pi f$).

## References
- [OpenSees page about uniaxial materials](https://opensees.berkeley.edu/wiki/index.php/UniaxialMaterial_Command)
- [Uniaxial multi-tool](https://portwooddigital.com/2020/12/09/uniaxial-multi-tool/)
- [Hysteretic pinching parameters](https://portwooddigital.com/2020/12/27/hysteretic-pinching-parameters)
- [Elastoplastic Calibration](https://portwooddigital.com/2021/05/19/elastoplastic-calibration/)
- [Hysteretic Material](https://opensees.berkeley.edu/wiki/index.php/Hysteretic_Material)
- [Series material](https://opensees.berkeley.edu/wiki/index.php/Series_Material
- [Parallel material](https://opensees.berkeley.edu/wiki/index.php/Parallel_Material)
- [Elastic-Perfectly Plastic Material](https://opensees.berkeley.edu/wiki/index.php/Elastic-Perfectly_Plastic_Material)
- [Concrete Zero](https://portwooddigital.com/2023/07/09/concrete-zero/)
- [Multi-Linear Parallel](https://portwooddigital.com/2024/09/08/multi-linear-parallel/)

## Material testing

- [Material Testing with White Noise](https://portwooddigital.com/2024/04/21/material-testing-with-white-noise/)
- [Minimal Damper Example](https://portwooddigital.com/2025/01/12/minimal-damper-example/)
