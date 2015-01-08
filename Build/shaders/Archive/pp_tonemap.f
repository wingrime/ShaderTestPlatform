
vec4 HdrEncode(vec3 value)
{
value = value / 65536.0;
vec3 exponent = clamp(ceil(log2(value)), -128.0, 127.0);
float commonExponent = max(max(exponent.r, exponent.g), exponent.b);
float range = exp2(commonExponent);
vec3 mantissa = clamp(value / range, 0.0, 1.0);
return vec4(mantissa, (commonExponent + 128.0)/256.0);
}
vec3 HdrDecode(vec4 encoded)
{
float exponent = encoded.a * 256.0 - 128.0;
vec3 mantissa = encoded.rgb;
return exp2(exponent) * mantissa * 65536.0;
}




//Gamma correction lineral ->sRGB
float GC(float x) {
	if ( x <= 0.00031308)
		return 12.92*x;
	else
		return 1.055*pow(x,1/2.4) - 0.055;
}
//filmic tonemaping
float F_GC(float x) {
 //	float c =  max(0, x-0.004) ;
	//return pow((c*(6.2*c + 0.5)) / (c*(6.2*c+1.7)+0.06),1/2.2);
    return pow(x,1/2.2);
}