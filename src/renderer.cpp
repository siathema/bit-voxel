#include "renderer.hpp"
#include "sMath.hpp"
#include "sAssert.hpp"
#include "assets.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "GL/glew.h"

namespace SMOBA
{

	char* Get_Source_From_File(const char* path)
	{
		char* result;
		FILE* fp = fopen(path, "r");
		if (fp == 0)
		{
			printf("Cannot open file!\n");
			return (char*)"";
		}

		u32 count = 0;
		while (fgetc(fp) != EOF) count++;
		fseek(fp, 0, SEEK_SET);
		s_assert(count > 0 ,"File not opened\n");

		result = (char*)malloc(sizeof(i8) * count + 1);
		s_assert(result, "Memory not Allocated\n");

		for (u32 i = 0; i < count + 1; i++)
		{
			result[i] = fgetc(fp);
		}
		result[count] = '\0';
		return result;
	}

	i32 Compile_Vertex_Shader(char* source)
	{
		u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// attach shader source and compile
		glShaderSource(vertexShader, 1, &source, 0);
		glCompileShader(vertexShader);

		i32 success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, 0, infoLog);
			printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s\n", infoLog);
		}
		free(source);
		return vertexShader;
	}

	i32 Compile_Fragment_Shader(char* source)
	{
		u32 fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		// attach shader source and compile
		glShaderSource(fragShader, 1, &source, 0);
		glCompileShader(fragShader);

		i32 success;
		char infoLog[512];
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragShader, 512, 0, infoLog);
			printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s\n",
				   infoLog);
		}
		free(source);
		return fragShader;
	}

	i32 Create_Shader_Program(char* vs, char* fs)
	{
		i32 vertexProgram = Compile_Vertex_Shader(vs);
		i32 fragmentProgram = Compile_Fragment_Shader(fs);

		u32 shaderProgram;
		shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexProgram);
		glAttachShader(shaderProgram, fragmentProgram);
		glLinkProgram(shaderProgram);

		glDeleteShader(vertexProgram);
		glDeleteShader(fragmentProgram);

		return shaderProgram;
	}

	Texture2D::Texture2D(u8* image, i32 width, i32 height)
		: Width(width), Height(height)
	{
		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA,
					 GL_UNSIGNED_BYTE, image);

		free(image);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D::Texture2D(i32 width, i32 height)
		: Width(width), Height(height)
	{
		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//u8* data = new u8[width*height*4];

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA,
					 GL_UNSIGNED_BYTE, 0);
		//delete[] data;
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D::~Texture2D() {}

	void Texture2D::Bind() { glBindTexture(GL_TEXTURE_2D, TextureID); }

	Renderer::Renderer(ViewportInfo *view)
	{
		Viewport = view;
		Load_Shaders();
		Init_Render_Data();
	}

	void Renderer::Render(Queue_Array<RenderCommand> *renderQueue,
						  Camera *camera)
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.4f, 0.8f, 1.0f);

		u32 commandCount = renderQueue->Size;

		for (i32 i = 0; i < commandCount; i++)
		{
			RenderCommand rc = renderQueue->Pop();

			switch (rc.RenderType)
			{
			case TEXTURERENDER:
				Draw_Texture(ASSETS::Get_Texture(rc.Texture),
							camera[0],
							rc.TextureRect,
							vec2(rc.Pos.x, rc.Pos.y),
							vec2(rc.Scale.x, rc.Scale.y),
							rc.Rot,
							rc.Pos.z,
							rc.Color,
							rc.ShaderType);
				break;
			case RECTANGLERENDER:
				Draw_Texture(0,
							camera[0],
							rc.TextureRect,
							vec2(rc.Pos.x, rc.Pos.y),
							vec2(rc.Scale.x, rc.Scale.y),
							rc.Rot,
							rc.Pos.z,
							rc.Color,
							rc.ShaderType);
				break;
			case STRINGRENDER:
				Render_String(rc.String,
							 camera[0],
							 vec2(rc.Pos.x, rc.Pos.y),
							 vec2(rc.Scale.x, rc.Scale.y),
							 rc.Rot,
							 rc.Color);
				break;
			case TEXTRECTRENDER:
				Draw_Text_Rect(rc.String,
							   camera[0],
							   rc.Pos.x,
							   rc.Pos.y,
							   rc.Scale.x,
							   rc.Scale.y,
							   rc.Color);
				break;
			case MESHRENDER:
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
                //glEnable(GL_MULTISAMPLE);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				Draw_Mesh(*ASSETS::Get_Mesh(rc.Mesh),
						 ASSETS::Get_Texture(rc.Texture),
						 camera[1],
						 rc.Pos,
						 rc.Scale,
						 rc.Quat,
						 rc.Color,
						 rc.ShaderType);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
                //glDisable(GL_MULTISAMPLE);
				glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
				break;

            case SIMPLE3DDEBUGLINES:
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glEnable(GL_DEPTH_TEST);
                Draw_3d_Debug_Line(rc.Point1, rc.Point2, rc.Color, camera[1]);
				glDisable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
                break;
			default:
				s_assert(false, "Invalid Render Command!\n");
			}
		}
		Draw_Overlay();
	}

