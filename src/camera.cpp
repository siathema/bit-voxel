#include "camera.hpp"


namespace SMOBA
{
	void Camera::Init(r32 x,
					  r32 y,
					  r32 z,
					  r32 r,
					  r32 fov,
					  r32 width,
					  r32 height,
					  r32 near,
					  r32 far)
	{
		Pos.x = x;
		Pos.y = y;
		Pos.z = z;
		Direction = vec3(0.0f, 0.0f, -1.0f);
		Up = vec3(0.0f, 1.0f, 0.0f);
		Rot = r;

		Pitch = LastX = LastY = 0.f;
		Yaw = toRadians(-90);

		Height = height;
		Width = width;
		Fov = fov;
		Near = near;
		Far = far;

		View = mat4::identity;
		//view.make_transform(pos, scale, rotation);
		Projection = mat4::orthographic(width, height, near, far);

	}

	void Camera::Rotate2d(r32 r)
	{
		Rot = r;
		View.rotate(quat(Rot, vec3(0.0f, 0.0f, 1.0f)));
	}

	void Camera::Rotate(quat& q)
	{
		//view.rotate(q);
		Rotation = Rotation + q;
		View.rotation(Rotation);
	}

	void Camera::Rotate(r32& t, vec3& axis)
	{
		View.rotate(quat(t, axis));
		Rotation = View.extract_quat();
	}

	void Camera::MoveTo(const vec3& p)
	{
		Pos = p;
		View.translation(vec3(Pos.x, Pos.y, Pos.z));
	}

	void Camera::Move(const vec3& p)
	{
		Pos = Pos + p;
		View.translation(Pos);
	}

	void Camera::SetUp()
	{

	}

	void Camera::Update()
	{
		View.make_transform(Pos, Scale, Rotation);
	}

	void Camera::Update(Input* ip)
	{
		static b8 firstMouse = true;
		static i32 lastWheelY;
		static i32 lastWheelX;
		static r32 wheelLevel = PI/4;

		// NOTE(matthias): Debug code!
		r32 speed = 0.05f;

		if (ip->Keys['w'])
			Pos += speed * -Direction;
		if (ip->Keys['s'])
			Pos -= speed * -Direction;
		if (ip->Keys['a'])
			Pos += vec3::normalize(-Direction.cross(Up)) * speed;
		if (ip->Keys['d'])
			Pos -= vec3::normalize(-Direction.cross(Up)) * speed;

		// mouse control
		if (firstMouse)
		{
			LastX = (r32)ip->MouseX;
			LastY = (r32)ip->MouseY;
			lastWheelX = ip->WheelX;
			lastWheelY = ip->WheelY;
			firstMouse = false;
		}

		r32 xoffset = (r32)ip->MouseX - LastX;
		r32 yoffset = LastY - (r32)ip->MouseY;

		LastX = (r32)ip->MouseX;
		LastY = (r32)ip->MouseY;

		r32 sensitivity = 0.01f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		Yaw -= xoffset;
		Pitch -= yoffset;

		if (Pitch > toRadians(89.0f))
			Pitch = toRadians(89.0);
		if (Pitch < toRadians(-89))
			Pitch = toRadians(-89);

		vec3 front(0.0f);
		front.x = cosf(Yaw) * cosf(Pitch);
		front.y = sinf(Pitch);
		front.z = sinf(Yaw) * cosf(Pitch);
		Direction = vec3::normalize(front);

		View = mat4::look(Pos, Direction, Up);

		//NOTE(matthias): Zoom here
		if (ip->WheelY != 0)
		{
			wheelLevel += (-ip->WheelY * 0.07);
		}
		Zoom(wheelLevel);
	}

	void Camera::Zoom(r32 f)
	{
		if (f < 0.0f)
		{
			Fov = 0.0f;
		}
		else if (f > PI/4)
		{
			Fov = PI/4;
		}
		else
		{
			Fov = f;
		}
		Projection = mat4::perspective(Fov, Width / Height, Near, Far);
	}

	void Camera::SetPerspective()
	{
		Projection = mat4::perspective(Fov, Width/Height, Near, Far);
	}
	void Camera::SetOrthographic()
	{
		Projection = mat4::orthographic(Width, Height, Near, Far);
	}
}
