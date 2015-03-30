#ifndef Earth_h
#define Earth_h

#include <G3D/G3DAll.h>

class Earth {
public:
  Earth(shared_ptr<Camera> camera, shared_ptr<Light> light);
  
  // Given latitude and longitude, calculate 3D position
  Vector3 getPosition(double latitude, double longitude);
  
  // Calculate and set up the buffers to render to screen
  void setupGeometry();

  // Creates a shader
  void setupShader();

  // Set the shader uniforms
  void configureShaderArgs(RenderDevice* rd);
  
  // Draw the Earth to screen
  void draw(RenderDevice *rd);

protected:
  
  // Stores the earth texture data
  shared_ptr<Texture> tex;

  // Shader to perform lighting calculations
  shared_ptr<Shader> shader;

  // Default GApp Camera
  shared_ptr<Camera> defaultCamera;

  // Directional light in the scene
  shared_ptr<Light> light;

  // VertexBuffer for storing positions, texture coordinates, and normals
  shared_ptr<VertexBuffer> vbuffer;
  
  // Per-vertex 3D position data
  AttributeArray gpuVerts;

  // Per-vertex 3D normals data
  AttributeArray gpuNorms;
    
  // Per-vertex s,t-texture coordinate data
  AttributeArray gpuTexCoords;

  // Index data for sending vertices
  IndexStream  gpuIndices;
 
};


#endif
  