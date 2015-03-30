#include "Earth.h"
#include "config.h"

using namespace std;

Earth::Earth(shared_ptr<Camera> camera, shared_ptr<Light> light) {
	defaultCamera = camera;
	this->light = light;

 	 // Note: TEXTURE_PATH is set in config.h
  	tex = Texture::fromFile(TEXTURE_PATH);
  	setupGeometry();
  	setupShader();

}

void Earth::setupGeometry() {
  // TODO: Write your own code here to tesselate and create texture coordinates for the Earth 
  // and then save the data to VertexBuffers, AttributeArrays, and an IndexStream.  The tesselation
  // should be based on the STACKS and SLICES parameters.

 	const int STACKS = 100;
	const int SLICES = 100;

	Array<Vector3> cpuVerts;
	Array<Vector3> cpuNorms;
	Array<Vector2> cpuTexCoords;
	Array<int> cpuIndices;

	//records the total number of vertices
	int i = 0;
	//records the number of stacks
	int k = 0;
	
	/*draws stacks of cans to form a sphere*/

	//latitude from -90 to 90 (in radians)
	for (double latitude = -pi()/2; latitude < pi()/2; latitude += pi()/STACKS) {

		//records the number of veritces in each STACK, resets for each STACK
		int j = 0;

		//longitude from -180 to 180 (in radians)
		for (double longitude = -pi(); longitude <= pi(); longitude += 2*pi()/SLICES) {

			//top circle
			Vector3 vertex_top = getPosition(latitude + pi()/STACKS, longitude);
			//bottom circle
			Vector3 vertex_bottom = getPosition(latitude, longitude);

			cpuVerts.append(vertex_top, vertex_bottom);

			//normals pointing outwards
			cpuNorms.append(vertex_top, vertex_bottom);

			//texCoord for top circle, from (0,0.9) to (0,0)
			Vector2 tex_top = Vector2((double)j/SLICES, 1.0 - (double)(k+1)/STACKS);
			//texCoord for bottom circle, from (0,1) to (0,0.1)
			Vector2 tex_bottom = Vector2((double)j/SLICES, 1.0 - (double)k/STACKS);

			cpuTexCoords.append(tex_top, tex_bottom);

			if (j > 0) {
				//012,234,456...
				cpuIndices.append((i-1)*2, i*2-1, i*2);
				//132,254,576...
				cpuIndices.append(i*2-1, i*2+1, i*2);
			}

			i++;
			j++;

		}

		k++;

	}

	//Allocate memory for Vertices, Normals, Indecies, Texture Coordinates for GPU storage
	vbuffer = VertexBuffer::create((sizeof(Vector3) + sizeof(Vector3)) * cpuVerts.size() +
		sizeof(int)*cpuIndices.size() + sizeof(Vector2)*cpuTexCoords.size());
	debugAssertGLOk();

	//store it in GPU
	gpuVerts = AttributeArray(cpuVerts, vbuffer);
	debugAssertGLOk();
	gpuNorms = AttributeArray(cpuNorms, vbuffer);
	debugAssertGLOk();
	gpuIndices = IndexStream(cpuIndices, vbuffer);
	debugAssertGLOk();
    gpuTexCoords = AttributeArray(cpuTexCoords, vbuffer);
    debugAssertGLOk();
}

//latitude and longitude in radians
Vector3 Earth::getPosition(double latitude, double longitude) {
  // TODO: Given a latitude and longitude as input, return the corresponding 3D x,y,z position 
  // on your Earth geometry
	double r = 1.0;

	double x = r * cos(longitude) * cos(latitude);
	double y = r * sin(latitude);
	double z = r * -sin(longitude) * cos(latitude);

	return Vector3(x,y,z);	

}

void Earth::setupShader() {
	G3D::String vertexShaderPath = DATA_DIR+"gouraud-shader.vrt";
	G3D::String fragmentShaderPath = DATA_DIR+"gouraud-shader.pix";

	debugAssert(FileSystem::exists(vertexShaderPath)); // If this asserts false, then your DATA_DIR is not set correctly in config.h
	debugAssert(FileSystem::exists(fragmentShaderPath));
	shader = Shader::fromFiles(vertexShaderPath, fragmentShaderPath);
}

void Earth::configureShaderArgs(RenderDevice* rd) {
	Args args;
	args.enableG3DArgs(true);
	args.setUniform("wsLight", light->position().xyz().direction());
	args.setUniform("lightColor", light->color);
	args.setUniform("wsEyePosition", defaultCamera->frame().translation);
	args.setUniform("textureSampler", tex, Sampler::video());
	
	//TODO: Set the attribute arrays and primitive types to draw the geometry
	//that you created in setupGeometry().

	args.setAttributeArray("vertex", gpuVerts);
	args.setAttributeArray("normal", gpuNorms);
	args.setAttributeArray("texCoord0", gpuTexCoords);

	args.setIndexStream(gpuIndices);

	//Set rendering method
	args.setPrimitiveType(PrimitiveType::TRIANGLES);

	rd->apply(shader, args);
}

void Earth::draw(RenderDevice *rd) {
  rd->pushState();
  rd->setCullFace(CullFace::NONE);

  configureShaderArgs(rd);

  rd->popState();
}


