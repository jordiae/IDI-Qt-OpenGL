#version 330 core

out vec4 FragColor;

uniform vec3 colFocus;
uniform vec3 llumAmbient;
uniform vec3 posFocus; // en SCO


in vec3 matamb_fs;
in vec3 matdiff_fs;
in vec3 matspec_fs;
in float matshin_fs;

in vec3 NormSCO;
in vec4 vertexSCO;



vec3 Lambert (vec3 NormSCO, vec3 L) 
{
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats

    // Inicialitzem color a component ambient
    vec3 colRes = llumAmbient * matamb_fs;

    // Afegim component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colFocus * matdiff_fs * dot (L, NormSCO);
    return (colRes);
}

vec3 Phong (vec3 NormSCO, vec3 L, vec4 vertSCO) 
{
    // Els vectors estan normalitzats

    // Inicialitzem color a Lambert
    vec3 colRes = Lambert (NormSCO, L);

    // Calculem R i V
    if (dot(NormSCO,L) < 0)
      return colRes;  // no hi ha component especular

    vec3 R = reflect(-L, NormSCO); // equival a: normalize (2.0*dot(NormSCO,L)*NormSCO - L);
    vec3 V = normalize(-vertSCO.xyz);

    if ((dot(R, V) < 0) || (matshin_fs == 0))
      return colRes;  // no hi ha component especular
    
    // Afegim la component especular
    float shine = pow(max(0.0, dot(R, V)), matshin_fs);
    return (colRes + matspec_fs * colFocus * shine); 
}

void main()
{	
    vec4 focusSCO = vec4(posFocus, 1.0);
    vec4 L = focusSCO - vertexSCO;
    vec3 Lxyz = normalize(L.xyz);
    //fcolor = Lambert(NormSCO, Lxyz);
    vec3 fcolor = Phong(NormSCO,Lxyz,vertexSCO);
	FragColor = vec4(fcolor,1);	
}
