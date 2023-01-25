#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <cmath>
#include "structs.h"
using namespace parser;
using namespace std;

typedef unsigned char RGB[3];
Vec3f pixel(Ray & ray, Scene &scene,Intersect & inter, Camera & cam, int depth);

Ray generateRay( Camera &camera, int y, int x)
{
    float left,right,top,bottom;
    left = camera.near_plane.x;
    right = camera.near_plane.y;
    bottom = camera.near_plane.z;
    top = camera.near_plane.w;

    Vec3f gaze = Normalize(camera.gaze); // -w
    Vec3f u, v;
    u = CrossP(gaze, camera.up);
    u = Normalize(u);

    v = CrossP(u, gaze);
    // -gaze x up = u

    //Vec3f v = CrossP(u, gaze);
    // m = e + (-w*distance)
    
    Vec3f m;
    m.x = camera.position.x + (gaze.x * camera.near_distance);
    m.y = camera.position.y + (gaze.y * camera.near_distance);
    m.z = camera.position.z + (gaze.z * camera.near_distance);

    Vec3f q;
    // q = m + l*u + t*v
    q.x = m.x + (u.x*left) + (v.x*top);
    q.y = m.y + (u.y*left) + (v.y*top);
    q.z = m.z + (u.z*left) + (v.z*top);

    float su_x = (right - left)*(x + 0.5)/camera.image_width;
    float sv_y = (top - bottom)*(y + 0.5)/camera.image_height;

    Vec3f s;
    // s = q + su_x * u - sv_y * v
    s.x = q.x + (u.x*su_x) - (v.x * sv_y);
    s.y = q.y + (u.y*su_x) - (v.y * sv_y);
    s.z = q.z + (u.z*su_x) - (v.z * sv_y);

    Ray ray;
    //r(t) = e + (s – e)t
    ray.e = camera.position; // will need in future calculations
    ray.r_t = Sub(s, camera.position);
    ray.r_t = Normalize(ray.r_t);
    ray.shadow = false;

    return ray;
}

Intersect sph_intersect(Ray &ray, Sphere &sphere, Scene & scene, int obje_id){
    Intersect inter;


    Vec3f center = scene.vertex_data[sphere.center_vertex_id-1];


    // - d (o-c) +- sqrt((d(o-c))^2 - d^2((o-c)^2 -R^2))/2*d^2 --> discriminant
    //(d.d)t^2 +2d.(o-c)t + (o-c)^2 -R^2 =0 sphere
    Vec3f temp = Sub(ray.e, center);
    float R_2 = pow( sphere.radius,2);

    float A = DotP(ray.r_t, ray.r_t);
    float B = 2 * DotP(ray.r_t, temp);
    float C = DotP(temp, temp) - R_2;

    //if B^2 -4AC < 0 no intersection
    float disc = pow(B,2) - (4*A*C);

    if( disc < 0 ){
        // no hit
        inter.intersect = false;
    }
    else {
        // find the roots of the equation
		float res1 = (-1 * B + sqrtf(disc))/2*A;
		float res2 = (-1 * B - sqrtf(disc))/2*A;

        inter.t = fmin(res1, res2);
        inter.intersect = true;
        inter.obje_id = obje_id;
        inter.material_id = sphere.material_id;
        inter.obje = SPHERE ;

        float radius = sphere.radius;
        inter.Point = int_P( ray, inter.t );    
        inter.normal = Sub(inter.Point, center);
		inter.normal.x /= radius;
		inter.normal.y /= radius;
		inter.normal.z /= radius;
    }
    return inter;
}


Intersect tri_intersect(Ray &ray, Triangle &triangle, Scene &scene, int obje_id){

    Intersect inter;
    Vec3f a = scene.vertex_data[triangle.indices.v0_id - 1];
    Vec3f b = scene.vertex_data[triangle.indices.v1_id - 1];
    Vec3f c = scene.vertex_data[triangle.indices.v2_id - 1];


    //////////// will use cramer's rule

    Vec3f a_o , a_c , d , a_b;
    a_o = Sub( a , ray.e); // a-o
    a_c = Sub( a , c); // a-c
    d = ray.r_t; // d
    a_b = Sub( a , b); // a-b

    float beta =  Det(a_o, a_c, d);
    float gamma = Det(a_b, a_o, d);
    float t = Det(a_b, a_c, a_o);
    float a_det = Det(a_b, a_c, d);

    beta = beta / a_det;
    gamma = gamma / a_det;
    t = t / a_det;

    /////check constraints
    
    if( beta + gamma <=1 && beta >= 0 && gamma >= 0 && t > 0 && a_det != 0){
        inter.intersect = true;
        inter.material_id = triangle.material_id ;
        inter.obje = TRIANGLE;
        inter.obje_id = obje_id;
        inter.Point = int_P(ray, t);
        inter.t = t;

        inter.normal = CrossP(Sub(b, a), Sub(c, a));
	    inter.normal = Normalize(inter.normal);
    }
    else {
        inter.intersect= false;
    }
    
    return inter;
}

