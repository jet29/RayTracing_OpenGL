#version 330 core
// Vertex color (interpolated/fragment)
in vec3 screenPos;
in mat4 ivp;

// Uniforms 
uniform vec3 eye;
uniform vec2 windowSize;
uniform int rebounds;

// Sphere
struct Sphere{
	vec3 normal;
	vec3 center;
	vec3 diffuse;
	vec3 specular;
	float radius;
	float rx_intensity;
	float rf_intensity;
};

// Plane
struct Plane{
	vec3 n;
	vec3 p0;
	vec3 diffuse;
	vec3 specular;
	float rx_intensity;
	float rf_intensity;
};

struct Intersect{
	vec3 pos;
	vec3 normal;
	vec3 diffuse;
	vec3 specular;
	float t;
	float rx_intensity;
	float rf_intensity;
};

// Models
Plane plane[6];
Sphere sphere[2];

// Fragment Color
out vec4 fragColor;

// Create scene function
void createScene(){
	// Init planes:
		// Normal - p0 - diffuse - spec - rx intensity - rf intensity
		// Left-side wall
		plane[0] = Plane(vec3(1.0f,0.0f,0.0f),vec3(-10.0f,0.0f,0.0f),vec3(1.0f,1.0f,1.0f),vec3(0.0f,1.0f,1.0f),0.0f, 0.0f);
		// Right-side wall
		plane[1] = Plane(vec3(-1.0f,0.0f,0.0f),vec3(10.0f,0.0f,0.0f),vec3(1.0f,1.0f,1.0f),vec3(0.0f,1.0f,1.0f),0.0f, 0.0f);
		// front-side wall
		plane[2] = Plane(vec3(0.0f,0.0f,1.0f),vec3(0.0f,0.0f,-30.0f),vec3(0.0f,0.0f,1.0f),vec3(0.0f,1.0f,1.0f),0.5f, 0.0f);
		// Top-side wall
		plane[3] = Plane(vec3(0.0f,-1.0f,0.0f),vec3(0.0f,7.0f,0.0f),vec3(1.0f,0.0f,0.0f),vec3(0.0f,1.0f,1.0f),0.0f, 0.0f);
		// Bottom-side wall
		plane[4] = Plane(vec3(0.0f,1.0f,0.0f),vec3(0.0f,-7.0f,0.0f),vec3(1.0f,0.0f,1.0f),vec3(0.0f,1.0f,1.0f),0.0f, 0.0f);
		// Back-side wall
		plane[5] = Plane(vec3(0.0f,0.0f,-1.0f),vec3(0.0f,0.0f,2.0f),vec3(0.0f,1.0f,0.0f),vec3(0.0f,1.0f,1.0f),0.0f, 0.0f);
	// Init spheres:
		// Normal - center - diffuse - specular - radius
		sphere[0] = Sphere(vec3(0.0f,0.0f,0.0f),vec3(-2.5f,0,-25.0f), vec3(1.0f,0.0f,0.0f),vec3(1.0f,0.0f,0.0f),1.5f,1.0f, 0.0f);
		sphere[1] = Sphere(vec3(0.0f,0.0f,0.0f),vec3(2.5f,0,-25.0f), vec3(1.0f,1.0f,1.0f),vec3(1.0f,0.0f,0.0f),1.0f,0.0f, 1.0f);
}


// Sc : Sphere center / Rp: Ray point / Rd: Ray direction
Intersect intersectSphere(vec3 Rp, vec3 Rd, Sphere sphere){
	// Sphere radius
	float X0XC = Rp.x - sphere.center.x;
	float Y0YC = Rp.y - sphere.center.y;
	float Z0ZC = Rp.z - sphere.center.z;
	// Compute B
	float B = 2 * (Rd.x * X0XC + Rd.y * Y0YC  + Rd.z * Z0ZC);
	// Compute C
	float C = X0XC*X0XC + Y0YC*Y0YC + Z0ZC*Z0ZC - sphere.radius*sphere.radius;
	// Determine if there's an intersection by using determinant
	float det = B*B - 4*C;
	if (det < 0.0f)
		return Intersect(vec3(0.0f),vec3(0.0f),vec3(1.0f),vec3(0.0f),-1.0f,0.0f,0.0f);
	// Compute t
	float t = (-B - pow(det,0.5f)) / 2;
	// Intersection point
	vec3 Ri = Rp + Rd*t;
	// Compute sphere normal
	vec3 normal = normalize(Ri - sphere.center);
	sphere.normal = normal;
	return Intersect(Ri,sphere.normal,sphere.diffuse,sphere.specular,t,sphere.rx_intensity,sphere.rf_intensity);
}

