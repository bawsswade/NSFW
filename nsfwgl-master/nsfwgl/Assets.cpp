#include "nsfw.h"
#include <stb\stb_image.h>

using namespace nsfw::ASSET;

const char *nsfw::TYPE_NAMES[eSIZE + 1] = { "NONE","vao","ibo","vbo","tri-size","fbo","rbo","texture","shader","SIZE" };

#ifdef _DEBUG
#define ASSET_LOG(type) do { std::cerr << "Trying to create key: <" << TYPE_NAMES[type] << ">" << name << std::endl; } while(0)
#else 
#define ASSET_LOG(type) do {} while (0)
#endif


nsfw::GL_HANDLE nsfw::Assets::getVERIFIED(const AssetKey &key) const
{
#ifdef _DEBUG
			if (!handles.count(key))
			{
				std::cerr << "***Asset Key not found: <" << TYPE_NAMES[key.first] << ">" << key.second << std::endl;
				return 0;
			}
#endif
			return handles.at(key);
}

bool nsfw::Assets::setINTERNAL(ASSET::GL_HANDLE_TYPE t, const char *name, GL_HANDLE handle)
{
	AssetKey key(t, name);
#ifdef _DEBUG
	if (handles.count(key))
	{
		std::cerr << "***Asset Key already exists: <" << TYPE_NAMES[key.first] << ">" << key.second << " ignoring." << std::endl;
		return false;
	}
	else std::cerr << "Asset Key successfully created: <" << TYPE_NAMES[key.first] << ">" << key.second << std::endl;
#endif
	handles[key] = handle;
	return true;
}

bool nsfw::Assets::makeVAO(const char * name, const struct Vertex *verts, unsigned vsize,  const unsigned * tris, unsigned tsize)
{
	ASSET_LOG(GL_HANDLE_TYPE::VBO);		// arrray on video card that stores vertices
	ASSET_LOG(GL_HANDLE_TYPE::IBO);		// array on the video card that stores whicvh vertices make up triangles
	ASSET_LOG(GL_HANDLE_TYPE::VAO);		// association between a VBO and IBO
	ASSET_LOG(GL_HANDLE_TYPE::SIZE);	// amount of tirangles does a VAO hold

	//get<SIZE>("MyMesh"); // number of triangles needed for glDRawElements
	//get<VAO>("MyMesh");  // vao for glDrawElements
	//TODO_D("Should generate VBO, IBO, VAO, and SIZE using the parameters, storing them in the 'handles' map.\nThis is where vertex attributes are set!");

	GL_HANDLE h_vao, h_vbo, h_ibo; // unsigned integer handles 

	// gen vbo, ibo, vao
	glGenVertexArrays(1, &h_vao);
	glGenBuffers(1, &h_ibo);
	glGenBuffers(1, &h_vbo);
	// bind buffers
	glBindVertexArray(h_vao);
	glBindBuffer(GL_ARRAY_BUFFER, h_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, h_ibo); 
	// allocate space in mem
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vsize, verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tsize, tris, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	// postion	: Spacial Location
	glEnableVertexAttribArray(1);	// normal
	glEnableVertexAttribArray(2);	// tangent
	glEnableVertexAttribArray(3);	// texCorrd

	// offsets
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::POSITION);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)Vertex::NORMAL);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)Vertex::TANGENT);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::TEXCOORD);


	// unbind vao, vbo, ibo
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	// attach to map
	setINTERNAL(VAO, name, h_vao);
	setINTERNAL(VBO, name, h_vbo);
	setINTERNAL(IBO, name, h_ibo);
	setINTERNAL(GL_HANDLE_TYPE::SIZE, name, tsize);

	GLenum err;
	if (err = glGetError() != GL_NO_ERROR)
	{
		std::cout << name << "'s VAO uncussessfully created\n";
		return false;
	}
	else return true;
}