Intersect final_inter( vector<Intersect> &all_inter){

    Intersect inter;
    int vector_size = all_inter.size();

    //no intersection
    if (vector_size == 0)
    {
        inter.intersect =false;
    }
    //intersection exists
    else{
        inter = all_inter[0];
        for (int i = 1; i < vector_size; i++)
        {
            if ( all_inter[i].t <= inter.t )
            {
               inter = all_inter[i];
            }
        }
        
    }
    return inter;
}

Intersect m_intersect (Ray &ray, const Mesh & mesh, Scene & scene, int obje_id){

    vector<Intersect> inter_vec;

    Intersect inter;
    Triangle tri;

    ///// have to check for every face of the mesh
    int f_count = mesh.faces.size();
    for (int i = 0; i < f_count ; i++)
    {
        
        tri.indices = mesh.faces[i];
        Vec3f a = scene.vertex_data[mesh.faces[i].v0_id - 1];		// find vertices of each triangle(face)
	 	Vec3f b = scene.vertex_data[mesh.faces[i].v1_id - 1];
	 	Vec3f c = scene.vertex_data[mesh.faces[i].v2_id - 1];
        /// check intersection with face
        inter = tri_intersect(ray, tri, scene, obje_id);

        if(inter.intersect ==true ){

            
            inter.material_id = mesh.material_id ;
            inter.obje = MESH;
            inter.obje_id = obje_id;

            /*
            Vec3f temp1= Sub(b, a);
            Vec3f temp2= Sub(c, a);
            inter.normal = CrossP(temp1, temp2);
            inter.normal = Normalize(inter.normal);*/

            inter_vec.push_back(inter);
        }
    }
    inter = final_inter(inter_vec);
    //retrun all faces that intersects with the ray
    return inter;

}


Vec3f irradiance( PointLight &p_light,  Vec3f &inter_p)
{
    Vec3f res;
    Vec3f d = Sub(p_light.position, inter_p);
    float d_d = DotP(d, d);

    //E(d) = I / d^2 --> E irradiance , I intensity, d distance

    if(d_d != 0.0)
    {
	    res.x = p_light.intensity.x/d_d;
	    res.y = p_light.intensity.y/d_d;
	    res.z = p_light.intensity.z/d_d;
    }
    return res;
}

Vec3f diffuse( PointLight &Light,  Scene &scene, Intersect & inter)
{
	Vec3f res;

	Vec3f irrad = irradiance(Light, inter.Point);
    
    // diffuse = diffuse reflectance coefficient *  cos(teta)' * irradiance
    // cos()' = max(0 , w_i . n) -->> w_i intersection point to  lighsource
    // and  n is normal

	Vec3f cos_1 = Sub(Light.position, inter.Point); // cos
	cos_1 = Normalize(cos_1);

	float dot = DotP(cos_1, inter.normal);

	dot = fmax(0.0 , dot);

	res.x = scene.materials[inter.material_id - 1].diffuse.x * dot * irrad.x;
	res.y = scene.materials[inter.material_id - 1].diffuse.y * dot * irrad.y;
	res.z = scene.materials[inter.material_id - 1].diffuse.z * dot * irrad.z;

	return res;
}


Vec3f specular( PointLight &light,  Scene &scene,  Ray &ray, Intersect & inter){
	
    Vec3f res;

    // a little bit similar to diffuse
    //  res = specular reflectance coefficient *  cos()'^(phong exp) * irradiance
    // cos =  max (0 , n.h)  -->> n is normal
    // h = w_i + w_0 / |w_i + w_0| -->> w _i intersection point to  lighsource 
    // w_0 intersection point to camera --> - ray 

	Material mat = scene.materials[inter.material_id - 1];

	Vec3f irrad = irradiance(light, inter.Point);

	Vec3f w_i = Sub(light.position, inter.Point);
    w_i = Normalize(w_i);

    // if angle is bigger than 90 light is coming from behind the surface
    // normalized before in the ray function
    /*
    Vec3f w_0 = Negate(ray.r_t); 
    float angle = (acos(DotP(w_i, w_0)) * 180 / 3.14);

    if(angle > 90){
        res.x=0;
        res.y=0;
        res.z=0;
        return res;
    }
    */
	Vec3f h = Sub(w_i, ray.r_t); // w_i + w_0 -->> w_i + (-1)ray.r_t
	h = Normalize(h);

    //max (0 , n.h)
	float dot = DotP(inter.normal, h);
    dot = fmax(0, dot);
    
	res.x = mat.specular.x * pow(dot, mat.phong_exponent) * irrad.x;
	res.y = mat.specular.y * pow(dot, mat.phong_exponent) * irrad.y;
	res.z = mat.specular.z * pow(dot, mat.phong_exponent) * irrad.z;

	return res;
}


