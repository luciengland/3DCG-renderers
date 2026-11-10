/// include image write library and enable it
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include "Vector3.hpp"
#include "triangle.hpp"
#include "file_util.h"
using namespace std;

/// set width/height of output image
const int width = 640;
const int height = 640;

/// array to store RGB values for every pixel
unsigned char pixels[width * height * 3];

/// store all triangles from OBJ file
vector<Triangle> triangles;

/// set default background color (light green)
Vector3 background_color = Vector3{207,222,174};

/// find distance to closest triangle intersected by a ray
/// returns large value if no intersection found
float trace_ray_distance(Vector3 origin, Vector3 direction)
/// origin: starting point of ray
/// direction: direction ray travels
{
  /// initialize closest intersection distance
  float closest_t = INFINITY;
  /// check ray against every triangle in scene
  for (const Triangle &triangle : triangles)
  {
    /// calculate two edge vectors of triangle
    Vector3 edge1 = triangle.vertex1 - triangle.vertex2;
    Vector3 edge2 = triangle.vertex3 - triangle.vertex2;
    /// calculate cross product of edges to get perpendicular vector
    Vector3 cross_product = edge2.cross(edge1);   /// "p"
    /// normalize perpendicular vector to get unit surface normal
    Vector3 surface_normal = cross_product.normalize(); /// "abc"
    /// calculate offset for triangle's plane equation (d in plane equation ax + by + cz + d = 0)
    float d_plane_offset = -surface_normal.dot(triangle.vertex1);    /// "d"
    /// calculate distance to intersection with triangle's ray (P = origin + direction * t)
    float t_intersection = -(surface_normal.dot(origin)+d_plane_offset) / surface_normal.dot(direction);   /// "t"

    /// skip if intersection is farther away or behind origin
    if(t_intersection > closest_t || t_intersection <= 0)
    {
      continue;
    }

    // cout << unit_normal << " " << plane_offset << " " << ray_distance << endl;

    /// calculate 3D coordinates of intersection point
    Vector3 intersection_point = direction * t_intersection + origin;
    /// calculate vectors from triangle vertex to collision point
    Vector3 point_to_v1 = intersection_point - triangle.vertex1;
    Vector3 point_to_v2 = intersection_point - triangle.vertex2;
    Vector3 point_to_v3 = intersection_point - triangle.vertex3;
    /// calculate total area of original triangle
    float total_triangle_area = cross_product.length() / 2;
    /// calculate areas of 3 smaller triangles formed by intersection point/original vertices
    float area_one = point_to_v2.cross(point_to_v3).length()/2;
    float area_two = point_to_v3.cross(point_to_v1).length()/2;
    float area_three = point_to_v1.cross(point_to_v2).length()/2;
    /// calculate each smaller triangle's proportion of total area (barycentric coordinates)
    float weight_one = area_one / total_triangle_area;
    float weight_two = area_two / total_triangle_area;
    float weight_three = area_three / total_triangle_area;

    /// sum proportions and compare against 1 to determine if inside triangle
    float weight_sum = weight_one + weight_two + weight_three;
    float weight_diff = abs(1-weight_sum);

    /// if sum != original triangle's area intersection point is outside triangle
    if (weight_diff > 0.00001)
    {
        /// intersection point outside triangle
    }
    else
    {
        /// intersection point is inside triangle
        /// update closest distance
        closest_t = t_intersection;
    }
  } /// end for loop

  /// return closest intersection distance found
  return closest_t;
}