Intersect intersectPlane(vec3 Rp, vec3 Rd, Plane plane){
	// P0: plane's point
	// n : plane's normal
	// Determine if given point p is on plane: (p-p0) . n = 0
	float t = dot((plane.p0 - Rp),plane.n) / dot(Rd,plane.n);
	// There's an intersection with the plane if t >= 0
	if (t < 0.0f)
		return Intersect(vec3(0.0f),vec3(0.0f),vec3(1.0f),vec3(0.0f),-1.0f,0.0f,0.0f);
	vec3 Ri = Rp + Rd*t;
	return Intersect(Ri,plane.n,plane.diffuse,plane.specular,t,plane.rx_intensity,plane.rf_intensity);
}

Intersect castRay(vec3 Rp, vec3 Rd){
	// Compare against all planes
	Intersect t0 = intersectPlane(Rp,Rd,plane[0]);
	Intersect t1 = intersectPlane(Rp,Rd,plane[1]);
	Intersect t2 = intersectPlane(Rp,Rd,plane[2]);
	Intersect t3 = intersectPlane(Rp,Rd,plane[3]);
	Intersect t4 = intersectPlane(Rp,Rd,plane[4]);
	Intersect t5 = intersectPlane(Rp,Rd,plane[5]);
	// Compare against all spheres
	Intersect t6 = intersectSphere(Rp,Rd,sphere[0]);
	Intersect t7 = intersectSphere(Rp,Rd,sphere[1]);


	t0 = (t0.t < 0.0f) || (t1.t > 0.0f && t1.t < t0.t) ? t1 : t0;
	t0 = (t0.t < 0.0f) || (t2.t > 0.0f && t2.t < t0.t) ? t2 : t0;
	t0 = (t0.t < 0.0f) || (t3.t > 0.0f && t3.t < t0.t) ? t3 : t0;
	t0 = (t0.t < 0.0f) || (t4.t > 0.0f && t4.t < t0.t) ? t4 : t0;
	t0 = (t0.t < 0.0f) || (t5.t > 0.0f && t5.t < t0.t) ? t5 : t0;
	t0 = (t0.t < 0.0f) || (t6.t > 0.0f && t6.t < t0.t) ? t6 : t0;	
	t0 = (t0.t < 0.0f) || (t7.t > 0.0f && t7.t < t0.t) ? t7 : t0;

	return t0;
}


Intersect castRayRf(vec3 Rp, vec3 Rd){

	// Compare against all planes
	Intersect t0 = intersectPlane(Rp,Rd,plane[0]);
	Intersect t1 = intersectPlane(Rp,Rd,plane[1]);
	Intersect t2 = intersectPlane(Rp,Rd,plane[2]);
	Intersect t3 = intersectPlane(Rp,Rd,plane[3]);
	Intersect t4 = intersectPlane(Rp,Rd,plane[4]);
	Intersect t5 = intersectPlane(Rp,Rd,plane[5]);
	// Compare against all spheres
	Intersect t6 = intersectSphere(Rp,Rd,sphere[0]);
	//Intersect t7 = intersectSphere(Rp,Rd,sphere[1]);


	t0 = (t0.t < 0.0f) || (t1.t > 0.0f && t1.t < t0.t) ? t1 : t0;
	t0 = (t0.t < 0.0f) || (t2.t > 0.0f && t2.t < t0.t) ? t2 : t0;
	t0 = (t0.t < 0.0f) || (t3.t > 0.0f && t3.t < t0.t) ? t3 : t0;
	t0 = (t0.t < 0.0f) || (t4.t > 0.0f && t4.t < t0.t) ? t4 : t0;
	t0 = (t0.t < 0.0f) || (t5.t > 0.0f && t5.t < t0.t) ? t5 : t0;
	t0 = (t0.t < 0.0f) || (t6.t > 0.0f && t6.t < t0.t) ? t6 : t0;	
	//t0 = (t0.t < 0.0f) || (t7.t > 0.0f && t7.t < t0.t) ? t7 : t0;

	return t0;
}

/**
 * Compute fragment occlusion
 * @param{Rp} Ray position
 * @param{Rd} Ray direction
 * @param{Rl} Ray length
 * */
float castShadowRay(vec3 Rp, vec3 Rd, float Rl){
	Intersect t0;
	vec3 sRp, sRd;
	float sRl;
	for (int i=0;i<2;i++){
		t0 = intersectSphere(Rp,Rd,sphere[i]);
		if (t0.t < 0.0f) continue;
		// Compute ray_origin-shadow_intersect ray
		sRp = t0.pos;
		sRd = sRp - Rp;
		sRl = length(sRd);
		if ( sRl < Rl ) 
			return 0.1f;
	}
	return 1.0f;
}

float lambert(vec3 normal, vec3 lightDir){
	return clamp(dot(normal,lightDir),0.0f,1.0f);
}

float blinn_phong(vec3 lightDir, vec3 rayDir, vec3 normal){
	vec3 halfwayDir = normalize(lightDir+rayDir);
	return pow(clamp(dot(normal,halfwayDir),0.0f,1.0f),2.0f);
}