//TODO(matthias): make this better.
#define RENDER_CHARACTER_WIDTH 8
#define RENDER_CHARACTER_HEIGHT 16
#define RENDER_CHARACTER_MAX_PER_LINE 1024
	void Renderer::Draw_Text_Rect(const char* str,
								  Camera &camera,
								  r32 x,
								  r32 y,
								  r32 width,
								  r32 height,
								  vec4 color)
	{
		u32 strLength = strlen(str);
		u32 numCols = (u32)width/(RENDER_CHARACTER_WIDTH);
		s_assert(numCols <= RENDER_CHARACTER_MAX_PER_LINE, "Too many Characters per line!\n");
		u32 numLines;
		if(strLength < numCols)
		{
			numLines = 1;
			Render_String(str, camera, vec2(x,y), vec2(1.0f, 1.0f), 0.0f, color);
			return;
		}
		else
		{
			numLines = (u32)height/(RENDER_CHARACTER_HEIGHT+4);
		}

		char line[RENDER_CHARACTER_MAX_PER_LINE] = {};
		u32 charIndex = 0;

		for(i32 l=0; l<numLines; l++)
		{
			//strncpy(line, str+charIndex, numCols);
			u32 endIndex = 0;

			for(i32 c=0; c<numCols; c++)
			{
				if(str[charIndex] == ' ' &&
				   c == 0)
					charIndex++;
				if(str[charIndex+c] == 0)
				{
					line[c] = 0;
					break;
				}
				if(str[charIndex+c] == ' ')
					endIndex = c;
				line[c] = str[charIndex+c];
			}
			line[numCols] = 0;
			if(str[charIndex+numCols] != ' ' &&
			   strlen(line)==numCols)
			{
				line[endIndex] = 0;
				charIndex += endIndex+1;
			}
			else
			{
				line[charIndex+numCols] = 0;
				charIndex += numCols;
			}

			Render_String(line, camera, vec2(x,y-(RENDER_CHARACTER_HEIGHT*l)), vec2(1.0f, 1.0f), 0.0f, color);

			if(charIndex > strlen(str) || *line == 0)
				break;
		}
	}

	void Renderer::Render_String(const char *str,
								Camera &camera,
								vec2 pos,
								vec2 scale,
								r32 rot,
								vec4 color)
	{
		i32 len = strlen(str);
		for(i32 i=0; i<len; i++)
		{
			char current = str[i];
			i32 padding = 6;
			i32 offset = 32;
			iRect texRect;
			texRect.x = (i32)((current-32)%16)*offset;
			texRect.y = (i32)((current-32)/16)*offset;
			texRect.w = 8;
			texRect.h = 13;

			Draw_Texture(ASSETS::Get_Texture(ASSETS::TEXTURES::DebugBitMapFont),
						camera,
						texRect,
						vec2(pos.x + ((i*(8))), pos.y),
						vec2(scale.x,-scale.y),
						rot, 0.0f, color);
		}
	}

	void Renderer::Draw_Overlay()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glUseProgram(ShaderIDs[FINAL]);

		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, FrameTexture);
		glBindVertexArray(OverlayVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void Renderer::Draw_Mesh(const Mesh& mesh,
							Texture2D *texture,
							Camera& camera,
							vec3& pos,
							vec3& scale,
							quat& rot,
							vec4& color,
							ShaderType shader)
	{
		glUseProgram(ShaderIDs[shader]);

		mat4 model;

		vec3 lightPosition(20.f, 20.f, 0.5f);
		model.make_transform(pos, scale, rot);

        mat4 mvp = model * camera.View * camera.Projection;

		u32 materialAmbLoc = glGetUniformLocation(ShaderIDs[shader],
												  "material.ambient");
		u32 materialDiffLoc = glGetUniformLocation(ShaderIDs[shader],
												   "material.diffuse");
		u32 materialSpecLoc = glGetUniformLocation(ShaderIDs[shader],
												   "material.specular");
		u32 materialShineLoc = glGetUniformLocation(ShaderIDs[shader],
													"material.shininess");
		u32 lightAmbLoc = glGetUniformLocation(ShaderIDs[shader],
											   "light.ambient");
		u32 lightDiffLoc = glGetUniformLocation(ShaderIDs[shader],
												"light.diffuse");
		u32 lightSpecLoc = glGetUniformLocation(ShaderIDs[shader],
												"light.specular");
		u32 lightPosLoc = glGetUniformLocation(ShaderIDs[shader],
											   "light.position");
		u32 viewPosLoc = glGetUniformLocation(ShaderIDs[shader],
											  "viewPos");
		//u32 lightPosLoc = glGetUniformLocation(ShaderIDs[shader], "lightPos");
		//u32 lightColorLoc = glGetUniformLocation(ShaderIDs[shader], "lightColor");
		//u32 objectColorLoc = glGetUniformLocation(ShaderIDs[shader], "objectColor");

		u32 mvpLoc = glGetUniformLocation(ShaderIDs[shader],
												 "mvp");
		glUniform3f(lightAmbLoc, 1.f, 1.0f, 1.0f);
		glUniform3f(lightDiffLoc, 0.5f, 0.5f, 0.5f);
		glUniform3f(lightSpecLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc,
					lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(viewPosLoc, camera.Pos.x, camera.Pos.y, camera.Pos.z);

		glUniformMatrix4fv(mvpLoc,
						   1,
						   GL_FALSE,
						   (GLfloat*)&mvp._m);

        glActiveTexture(GL_TEXTURE0);
        texture->Bind();
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES,
					   mesh.Indices,
					   GL_UNSIGNED_INT,
					   (void*)0);
		glBindVertexArray(0);
	}

	void Renderer::Draw_3d_Debug_Line(vec3 point1,
		vec3 point2,
		vec4 color,
		Camera& camera)
	{

		
		r32 points[] = {
			point1.x, point1.y, point1.z,
			point2.x, point2.y, point2.z
		};
		

		//r32* points = (r32*)malloc(sizeof(r32) * 14);

		u32 indices[] = { 0,1 };
		u32 VAO, VBO;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points) , points, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(r32), (void*)0);
		
		//glBindVertexArray(0);

        glUseProgram(ShaderIDs[SIMPLE3DCOLOR]);
        mat4 mvp = camera.View * camera.Projection;

		u32 colorLoc = glGetUniformLocation(ShaderIDs[SIMPLE3DCOLOR], "inColor");
        u32 mvpLoc = glGetUniformLocation(ShaderIDs[SIMPLE3DCOLOR], "mvp");
		glUniform4f(colorLoc, color.x, color.y, color.z, color.w);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (GLfloat*)&mvp._m);
		glLineWidth(2.0);
		//glBindVertexArray(VAO);

        glDrawArrays(GL_LINES, 0, 2);

        glBindVertexArray(0);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);

	}

	void Renderer::Draw_Texture(Texture2D *texture,
							   Camera &camera,
							   iRect &textureRect,
							   vec2 position,
							   vec2 size,
							   r32 rotate,
							   r32 depth,
							   vec4 color,
							   ShaderType shader)
	{
		// Prepare texture coordinates
		if (shader != FILLRECTANGLE)
		{
			r32 texture_pixel_ratio_x = 1.0f / texture->GetWidth();
			r32 texture_pixel_ratio_y = 1.0f / texture->GetHeight();

			r32 texture_coord_offset_x = texture_pixel_ratio_x * textureRect.left;
			r32 texture_coord_offset_y = texture_pixel_ratio_y * textureRect.top;

			r32 texture_coord_offset_width =
				texture_pixel_ratio_x * textureRect.w;
			r32 texture_coord_offset_height =
				texture_pixel_ratio_y * textureRect.h;

			// top left
			TexCoords[0] = texture_coord_offset_x;
			TexCoords[1] = texture_coord_offset_y + texture_coord_offset_height;
			// bottom right
			TexCoords[2] = texture_coord_offset_x + texture_coord_offset_width;
			TexCoords[3] = texture_coord_offset_y;
			// bottom left
			TexCoords[4] = texture_coord_offset_x;
			TexCoords[5] = texture_coord_offset_y;

			// top left
			TexCoords[6] = texture_coord_offset_x;
			TexCoords[7] = texture_coord_offset_y + texture_coord_offset_height;
			// top right
			TexCoords[8] = texture_coord_offset_x + texture_coord_offset_width;
			TexCoords[9] = texture_coord_offset_y + texture_coord_offset_height;
			// bottom right
			TexCoords[10] = texture_coord_offset_x + texture_coord_offset_width;
			TexCoords[11] = texture_coord_offset_y;

			glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexCoords), TexCoords);
		}
		// Prepare transformations
		glUseProgram(ShaderIDs[shader]);
		mat4 model;
		vec3 scale(textureRect.w * size.x, textureRect.h * size.y, 0.0f);
		model.make_transform(vec3(position.x, position.y, 0.0f),
							 scale,
							 quat(rotate, vec3(0.0f, 0.0f, 1.0f)));
		//model.translation(vec3(position.x, position.y, 0.0f));
		//model = translate(model, vec3(position, 0.0f));

		//model.translate(vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
		//model = translate(model, vec3(0.5f * size.x, 0.5f * size.y, 0.0f));

		//quat rot = quat(rotate, vec3(0.0f, 0.0f, 1.0f));
		//model.rotate(rot);
		//model = rotate(model, rotate, vec3(0.0f, 0.0f, 1.0f));

		//model.translate(vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
		//model = translate(model, vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

		//model.scale(vec3(size.x, size.y, 1.0f));

		u32 modelLoc = glGetUniformLocation(ShaderIDs[shader], "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (GLfloat*)&model._m);

		u32 viewLoc = glGetUniformLocation(ShaderIDs[shader], "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (GLfloat*)&camera.View._m);

		u32 projLoc = glGetUniformLocation(ShaderIDs[shader], "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, (GLfloat*)&camera.Projection._m);

		u32 colorLoc = glGetUniformLocation(ShaderIDs[shader], "spriteColor");
		glUniform4f(colorLoc, color.x, color.y, color.z, color.w);


		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture->TextureID);

		glBindVertexArray(QuadVAO);
		if (shader == DEBUGLINES)
		{
			glDrawArrays(GL_LINE_STRIP, 0, 6);
		}
		else
		{
			if(texture != 0) texture->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			//printf("GLERROR: %d\n", glGetError());
		}
		glBindVertexArray(0);
	}

	void Renderer::Init_Render_Data()
	{
		glClearColor(0.3f, 0.3f, 0.6f, 1.0f);


		r32 vertices[] = {
			// Pos
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,

			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f };

		for (i32 i = 0; i < 12; i++)
		{
			TexCoords[i] = 0.0f;
		}

		glGenBuffers(2, VBOs);

		// vertex VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(vertices),
					 vertices,
					 GL_STATIC_DRAW);

		// tex coords VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(TexCoords),
					 TexCoords,
					 GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &QuadVAO);
		glBindVertexArray(QuadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glVertexAttribPointer(0,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  2 * sizeof(r32),
							  (GLvoid *)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
		glVertexAttribPointer(1,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  2 * sizeof(r32),
							  (GLvoid *)0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);

		// for overlay
		r32 quad[] = {
			// Position    // Texcoords
			-1.0f, 1.0f,   0.0f, 1.0f,
			1.0f, -1.0f,   1.0f, 0.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,

			-1.0f, 1.0f,  0.0f, 1.0f,
			1.0f, 1.0f,   1.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f
		};

		glGenBuffers(1, &VBOs[2]);

		// vertex VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

		// VAO
		glGenVertexArrays(1, &OverlayVAO);
		glBindVertexArray(OverlayVAO);
		glVertexAttribPointer(0,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  4 * sizeof(r32),
							  (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  4 * sizeof(r32),
							  (GLvoid*)(2*sizeof(r32)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);

		// Intitalize FrameBuffer
		glGenFramebuffers(1, &FrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		// attach texture
		//Texture2D* frameTex = new Texture2D(viewportInfo->ScreenWidth, viewportInfo->ScreenHeight);
		glGenTextures(1, &FrameTexture);
		glBindTexture(GL_TEXTURE_2D, FrameTexture);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 Viewport->ScreenWidth,
					 Viewport->ScreenHeight,
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   GL_COLOR_ATTACHMENT0,
							   GL_TEXTURE_2D, FrameTexture,
							   0);
		// create a renderbuffer object for depth and stencil attachment
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER,
							  GL_DEPTH24_STENCIL8,
							  Viewport->ScreenWidth,
							  Viewport->ScreenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,
								  GL_DEPTH_STENCIL_ATTACHMENT,
								  GL_RENDERBUFFER,
								  RBO);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("Error: framebuffer is not complete!\n");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Renderer::Load_Shaders()
	{
		// SIMPLEBLIT
		char* vs = Get_Source_From_File("assets/shaders/SimpleBlit.vs");
		char* fs = Get_Source_From_File("assets/shaders/SimpleBlit.frag");
		u32 id = Create_Shader_Program(vs, fs);
		ShaderIDs[SIMPLEBLIT] = id;

		// DEBUGLINES
		vs = Get_Source_From_File("assets/shaders/DebugLines.vs");
		fs = Get_Source_From_File("assets/shaders/DebugLines.frag");
		id = Create_Shader_Program(vs, fs);
		ShaderIDs[DEBUGLINES] = id;

		// FILLRECTANGLE
		vs = Get_Source_From_File("assets/shaders/FillRectangle.vs");
		fs = Get_Source_From_File("assets/shaders/FillRectangle.frag");
		id = Create_Shader_Program(vs, fs);
		ShaderIDs[FILLRECTANGLE] = id;

		//SIMPLEMESH
		vs = Get_Source_From_File("assets/shaders/SimpleMesh.vs");
		fs = Get_Source_From_File("assets/shaders/SimpleMesh.frag");
		id = Create_Shader_Program(vs, fs);
		ShaderIDs[SIMPLEMESH] = id;

		//FINAL
		vs = Get_Source_From_File("assets/shaders/Final.vs");
		fs = Get_Source_From_File("assets/shaders/Final.frag");
		id = Create_Shader_Program(vs, fs);
		ShaderIDs[FINAL] = id;

		//PHONGLIGHT
		vs = Get_Source_From_File("assets/shaders/Simple3dColor.vertex");
		fs = Get_Source_From_File("assets/shaders/Simple3dColor.frag");
		id = Create_Shader_Program(vs, fs);
		ShaderIDs[SIMPLE3DCOLOR] = id;
	}
}