/// trace ray thru scene & calculate its resulting color
/// bounces = max number of recursive reflection bounces 
Vector3 trace_ray(Vector3 origin, Vector3 direction, int bounces = 3)
{
  /// initialize ray color to background color
  Vector3 color = Vector3{background_color.x, background_color.y, background_color.z};

  /// stop tracing reflections when no bounces remain
  if (bounces <= 0)
    return background_color;
  
  /// initialize closest intersection distance
  float closest_t = INFINITY;

  /// check ray intersection against every triangle 
 for (const Triangle &triangle : triangles)
  {
    /// calculate two edge vectors of triangle
    Vector3 edge1 = triangle.vertex1 - triangle.vertex2;
    Vector3 edge2 = triangle.vertex3 - triangle.vertex2;
    /// calculate cross product of edges to get perpendicular vector
    Vector3 cross_product = edge2.cross(edge1);    /// "p"
    /// normalize perpendicular vector to get unit surface normal
    Vector3 surface_normal = cross_product.normalize();    /// "abc"
    /// calculate offset for triangle's plane equation (d in plane equation ax + by + cz + d = 0)
    float d_plane_offset = -surface_normal.dot(triangle.vertex1);    /// "d"
    /// calculate distance to intersection with triangle's ray (P = origin + direction * t)
    float t_intersection = -(surface_normal.dot(origin)+d_plane_offset) / surface_normal.dot(direction);   /// "t"

    /// skip if intersection is farther away or behind origin
    if(t_intersection > closest_t || t_intersection <= 0)
    {
      continue;
    }

    // cout << unit_normal << " " << plane_offset << " " << ray_distance << endl;

    /// calculate 3D coordinates of intersection point
    Vector3 intersection_point = direction * t_intersection + origin;
    /// calculate vectors from triangle vertex to collision point
    Vector3 point_to_v1 = intersection_point - triangle.vertex1;
    Vector3 point_to_v2 = intersection_point - triangle.vertex2;
    Vector3 point_to_v3 = intersection_point - triangle.vertex3;
    /// calculate total area of original triangle
    float total_triangle_area = cross_product.length() / 2;
    /// calculate areas of 3 smaller triangles formed by intersection point/original vertices
    float area_one = point_to_v2.cross(point_to_v3).length()/2;
    float area_two = point_to_v3.cross(point_to_v1).length()/2;
    float area_three = point_to_v1.cross(point_to_v2).length()/2;
    /// calculate each smaller triangle's proportion of total area (barycentric coordinates)
    float weight_one = area_one / total_triangle_area;
    float weight_two = area_two / total_triangle_area;
    float weight_three = area_three / total_triangle_area;
    /// sum proportions and compare against 1 to determine if inside triangle
    float weight_sum = weight_one + weight_two + weight_three;
    float weight_diff = abs(1-weight_sum);
    /// if sum != original triangle's area intersection point is outside triangle
    if (weight_diff > 0.00001)
    {
        /// intersection point outside triangle
    }
    else
    {
        /// intersection point is inside triangle
        /// update closest distance
        closest_t = t_intersection;
        /// set surface base color to white
        Vector3 base_color = Vector3{255,255,255};
        /// define direction of incoming light and normalize it
        Vector3 light_direction = Vector3{0.5,1,1}.normalize();
        /// calculate ambient lighting 
        Vector3 ambient = base_color * 0.1;

        /// calculate diffuse lighting
        float diffuse_strength = max(0.f, light_direction.dot(surface_normal));
        /// apply diffuse lighting intensity to base color
        Vector3 diffuse = base_color * diffuse_strength;

        /// cast shadow ray toward light source
        if (trace_ray_distance(intersection_point + light_direction * 0.00001, light_direction) != INFINITY)
        {
          /// if another triangle blocks the light, remove diffuse light
          diffuse = Vector3{0,0,0};
        }
        /// initialize reflected color to black
        Vector3 reflection_color = Vector3{0,0,0};

        if (true)    /// reflection always enabled
        {
          /// calculate reflected ray direction (R = D - 2(N dot D)N)
          Vector3 reflection_direction = direction - surface_normal * 2 * surface_normal.dot(direction);

          /// offset reflection away from surface
          Vector3 reflection_origin = intersection_point + reflection_direction * 0.0001f;

          /// recursively trace reflected ray, reduce remaining bounces, multipy reflected color
          reflection_color = trace_ray(reflection_origin, reflection_direction, bounces - 1) * 0.5;
          
          /// reduce diffuse and ambient lighting 
          diffuse *= 0.5;
          ambient *= 0.5;
        }
        /// combine ambient, diffuse, reflected colors
        Vector3 final_color = ambient + diffuse + reflection_color;
        /// store color
        color = final_color;
    }
  }   /// end for loop
  /// return ray color
  return color;
}