void computeLight(vec3 lightPos, vec3 lightDir, vec3 rayDir, Intersect vertex, out vec3 diffuse, out vec3 specular){
	// Compute attenuation
	float distance    = length(lightPos - vertex.pos);
	float attenuation = 1.0 / (0.3f + 0.13f * distance + 0.001f * (distance * distance)); 
	// Compute Lambert diffuse component
	float diff = lambert(vertex.normal,lightDir);

	// Compute Blinn-Phong specular component
	float spec = blinn_phong(lightDir,rayDir,vertex.normal);

	// Diffuse total contribution
	diffuse = vertex.diffuse * diff * attenuation;
		
	// Specular total contribution
	specular = vertex.specular * spec * attenuation;
}

void main(){
	// Background (ambient) color
	vec3 ambient = vec3(0.0f);
	// Ray direction
    vec3 rayDir = normalize(screenPos - eye);
	// Ray origin
	vec3 rayOrigin = eye;
	// Ray intensity
	float rayInt = 1.0f;
	// Point light
	vec3 lightPos = vec3(0.0f,5.0f,-25.0f);
	// Creating scene
	createScene();
	// Intersect vertex
	Intersect vertex, reflex, _refract;
	// Color source contribution vectors
	vec3 lights = vec3(0.0f);
	vec3 reflection = vec3(0.0f);
	vec3 refraction = vec3(0.0f);
	vec3 maskColor = vec3(1.0f);
	vec3 result = vec3(0.0f);

	// Rebounds loops
	for (int i=0;i<rebounds;i++){
		// Cast ray
		vertex = castRay(rayOrigin,rayDir);
		vertex.t = vertex.t < 0.0f ? 0.0f : 1.0f;

		// Get shadow ray
		vec3 sRayDir = lightPos-vertex.pos;
		float sRayLength = length(sRayDir);
		sRayDir = normalize(sRayDir);
		// Compute shadow of primary ray
		float shadow = castShadowRay(vertex.pos + vertex.normal*0.001f,sRayDir,sRayLength);
		
		// Compute light direction
		vec3 lightDir = normalize(lightPos - vertex.pos);
		
		// Diffuse and specular components of light
		vec3 diffuse, specular;
		
		// Compute light contribution of primary ray
		computeLight(lightPos,lightDir,rayDir,vertex,diffuse,specular);

		// Lights contribution of primary ray
		lights += diffuse + specular;
		
		// Shadows contribution of primary ray
		lights *= shadow * rayInt;

		// If this was invalid vertex, draw with background color, else draw light contribution color
		lights = mix(ambient,lights,vertex.t);

		// Get reflection ray
		vec3 rxRayDir = normalize(reflect(rayDir,vertex.normal));
		reflex = castRay(vertex.pos + vertex.normal*0.001f,rxRayDir); 
		reflex.t = reflex.t < 0.0f ? 0.0f : 1.0f;

		// Get shadow ray for reflection ray
		sRayDir = lightPos-reflex.pos;
		sRayLength = length(sRayDir);
		sRayDir = normalize(sRayDir);

		// Compute shadow of reflection ray
		shadow = castShadowRay(reflex.pos + reflex.normal*0.001f,sRayDir,sRayLength);

		// Compute light contribution of reflection ray
		computeLight(lightPos,lightDir,rxRayDir,reflex,diffuse,specular);

		reflection += diffuse + specular;
		reflection *= vertex.rx_intensity * shadow * rayInt;
		reflection = mix(ambient,reflection,reflex.t);

		// Get refraction ray
		vec3 rfRayDir = normalize(refract(rayDir,vertex.normal,1.0f/0.5f)); 
		_refract = castRayRf(vertex.pos + vertex.normal*0.001f,rfRayDir);
		_refract.t  = _refract.t < 0.0f ? 0.0f : 1.0f;

		// Get shadow ray for refraction ray
		sRayDir = lightPos-_refract.pos;
		sRayLength = length(sRayDir);
		sRayDir = normalize(sRayDir);

		// Compute shadow of refraction rayr
		shadow = castShadowRay(_refract.pos + _refract.normal*0.001f,sRayDir,sRayLength);

		// Compute light contribution of refraction ray
		computeLight(lightPos,lightDir,rfRayDir,_refract,diffuse,specular);

		refraction += _refract.diffuse + _refract.specular; 
		refraction *= vertex.rf_intensity * shadow * rayInt;
		refraction  = mix(ambient,refraction,_refract.t);

		// Update ray position
		rayOrigin = vertex.pos + vertex.normal*0.001f;
		rayDir    = reflect(rayDir,vertex.normal);
		//rayInt -= 0.3f;

		maskColor += vertex.diffuse*0.5f ;
		result += (lights + reflection + refraction) * maskColor;

		if(vertex.rx_intensity == 0.0f) break;
	}

	// Gamma correction
	result = result / (result + vec3(1.0));
	result = pow(result, vec3(1.0/2.2)); 

    fragColor = vec4(result,1.0f);

	//fragColor = vec4(vPos,1.0f);
}
