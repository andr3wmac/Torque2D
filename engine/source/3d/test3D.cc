#include "test3D.h"
#include "console/consoleInternal.h"
#include "graphics/shaders.h"

#include <bgfx.h>
#include <bx/fpumath.h>

// assimp include files. 
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

struct PosColorVertex
{
	F32 m_x;
	F32 m_y;
	F32 m_z;
	U32 m_abgr;

	static void init()
	{
		ms_decl
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
			.end();
	};

	static bgfx::VertexDecl ms_decl;
};

bgfx::VertexDecl PosColorVertex::ms_decl;

static PosColorVertex s_cubeVertices[8] =
{
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeIndices[36] =
{
	0, 1, 2, // 0
	1, 3, 2,
	4, 6, 5, // 2
	5, 6, 7,
	0, 2, 4, // 4
	4, 2, 6,
	1, 5, 3, // 6
	5, 7, 3,
	0, 4, 1, // 8
	4, 5, 1,
	2, 3, 6, // 10
	6, 3, 7,
};

bgfx::VertexBufferHandle vbh;
bgfx::IndexBufferHandle ibh;
Shader* cubeShader = NULL;
F32 tickCount = 0.0f;

void test3DRender(U32 width, U32 height)
{
   if ( cubeShader == NULL )
   {
      // Test Assimp.
      const struct aiScene* mScene = aiImportFile("models/spider.obj", aiProcessPreset_TargetRealtime_MaxQuality);
      if ( mScene )
      {
         Con::printf("[ASSIMP] Mesh Count: %d", mScene->mNumMeshes);
         Con::printf("[ASSIMP] Material Count: %d", mScene->mNumMaterials);
         aiReleaseImport(mScene);
      }

      // Create vertex stream declaration.
	   PosColorVertex::init();

	   // Create static vertex buffer.
	   vbh = bgfx::createVertexBuffer(
		     // Static data can be passed with bgfx::makeRef
		     bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) )
		   , PosColorVertex::ms_decl
		   );

	   // Create static index buffer.
	   ibh = bgfx::createIndexBuffer(
		   // Static data can be passed with bgfx::makeRef
		   bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices) )
		   );

	   // Create program from shaders.
	   cubeShader = new Shader("shaders/cubes_vs.bin", "shaders/cubes_fs.bin");
   }

   // TODO: Not sure if it matters, maybe not do this every frame?
   bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
		, 0
		, 1.0f
		, 0
	);

   // Dummy submit to ensure viewport is cleared.
   bgfx::submit(0);

   F32 at[3]  = { 0.0f, 0.0f,   0.0f };
   F32 eye[3] = { 0.0f, 0.0f, -30.0f };

   float view[16];
	bx::mtxLookAt(view, eye, at);

	float proj[16];
	bx::mtxProj(proj, 60.0f, float(width)/float(height), 0.1f, 100.0f);
	bgfx::setViewTransform(0, view, proj);

	// Set view 0 default viewport.
	bgfx::setViewRect(0, 0, 0, width, height);

   // Submit 11x11 cubes.
   for (uint32_t yy = 0; yy < 11; ++yy)
   {
	   for (uint32_t xx = 0; xx < 11; ++xx)
	   {
		   float mtx[16];
		   bx::mtxRotateXY(mtx, tickCount + xx*0.21f, tickCount + yy*0.37f);
		   mtx[12] = -15.0f + float(xx)*3.0f;
		   mtx[13] = -15.0f + float(yy)*3.0f;
		   mtx[14] = 0.0f;

		   // Set model matrix for rendering.
		   bgfx::setTransform(mtx);

		   // Set vertex and fragment shaders.
         bgfx::setProgram(cubeShader->mProgram);

		   // Set vertex and index buffer.
		   bgfx::setVertexBuffer(vbh);
		   bgfx::setIndexBuffer(ibh);

		   // Set render states.
		   bgfx::setState(BGFX_STATE_DEFAULT);

		   // Submit primitive for rendering to view 0.
		   bgfx::submit(0);
	   }
   }

   tickCount += 0.001f;
}