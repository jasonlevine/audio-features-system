#version 150

uniform sampler2DRect tex0;

uniform vec4 fft;


in vec2 texCoordVarying;

out vec4 outputColor;

void main()
{
    //vec4 texel0;
    vec4 texel0 = texture(tex0, (texCoordVarying + (sin( texCoordVarying * fft.x * 0.05)) * 50.0  ) );
   // texel0 = texture( tex0, vec2( texCoordVarying.x , (sin(texCoordVarying.y) + 1.0) * 100.0));
    
//    texel0.y = texture(tex0, texCoordVarying.y);
//    texel0.z = texture(tex0, texCoordVarying.z);
//    texel0.w = texture(tex0, texCoordVarying.w);
//    

    //outputColor = vec4(texel0.rgb, texel0.a * texel1.a);
    float sin1 = (sin(fft.x/10.0) + 1.0) / 2.0;
    float sin2 = (sin(fft.y/10.0) + 1.0) / 2.0;
    float sin3 = (sin(fft.z/10.0) + 1.0) / 2.0;
    float sin4 = (sin(fft.w/25.0) + 1.0) / 2.0;

//    outputColor = vec4(texel0.r * sin1 + texel0.g * (1.0 - sin1),
//                       texel0.g * sin2 + texel0.b * (1.0 - sin2),
//                       texel0.b * sin3 + texel0.r * (1.0 - sin3),
//                       texel0.a * texel1.a);

    float scale = 10.0;
    outputColor = texel0;

}