bool nsfw::Assets::makeFBO(const char * name, unsigned w, unsigned h, unsigned nTextures, const char * names[], const unsigned depths[])
{
	//TODO_D("Create an FBO! Array parameters are for the render targets, which this function should also generate!\nuse makeTexture.\nNOTE THAT THERE IS NO FUNCTION SETUP FOR MAKING RENDER BUFFER OBJECTS.");
	ASSET_LOG(GL_HANDLE_TYPE::FBO);
	GL_HANDLE h_fbo;
	glGenFramebuffers(1, &h_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, h_fbo);
	
	std::vector<GLenum> drawBuffers;

	//	8 guaranteed color attachments (with 1 stencil, 1 depth)
	int n = 0;
	for (int i = 0; i < nTextures; i++)
	{
		makeTexture(names[i], w, h, depths[i]);

		unsigned Attachments;
		// differentiates between color, stencil, and depth
		if (depths[i] == GL_DEPTH_COMPONENT) Attachments = GL_DEPTH_ATTACHMENT;				// depth
		else if (depths[i] == GL_DEPTH_STENCIL) Attachments = GL_STENCIL_ATTACHMENT;		// stencil
		else Attachments = GL_COLOR_ATTACHMENT0 + n++;										// color

		glFramebufferTexture(GL_FRAMEBUFFER, Attachments, get(TEXTURE, names[i]), 0);
	}

	GLenum *colorAttachments = new GLenum[n];
	for (int i = 0; i < n; i++) 
		colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
	
	glDrawBuffers(n, colorAttachments);

	setINTERNAL(FBO, name, h_fbo);

	//error checking
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		bool incompleteAttachment = status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
		bool invalidEnum = status == GL_INVALID_ENUM;
		bool invalidValue = status == GL_INVALID_VALUE;
		std::cout << name << " FBO unsuccessfully made" << std::endl;
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool nsfw::Assets::makeTexture(const char * name, unsigned w, unsigned h, unsigned depth, const unsigned char *pixels)
{
	//TODO_D("Allocate a texture using the given space/dimensions. Should work if 'pixels' is null, so that you can use this same function with makeFBO\n note that Dept will use a GL value.");
	ASSET_LOG(GL_HANDLE_TYPE::TEXTURE);
	GL_HANDLE h_texture;

	glGenTextures(1, &h_texture);
	
	glBindTexture(GL_TEXTURE_2D, h_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, depth, w, h, 0, depth, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	//GLenum a_depth = (depth == GL_DEPTH_COMPONENT) ? GL_DEPTH_ATTACHMENT : depth;			//what does this do?
	// valic values for depth GL_RGBA, GL_RGB, GL_RG, GL_RED, GL_REPTH_XOMPONTENT, GL_DEPTH_STENCIL
	

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// error checking
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		bool invalidEnum = error == GL_INVALID_ENUM;
		bool invalidValue = error == GL_INVALID_VALUE;
		std::cout << name << " texture unsuccessfully created\n";
		if (invalidEnum) printf("invalid enum");
		if (invalidValue) printf("invalid enum");
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	setINTERNAL(TEXTURE, name, h_texture);
	return false;
}

bool nsfw::Assets::loadTexture(const char * name, const char * path)
{
	//TODO_D("This should load a texture from a file, using makeTexture to perform the allocation.\nUse STBI, and make sure you switch the format STBI provides to match what openGL needs!");
	
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	const unsigned char* data = stbi_load(path, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	//error checking
	if (data == nullptr)
	{
		std::cout << "error loading" << name << " texture.\n" << stbi_failure_reason();
		return false;
	}

	switch (imageFormat)
	{
	case STBI_grey:			imageFormat = GL_RED;	break;
	case STBI_grey_alpha:	imageFormat = GL_RG;	break;
	case STBI_rgb:			imageFormat = GL_RGB;	break;
	case STBI_rgb_alpha:	imageFormat = GL_RGBA;	break;
	}

	makeTexture(name, imageWidth, imageHeight, imageFormat, data);
	stbi_image_free((void*)data);

	return false;
}

bool nsfw::Assets::loadShader(const char * name, const char * vpath, const char * fpath)
{
	ASSET_LOG(GL_HANDLE_TYPE::SHADER);
	//TODO_D("Load shader from a file.");
	GL_HANDLE h_shader;

	std::ifstream vin(vpath);
	std::string vcontents((std::istreambuf_iterator<char>(vin)), std::istreambuf_iterator<char>());

	std::ifstream fin(fpath);
	std::string fcontents((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

	const char* vsSource = vcontents.c_str();
	const char* fsSource = fcontents.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsSource, 0);
	glCompileShader(vertexShader);

	// error checking
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		bool invalidEnum = error == GL_INVALID_ENUM;
		bool invalidValue = error == GL_INVALID_VALUE;
		std::cout << name << "vertex shader unsuccessfully loaded\n";
		return false;
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsSource, 0);
	glCompileShader(fragmentShader);

	// error checking
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		bool invalidEnum = error == GL_INVALID_ENUM;
		bool invalidValue = error == GL_INVALID_VALUE;
		std::cout << name << "fragment shader unsuccessfully loaded\n";
		return false;
	}

	h_shader = glCreateProgram();
	glAttachShader(h_shader, vertexShader);
	glAttachShader(h_shader, fragmentShader);
	glLinkProgram(h_shader);

	GLint status;
	glGetProgramiv(h_shader, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)	// do we have a problem?!
	{
		GLsizei logLen = 0;
		GLchar errorMsg[1024];
		glGetProgramInfoLog(h_shader, 1024, &logLen, errorMsg);

		std::cerr << errorMsg << std::endl;
	}

	// error checking
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		bool invalidEnum = error == GL_INVALID_ENUM;
		bool invalidValue = error == GL_INVALID_VALUE;
		std::cout << name << "shader handler unsuccessfully loaded\n";
		return false;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// error checking
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		bool invalidEnum = error == GL_INVALID_ENUM;
		bool invalidValue = error == GL_INVALID_VALUE;
		std::cout << name << " shader unsuccessfully loaded\n";
		return false;
	}
	setINTERNAL(SHADER, name, h_shader);
	
	return true;
}

// for include geometry shader
bool nsfw::Assets::loadShader(const char *name, const char *vpath, const char *fpath, const char *gpath)
{
	//TODO_D("Optimize to load geo pass.\n");

	ASSET_LOG(GL_HANDLE_TYPE::SHADER);
	GL_HANDLE h_shader;

	std::ifstream vin(vpath);
	std::string vcontents((std::istreambuf_iterator<char>(vin)), std::istreambuf_iterator<char>());

	std::ifstream fin(fpath);
	std::string fcontents((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

	std::ifstream gin(gpath);
	std::string gcontents((std::istreambuf_iterator<char>(gin)), std::istreambuf_iterator<char>());

	const char* vsSource = vcontents.c_str();
	const char* fsSource = fcontents.c_str();
	const char* gsSource = gcontents.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsSource, 0);
	glCompileShader(fragmentShader);

	unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &gsSource, 0);
	glCompileShader(geometryShader);

	h_shader = glCreateProgram();
	glAttachShader(h_shader, vertexShader);
	glAttachShader(h_shader, fragmentShader);
	glAttachShader(h_shader, geometryShader);
	glLinkProgram(h_shader);

	GLint status;
	glGetProgramiv(h_shader, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)	// do we have a problem?!
	{
		GLsizei logLen = 0;
		GLchar errorMsg[1024];
		glGetProgramInfoLog(h_shader, 1024, &logLen, errorMsg);

		std::cerr << errorMsg << std::endl;
		return false;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);

	setINTERNAL(SHADER, name, h_shader);

	return true;
}

bool nsfw::Assets::loadFBX(const char * name, const char * path)
{
	//TODO_D("FBX file-loading support needed.\nThis function should call loadTexture and makeVAO internally.\nFBX meshes each have their own name, you may use this to name the meshes as they come in.\nMAKE SURE YOU SUPPORT THE DIFFERENCE BETWEEN FBXVERTEX AND YOUR VERTEX STRUCT!\n");
	
	FBXFile file;
	bool success = file.load(path);
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	//error checking
	if (!success)
	{
		std::cout << name << " FBX unsuccessfully loaded\n";
		return false;
	}

	file.initialiseOpenGLTextures();
	
	// load meshes
	for (unsigned int i = 0; i < file.getMeshCount(); i++)
	{
		FBXMeshNode* mesh = file.getMeshByIndex(i);	

		for (int j = 0; j < mesh->m_vertices.size(); j++)
		{
			auto xVert = mesh->m_vertices[j];
			Vertex v;
			v.position = xVert.position;
			v.normal = xVert.normal;
			v.texCoord = xVert.texCoord1;
			v.tangent = xVert.tangent;

			vertices.push_back(v);
		}
		indices = mesh->m_indices;

		makeVAO(mesh->m_name.c_str(), vertices.data(), vertices.size(), indices.data(), indices.size());
	}

	// load textures
	for (unsigned int i = 0; i < file.getTextureCount(); i++)
	{
		FBXTexture* tex = file.getTextureByIndex(i);
		std::string n = name;
		n += tex->name;
		loadTexture(n.c_str(), tex->path.c_str());
	}

	file.unload();
	return true;
}

// not finished
bool nsfw::Assets::loadOBJ(const char * name, const char * path)
{
	TODO_D("OBJ file-loading support needed.\nThis function should call makeVAO and loadTexture (if necessary), MAKE SURE TO TAKE THE OBJ DATA AND PROPERLY LINE IT UP WITH YOUR VERTEX ATTRIBUTES (or interleave the data into your vertex struct).\n");
	/*std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	
	std::vector<Vertex> verticies;
	std::vector<unsigned int>indicies;

	std::string err = tinyobj::LoadObj(shapes, materials, path);*/

	/*for (unsigned int i = 0; i < shapes.size(); i++)
	{

	}*/

	return false;
}

void nsfw::Assets::init()
{
	//TODO_D("Load up some default assets here if you want.");
	
	setINTERNAL(FBO,"Screen",0);
	
	makeVAO("Cube",CubeVerts,24,CubeTris,36);
	makeVAO("Quad",QuadVerts, 4, QuadTris,6);
	/*
	char w[] = { 255,255,255,255 };
	makeTexture("White", 1, 1, GL_RGB8, w);
	*/
	const unsigned char w[] = { 255,0,255,255 };
	makeTexture("Magenta", 1, 1, GL_RGBA, w);

}

void nsfw::Assets::term()
{
	TODO();
	for each(std::pair<AssetKey,unsigned> k in handles)
	{
		switch (k.first.first)
		{
		case VBO:		TODO_D("VBO deletion");		break;
		case IBO:		TODO_D("IBO deletion");		break;
		case VAO:		TODO_D("VAO deletion");		break;
		case SHADER:	TODO_D("Shader deletion");	break;
		case TEXTURE:	TODO_D("Texture deletion"); break;
		case RBO:		TODO_D("RBO deletion");		break;
		case FBO:		TODO_D("FBO deletion");		break;
		}
	}
}


