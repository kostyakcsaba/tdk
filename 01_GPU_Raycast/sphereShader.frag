#version 330

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec2 vs_out_tex;
// kimenõ érték - a fragment színe
out vec4 fs_out_col;

//
// uniform változók
//
uniform mat4 viewProj;
uniform mat4 projI;
uniform mat4 viewI;
uniform mat4 modelI;
uniform mat4 model;
uniform mat4 view;
uniform vec3 time;
uniform float zNear = 1;
uniform float zFar = 1000;

uniform vec3 center = vec3(0,0,0);
uniform float r = 1;

uniform vec3 lightPos = vec3(0,20,0);

uniform sampler3D text;

vec3 tpmap(vec3 p){


   return vec3(1,0,1);
 }

  float saturate(float p) {
    return (min(max(p,0.),1.));
  }
  float DE(vec3 p)
  {
  return (texture3D(text,p).r);
  }
 int MaxRaySteps=100;
float MinimumDistance=0.008;
  int iterations=34;


  float DEN(vec3 p0, vec3 p1)
{
	float x;
	x=DE(p0);
	x=x-DE(p1);
	return x;
}

vec3 calcNormal( vec3 p) 
{
	float seg1;
	float seg2;
	float eps=1.0/10.0;

	vec3 nor=vec3(0.0,0.0,0.0);

	vec3 p0= vec3(p.x+eps,p.y,p.z);
	vec3 p1= vec3(p.x-eps,p.y,p.z);
	nor.x=DEN(p0,p1);
	
	p0=vec3(p.x,p.y+eps,p.z);
	p1=vec3(p.x,p.y-eps,p.z);
	nor.y = DEN(p0,p1);
	
	p0=vec3(p.x,p.y,p.z+eps);
	p1=vec3(p.x,p.y,p.z-eps);
	nor.z = DEN(p0,p1);

	normalize(nor);

	return nor;
}

//
//raymarching-et megvalosito fuggveny
//

float trace(vec3 from, vec3 direction) 
{
	float totalDistance = 0.0;
	int steps;
	float ret=0.0;
	vec3 nor=vec3(0.0,0.0,0.0);

	for (steps=0; steps < 200; steps++) 
	{
		vec3 p = from + totalDistance * direction;
		float distance = DE(p);
		totalDistance += distance;

		if (distance< 0.0009) 
		{
			ret=totalDistance;
			break;
		}

		

	}
	return ret;
}
void getRay(in vec3 inVec, out vec3 rayOrig, out vec3 rayDir)
{
	vec4 nearPt = viewI * projI * vec4(inVec.xy, -1, 1);
	vec4 farPt  = viewI * projI * vec4(inVec.xy, 1, 1);

	rayOrig = nearPt.xyz/nearPt.w;

	vec3 rayEnd = farPt.xyz/farPt.w;
	rayDir  = normalize( rayEnd - rayOrig  );
}

void main()
{
	vec3 rayOrig, rayDir;

	getRay(vs_out_pos, rayOrig, rayDir);

	rayOrig = (modelI * vec4(rayOrig, 1) ).xyz;
	rayDir  = (modelI * vec4(rayDir,  0) ).xyz;
	
	
	float t = trace(rayOrig,rayDir);
	vec3 intersectionPoint = rayOrig + t*rayDir;
	vec3 surfaceNormal = calcNormal(intersectionPoint);

	intersectionPoint = (model * vec4(intersectionPoint, 1) ).xyz;
	surfaceNormal = normalize( ( modelI * vec4(surfaceNormal, 0) ).xyz);
	if(surfaceNormal==vec3(0,0,0))
	{
	surfaceNormal=vec3(0,1,0);
	}
	vec4 tex=vec4((tpmap(intersectionPoint).xyz),1);

	// egyszeru diffuz szin
	vec3 toLight = normalize(lightPos - intersectionPoint);
	vec4 diffuseColor = vec4(clamp( dot(surfaceNormal, toLight), 0, 1 ));
	
	vec3 s=mix(vec3(0.1,0.1,0.8),vec3(0.8,0.1,0.1),(surfaceNormal.y+1.0)/2.0);

	fs_out_col = ((diffuseColor*vec4(0.9))-(vec4(s/2.0,1)))* tex;
	t=1.0/t;
	//fs_out_col =vec4(t,t,t,1);
	// viewport transzformáció: http://www.songho.ca/opengl/gl_transform.html 
	// gl_DepthRange: http://www.opengl.org/registry/doc/GLSLangSpec.4.30.6.pdf , 130.o. 
	vec4 clipPos = viewProj * vec4( intersectionPoint, 1 );

	float zndc = clipPos.z / clipPos.w; 

	float n = gl_DepthRange.near-2.0;
	float f = gl_DepthRange.far;

	gl_FragDepth = (f-n)/2 * zndc + (f+n)/2;
}