/// load obj file and return triangles
vector<Triangle> parse_obj(const string& path)
{
  /// store loaded triangles
  vector<Triangle> loaded_triangles;

  /// read all lines from OBJ file into vector of strings
  std::vector<std::string> lines = readLines(path);
  /// print number of lines read from OBJ file
  // cout << "Number of lines: " << lines.size() << endl;

  /// store vertex positions ("v")
  vector<Vector3> vertices;
  /// store vertex positions ("vt")
  vector<Vector3> texture_coords;
  /// store vertex positions ("vn")
  vector<Vector3> vertex_normals;

  /// loop thru each line of OBJ file
  for (int i=0; i < lines.size(); i++)
  {
    /// get current line
    string line = lines[i];
    /// split line into individual strings using spaces
    vector<string> record = split(line, ' ');
    /// skip empty lines
    if (record.empty() || record[0].empty())
    {
      continue;
    }
    /// if start with "v"/defines a vertex
    if (record[0] == "v")
    {
      /// convert vertex coordinate strings to float
      float x = stof(record[1]);
      float y = stof(record[2]);
      float z = stof(record[3]);
      /// create Vector3 object using vertex coordinates
      Vector3 vertex = Vector3{x,y,z};
      /// add vertex to vector of vertices
      vertices.push_back(vertex);
      /// print vertex coordinates to verify
      // cout << "Vertex: " << x << ", " << y << ", " << z << endl;
      /// print original vertex line
      // cout << "Vertex: " << line << endl;
    }
    /// if start with "vt"/texture coordinate
    else if (record[0] == "vt")
    {
      /// convert texture coordinate to float
      float u = stof(record[1]);
      float v = stof(record[2]);
      /// create texture coordinate 
      Vector3 texture_coord = Vector3{u,v,0};
      /// store texture coordinate
      texture_coords.push_back(texture_coord);
    }
    /// if start with "vn"/vertex normal
    else if (record[0] == "vn")
    {
      /// convert normal coordinate to float
      float x = stof(record[1]);
      float y = stof(record[2]);
      float z = stof(record[3]);
      /// create vertex normal
      Vector3 vertex_normal = Vector3{x,y,z};
      /// store vertex normal
      vertex_normals.push_back(vertex_normal);
    }
    /// if start with "f"/defines a face
    else if (record[0] == "f")
    {
      /// store vertex position indices for current face
      vector<int> face_vertex_indices;
      /// store texture coordinate indices for current face
      vector<int> face_texture_indices;
      /// store vertex normal indices for current face
      vector<int> face_normal_indices;
      /// loop thru each vertex in face record (start index 1 bc index 0 is f)
      for (int j = 1; j < record.size(); j++)
      {
        /// split face into vertex/texture/normal
        vector<string> face_values = split(record[j], '/');
        /// convert indices to integer (- 1 bc OBJ file indices start 1 & vector indicies start 0)
        int vertex_index = stoi(face_values[0]) - 1;
        int texture_index = stoi(face_values[1]) - 1;
        int normal_index = stoi(face_values[2]) - 1;
        /// add index to corresponding face vector
        face_vertex_indices.push_back(vertex_index);
        face_texture_indices.push_back(texture_index);
        face_normal_indices.push_back(normal_index);
        // cout << "Vertex index: " << vertex_index << endl;   /// print vertex index
      }
      /// convert face into triangle
      for (int j = 1; j +1 < face_vertex_indices.size(); j++)
      {
        /// get vertex position using face vertex indices
        Vector3 v1 = vertices[face_vertex_indices[0]];
        Vector3 v2 = vertices[face_vertex_indices[j]];
        Vector3 v3 = vertices[face_vertex_indices[j + 1]];
        /// create triangle with vertices v1,v2,v3
        Triangle triangle = Triangle{v1, v2, v3};
        /// add triangle to triangle vector
        loaded_triangles.push_back(triangle);
      }
    }
  } 
  return loaded_triangles;
} /// end parse_obj()

/// cast camera rays and store rendered colors in pixel array
void render_image()
{
  /// define position, target view, up direction of camera
  Vector3 cam_origin = Vector3{2, 1, 3};
  Vector3 cam_lookat = Vector3{0, 0, 1}; 
  Vector3 cam_up = Vector3{0, 1, 0};
  
  /// calculate camera's orientation
  /// z-axis = | look_at - look_from |
  /// y-axis = look_up
  /// x-axis = z-axis cross y-axis
  Vector3 camera_forward = (cam_lookat - cam_origin).normalize();
  Vector3 camera_right = camera_forward.cross(cam_up).normalize();
  Vector3 camera_up = camera_right.cross(camera_forward).normalize();


  /// loop thru each row of pixels from top to bottom
  for (int y = 0; y < height; y++)
  {
    /// loop thru each pixel in current row from left to right
    for (int x = 0; x < width; x++)
    {
        /// calculate pixel's starting position in RGB array
        int pixel_index = y * width * 3 + x * 3;
        
        /// initialize pixel color to background color
        unsigned char r = background_color.x;
        unsigned char g = background_color.y;
        unsigned char b = background_color.z;

        // /// convert pixel coordinates into 3D coordinates (flip y-coord to match 3D coord system)
        // Vector3 pixel_target
        // {
        //     x/(float)width * 2-1, -(y/(float)height * 2-1), 0
        // };
        // /// calculate direction from camera to target point
        // Vector3 ray_direction = (pixel_target - cam_origin);

        /// map pixel coordinates onto camera plane
        float screen_x = x / (float)width * 2 - 1;
        float screen_y = -(y / (float)height * 2 - 1);

        /// calculate ray direction
        Vector3 ray_direction = camera_forward * 3.0f + camera_right * screen_x + camera_up * screen_y;

        /// normalize direction vector to unit length
        Vector3 normalized_ray = ray_direction.normalize();
        /// trace ray to determine pixel color
        Vector3 color = trace_ray(cam_origin, normalized_ray);
        /// extract RGB components
        r = color.x;
        g = color.y; 
        b = color.z;
        /// store RGB values in pixel array
        pixels[pixel_index] = r;
        pixels[pixel_index + 1] = g;
        pixels[pixel_index + 2] = b;
    }
  }
} /// end render_image()

/// ---------------------------------- MAIN -----------------------------------
int main()
{
  /// load triangles from OBJ file
  triangles = parse_obj("ex.obj");

  /// render into pixel array
  render_image();

  /// save completed pixel array as PNG image
  stbi_write_png("image.png", width, height, 3, pixels, width * 3);

  /// program end successfully
  return 0;
}  

// g++ rt.cpp Vector3.cpp -o rt