Intersect closest_hit(Ray &ray, Scene & scene){

    vector<Intersect> all_inter;

    int sph_count = scene.spheres.size();
    int tri_count = scene.triangles.size();
    int m_count = scene.meshes.size();

    Sphere sph;
    //find intersection for spheres
    for (int i = 0; i < sph_count; i++)
    {
        sph = scene.spheres[i];
        Intersect inter_sph = sph_intersect(ray, sph, scene, i);

        if (inter_sph.intersect == true)
        {
            all_inter.push_back(inter_sph);
        }
        
    }

    Triangle tri;
    //find intersetion for triangles
    for (int i = 0; i < tri_count; i++)
    {
        tri = scene.triangles[i];
        
        Intersect inter_tri = tri_intersect(ray, tri, scene, i);

        if (inter_tri.intersect == true)
        {
            all_inter.push_back(inter_tri);
        }
    }
    

    // find mesh triangle that intersects with ray
    for (int i = 0; i < m_count; i++)
    {
        Mesh mesh = scene.meshes[i];
        Intersect m_inter = m_intersect( ray, mesh, scene, i);

        //push back the triangle that on the mesh that intersects
            if(m_inter.intersect == true){

                all_inter.push_back(m_inter);
            }

    }
        
    //find  the closest intersection
    Intersect res = final_inter(all_inter);
    
    return res;
}

Ray shadowRay1( Ray & ray, Scene & scene , PointLight & light, Intersect & inter){

    Ray res;
    //s(t) = (intersection + n.epsilon) +  t * w_i

    Vec3f w_i = Sub(light.position, inter.Point);
    w_i = Normalize(w_i);

    Vec3f n_eps = Scale( inter.normal, scene.shadow_ray_epsilon); //n.epsilon
    
    res.e = Add(inter.Point, n_eps);
    res.r_t = w_i;
    res.shadow = true;
    
    return res;
}

Vec3f ambient( Scene &scene,  Intersect & inter){
    Vec3f res;
    return res = Multiply(scene.materials[inter.material_id-1].ambient, scene.ambient_light);
}

