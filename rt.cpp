/// include image write library and enable it
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include "Vector3.hpp"
#include "triangle.hpp"
using namespace std;

int main()
{
  /// test vector addition and print result
  // Vector3 test_vector = Vector3{0, 0, 0};
  // test_vector += Vector3{1, 1, 1};
  // cout << test_vector << endl;

  /// set width/height of output image
  const int width = 640;
  const int height = 640;

  /// store RGB values for every pixel in 1D array
  unsigned char pixels[width * height * 3];

  /// define triangle using three vertices in 3D space
  Triangle triangle = Triangle{Vector3{-1,1,0}, Vector3{-1,-1,0}, Vector3{1,-1,0}};
  Triangle triangle2 = Triangle{Vector3{-1,1,.5}, Vector3{-1,-1,-.5}, Vector3{1,-1,.5}};

  vector<Triangle> triangles = {triangle};

  /// set default background color to purple
  Vector3 background_color = Vector3{150,0,225};

  /// loop thru each row of pixels from top to bottom
  for (int y = 0; y < height; y++)
  {
    /// loop thru each pixel in current row from left to right
    for (int x = 0; x < width; x++)
    {
        /// calculate pixel's starting position in RGB array
        int index = y * width * 3 + x * 3;
        
        /// initialize pixel with background color
        unsigned char r = background_color.x;
        unsigned char g = background_color.y;
        unsigned char b = background_color.z;

        /// define camera's position, target view, up direction
        Vector3 cam_origin = Vector3{0, 0, 1};
        Vector3 cam_lookat = Vector3{0, 0, 0};
        Vector3 cam_up = Vector3{0, 1, 0};

        /// convert pixel coordinates into 3D coordinates on image plane
        /// flip y-coordinate to match 3D coordinate system
        Vector3 target
        {
            x/(float)width * 2-1, -(y/(float)height * 2-1), 0
        };

        /// calculate direction from camera to target
        Vector3 ray_direction = (target - cam_origin);
        /// normalize ray direction to length of 1
        Vector3 normalized_ray = ray_direction.normalize();

        float closest_t = 1000000;
        for(const Triangle &triangle : triangles)
        {
          /// calculate two edges of triangle
          Vector3 edge_one = triangle.one - triangle.two;
          Vector3 edge_two = triangle.three - triangle.two;

          /// calculate normal vector perpendicular to triangle's plane
          Vector3 normal = edge_two.cross(edge_one);

          /// normalize vector to have length 1
          Vector3 unit_normal = normal.normalize();

          /// calculate offset for triangle's plane equation
          float plane_offset = -unit_normal.dot(triangle.one);

          /// calculate how far along the ray it intersects triangle's plane
          float ray_distance = -(unit_normal.dot(cam_origin)+plane_offset) / unit_normal.dot(normalized_ray);

          if(plane_offset > closest_t)
          {
            continue;
          }

          /// calculate 3D coordinates of intersection point
          Vector3 collision_point = normalized_ray * ray_distance + cam_origin;

          /// create vectors from each triangle vertex to intersection point
          Vector3 vector2point1 = collision_point - triangle.one;
          Vector3 vector2point2 = collision_point - triangle.two;
          Vector3 vector2point3 = collision_point - triangle.three;

          /// calculate total area of triangle
          float triangle_area = normal.length() / 2;

          /// calculate areas of the three smaller triangles
          /// formed by intersection point and original vertices
          float area_one = vector2point2.cross(vector2point3).length()/2;
          float area_two = vector2point3.cross(vector2point1).length()/2;
          float area_three = vector2point1.cross(vector2point2).length()/2;


          /// calculate barycentric coordinates using area ratios
          float weight_one = area_one / triangle_area;
          float weight_two = area_two / triangle_area;
          float weight_three = area_three / triangle_area;

          /// add barycentric weights to check if point inside triangle
          float weight_sum = weight_one + weight_two + weight_three;

          /// calculate difference from 1 to account for floating-point precision
          float weight_diff = abs(1-weight_sum);

          if (weight_diff > 0.00001)
          {
              /// intersection point outside triangle
              /// keep original background color
          }
          else
          {
              /// intersection point is inside triangle
              /// use barycentric coordinates to interpolate triangle's colors
              closest_t = plane_offset;
              r = 255 * weight_one + 0 * weight_two + 255 * weight_three;
              g = 255 * weight_one + 255 * weight_two + 0 * weight_three; 
              b = 0 * weight_one + 0 * weight_two + 0 * weight_three;
          }
        }

        /// store final RGB values in pixel array
        pixels[index] = r;
        pixels[index + 1] = g;
        pixels[index + 2] = b;

    }
  }

  /// save completed pixel array as PNG image
  stbi_write_png("image.png", width, height, 3, pixels, width * 3);
  return 0;

}
