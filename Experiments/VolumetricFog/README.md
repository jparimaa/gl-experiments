# Volumetric fog

Based on volumetric fog implementation by Bartłomiej Wroński. 

Short overview: The light scattering (i.e. "visible fog") in the view frustum is calculated into a 3D texture (size 160 x 90 x 128) so that each texel represents the amount of scattering at that location. This 3D texture is then summed cumulatively to another 3D texture so that it's possible to quickly sample the total scattering amount at certain location.

More info:

GPU Pro 6, Chapter III: Lighting, section 3 "Volumetric fog and lighting" https://www.amazon.com/GPU-Pro-Advanced-Rendering-Techniques/dp/1482264617

Siggraph 2014 presentation https://bartwronski.files.wordpress.com/2014/08/bwronski_volumetric_fog_siggraph2014.pdf

Reference implementation https://github.com/bartwronski/CSharpRenderer

![fog](fog.png?raw=true "fog")