#pragma once

#include "game.hpp"
#include "sMath.hpp"

namespace SMOBA
{
	struct Camera
	{
		r32 Rot, Height, Width, Fov, Near, Far;
		// NOTE(matthias): so much debug!
		//  !		!
		r32 Pitch, Yaw, LastX, LastY;
		vec3 Up;
		vec3 Direction;
		vec3 Pos;
		vec3 Scale;
		quat Rotation;
		mat4 View;
		mat4 Projection;

		Camera() : Up(0.0f, 1.0f, 0.0f),
				   Direction(),
				   Pos(),
				   Scale(1.0f, 1.0f, 1.0f),
				   Rotation(),
				   View(),
				   Projection() {}

		Camera(Camera& c) {
			Rot = c.Rot;
			Height = c.Height;
			Width = c.Width;
			Fov = c.Fov;
			Near = c.Near;
			Far = c.Far;
			Pitch = c.Pitch;
			Yaw = c.Yaw;
			LastX = c.LastX;
			LastY = c.LastY;
			Up = c.Up;
			Direction = c.Direction;
			Pos = c.Pos;
			Scale = c.Scale;
			Rotation = c.Rotation;
			View = c.View;
			Projection = c.Projection;
		}

		void Init(r32 x,
				  r32 y,
				  r32 z,
				  r32 r,
				  r32 fov,
				  r32 width,
				  r32 height,
				  r32 near,
				  r32 far);
		void Rotate2d(r32 r);
		void Rotate(quat& q);
		void Rotate(r32& t, vec3& axis);
		void MoveTo(const vec3& p);
		void Move(const vec3& p);
		void SetUp();
		void Update();
		void Update(Input* ip);
		void Zoom(r32 fov);
		void SetPerspective();
		void SetOrthographic();
	};
}
