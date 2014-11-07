//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "graphics/shaders.h"
#include <bgfx.h>

bgfx::UniformHandle Shader::u_texColor;

void createShaderUniforms()
{
   Shader::u_texColor = bgfx::createUniform("u_texColor", bgfx::UniformType::Uniform1i);
}

Shader::Shader(const char* vertex_shader_path, const char* pixel_shader_path)
{
   mPixelShader.idx = bgfx::invalidHandle;
   mVertexShader.idx = bgfx::invalidHandle;
   mProgram.idx = bgfx::invalidHandle;

   // Vertex Shader
   if ( mVertexShaderFile.readMemory(vertex_shader_path) )
   {
      const bgfx::Memory* mem = bgfx::makeRef(mVertexShaderFile.getBuffer(), mVertexShaderFile.getBufferSize());
      mVertexShader = bgfx::createShader(mem);
   }

   // Pixel Shader
   if ( mPixelShaderFile.readMemory(pixel_shader_path) )
   {
      const bgfx::Memory* mem = bgfx::makeRef(mPixelShaderFile.getBuffer(), mPixelShaderFile.getBufferSize());
      mPixelShader = bgfx::createShader(mem);
   }

   // Load Actualy Program
   if ( mPixelShader.idx != bgfx::invalidHandle && mVertexShader.idx != bgfx::invalidHandle )
   {
      mProgram = bgfx::createProgram(mVertexShader, mPixelShader, true);
   }
}

Shader::~Shader()
{
   mVertexShaderFile.close();
   mPixelShaderFile.close();
   bgfx::destroyShader(mVertexShader);
   bgfx::destroyShader(mPixelShader);
   bgfx::destroyProgram(mProgram);
}