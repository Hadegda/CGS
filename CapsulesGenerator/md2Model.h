#ifndef _MODEL_MD2_H_
#define _MODEL_MD2_H_

#include <windows.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <vector>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

// number of precalculated normals
#define NUMVERTEXNORMALS        162

// number of precalculated dot product results (for lighting)
#define SHADEDOT_QUANT          16

typedef struct
{
	int     ident;              // magic number. must be equal to "IDP2"
	int     version;            // md2 version. must be equal to 8

	int     skinwidth;          // width of the texture
	int     skinheight;         // height of the texture
	int     framesize;          // size of one frame in bytes

	int     num_skins;          // number of textures
	int     num_xyz;            // number of vertices
	int     num_st;             // number of texture coordinates
	int     num_tris;           // number of triangles
	int     num_glcmds;         // number of opengl commands
	int     num_frames;         // total number of frames

	int     ofs_skins;          // offset to skin names (64 bytes each)
	int     ofs_st;             // offset to s-t texture coordinates
	int     ofs_tris;           // offset to triangles
	int     ofs_frames;         // offset to frame data
	int     ofs_glcmds;         // offset to opengl commands
	int     ofs_end;            // offset to end of file

} md2_header;

typedef struct
{
	short	index_xyz[3];
	short	index_st[3];
} dtriangle_t;

typedef struct
{
	byte	v[3];			// scaled byte to fit in frame mins/maxs
	byte	lightnormalindex;
} dtrivertx_t;

typedef struct
{
	float		scale[3];	// multiply byte verts by this
	float		translate[3];	// then add this
	char		name[16];	// frame name from grabbing
	dtrivertx_t	verts[1];	// variable sized
} daliasframe_t;

///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
	UINT pos;
	DirectX::XMFLOAT2 Tex;
} md2_vertex;

typedef struct
{
	DirectX::XMFLOAT3		scale;	// multiply byte verts by this
	DirectX::XMFLOAT3		translate;	// then add this
	std::string		name;	// frame name
} md2_frame;

struct cbMD2Model
{
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 params; // ->
														/*  ->  UINT normalTabIndex;
														UINT curframe;
														UINT oldframe;
														FLOAT backlerp;*/
};

class ModelMD2 {
public:
	bool LoadModel(wchar_t* path);
	void DrawModel();

	void NextFrame();
	void PrevFrame();

	ModelMD2();
	~ModelMD2();

private:
	bool CreateBuffers(char* buffer);
	bool CreateVertCoordTex(char* buffer);
	bool CreateVertScaleTranslateTex(char* buffer);

	int num_frames;
	int num_vertex;
	int num_tris;
	int num_st;
	int frame_size;
	std::vector<md2_frame> frames;
	size_t indices;

	ID3D11Buffer* pVertices;
	ID3D11Buffer* pIndices;
	ID3D11Buffer* pCBuffer;

	ID3D11Texture2D* pTexture;
	ID3D11ShaderResourceView* pTexSRV;
	ID3D11Texture2D* pTextureST;
	ID3D11ShaderResourceView* pTexScaleTranslateSRV;

	ID3D11SamplerState* sampler_state;

	int curframe;
	int oldframe;
};

#endif