Vec3f pixel(Ray & ray, Scene &scene,  Intersect & inter, Camera & cam, int depth)
{

    Vec3f res_pix;
    bool s_exists = false;
    Vec3f pixelColor;
    int sph_count = scene.spheres.size();
    int tri_count = scene.triangles.size();
    int m_count = scene.meshes.size();
    int l_count = scene.point_lights.size();
    float pixel1 = 0;
    float pixel2 = 0;
    float pixel3 = 0;


    if(inter.intersect == true)
    {
        int material_id = inter.material_id;
        pixel1 = scene.materials[material_id - 1].ambient.x * scene.ambient_light.x;
        pixel2 = scene.materials[material_id - 1].ambient.y * scene.ambient_light.y;
        pixel3 = scene.materials[material_id - 1].ambient.z * scene.ambient_light.z;

		for(int light = 0; light < l_count; light++)
		{
            PointLight p_light = scene.point_lights[light];
            bool s_exists = false;
            Ray shadow = shadowRay1(ray, scene, p_light, inter);
            float lightToCam = Distance(p_light.position, cam.position);

            float x_dist = Sub(p_light.position, shadow.e).x / shadow.r_t.x;
            
            Intersect shadow_inter;
			vector<Intersect> shadowInterss;

            // Does shadow ray intersects with any sphere
            for(int sph_num = 0; sph_num < sph_count; sph_num++)
        	{
          		Sphere sphere = scene.spheres[sph_num];

          		shadow_inter = sph_intersect( shadow, sphere, scene, sph_num);

          		if(shadow_inter.intersect)
          		{
            		if(x_dist > shadow_inter.t && shadow_inter.t >= 0)
            		{
              			s_exists =  true;
            		}
          		}
        	}

            //If there is a shadow no need to look at the remaining objects
            if(s_exists == false){

                //Does shadow ray intersects with any triangles
                for(int tri_num = 0; tri_num < tri_count; tri_num++)
                {
                    Triangle triangle = scene.triangles[tri_num];

                    shadow_inter = tri_intersect(shadow, triangle, scene, tri_num);

                    if(shadow_inter.intersect)
                    {
                        if(x_dist > shadow_inter.t && shadow_inter.t >= 0)
                        {
                            s_exists = true;
                        }
                    }
                }
            }
            //If there is a shadow no need to look at the remaining objects
            if(s_exists == false)
            {
                //Does shadow ray intersects with any mesh
                for(int m_num = 0; m_num < m_count; m_num++)
                {
                    Mesh mesh = scene.meshes[m_num];
                    Intersect  shadow_inter=  m_intersect( shadow, mesh, scene, m_num);


                    if(shadow_inter.intersect)
                    {
                        if(x_dist > shadow_inter.t && shadow_inter.t >= 0)
                        {
                            s_exists = true;
                        }
                    }
                }
            }
            
            
            // if s_exists true current light source doesn't contribute to this point's lightning
            // if s_exists is not true then calculate the diffuse and specular light at this point
            if (s_exists == false || (s_exists && lightToCam == 0))
            {
                int material_id = inter.material_id;

		        Vec3f dif = diffuse(p_light, scene, inter);

		        Vec3f spec = specular(p_light, scene, ray, inter);
		        
              	pixel1 += dif.x + spec.x;
  		        pixel2 += dif.y + spec.y;
  		        pixel3 += dif.z + spec.z;
            }
            
        }

        bool mirror = scene.materials[inter.material_id -1].is_mirror;
        Vec3f reflection;
        reflection.x = 0;
        reflection.y = 0;
        reflection.z = 0;
        if (mirror == true && depth > 0)
        {
        
            // -w_0 + 2 * n * (n . w_0)

        	float n_w_0 = -2 * DotP(ray.r_t, inter.normal); //2*( n . w_0)
            //Vec3f w_r = Scale(inter.normal, n_w_0); // -2 * n * (n . w_0)
        	Vec3f w_r;
        	w_r.x = inter.normal.x * n_w_0 + ray.r_t.x;
        	w_r.y = inter.normal.y * n_w_0 + ray.r_t.y;
        	w_r.z = inter.normal.z * n_w_0 + ray.r_t.z;
      		w_r= Normalize(w_r);
        	//w_r = Add(w_r, ray.r_t); //// -w_0 + 2 * n * (n . w_0)
            w_r = Negate(w_r);

            Vec3f n_eps;
        	n_eps.x = w_r.x * scene.shadow_ray_epsilon;
        	n_eps.y = w_r.y * scene.shadow_ray_epsilon;
        	n_eps.z = w_r.z * scene.shadow_ray_epsilon;
             // reflection ray
            Ray m_ray;
            m_ray.e = Add(inter.Point, n_eps);
            m_ray.r_t = w_r;
            m_ray.shadow = false;

            // what the reflection ray intersects
            Intersect reflect_ray_inter = closest_hit(m_ray, scene);

            // light recursicely mirrors if it came accross a mirror again
            if( reflect_ray_inter.intersect && !(reflect_ray_inter.obje_id == inter.obje_id && reflect_ray_inter.obje == inter.obje))
            {
                reflection = pixel(m_ray,  scene, reflect_ray_inter, cam,  depth-1);
            }

            pixel1 += reflection.x * scene.materials[material_id - 1].mirror.x;
      		pixel2 += reflection.y * scene.materials[material_id - 1].mirror.y;
      		pixel3 += reflection.z * scene.materials[material_id - 1].mirror.z;

        }
  	}
  	else         // if hitHappened == 0
 	{
      	pixel1 = scene.background_color.x;
      	pixel1 = scene.background_color.y;
      	pixel1 = scene.background_color.z;
  	}

    pixelColor.x = pixel1;
  	pixelColor.y = pixel2;
  	pixelColor.z = pixel3;

      	return pixelColor;
}



int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

       int cameras = scene.cameras.size();
    
    for (int c = 0; c < cameras; c++) // for every camera we will generate a scene
    {
        Camera cam = scene.cameras[c];
        int width = cam.image_width , height =cam.image_height ;
        unsigned char* image = new unsigned char [width * height * 3];
        int i = 0;

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
            	/*********GENERATE RAYS*********/
                Ray ray = generateRay(cam, y, x);

                Intersect inter = closest_hit(ray, scene);


             	Vec3f pixelColor = pixel(ray, scene, inter, cam, (scene.max_recursion_depth));

            	if(pixelColor.x > 255)
            		image[i] = 255;
              	else
                	image[i] = round(pixelColor.x);

            	if(pixelColor.y > 255)
            		image[i + 1] = 255;
             	else
                	image[i + 1] = round(pixelColor.y);

            	if(pixelColor.z > 255)
            		image[i + 2] = 255;
              	else
                	image[i + 2] = round(pixelColor.z);


              	i += 3;

            }
        }

        write_ppm(cam.image_name.c_str(), image, width, height);
    }
}