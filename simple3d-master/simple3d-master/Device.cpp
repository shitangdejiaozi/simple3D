#include <windows.h>
#include <math.h>
#include <iostream>
#include "BasicMath.h"
#include "Device.h"
using namespace std;

void Device_Init(device_PTR device, int width, int height, IUINT32 color, IUINT32 * framebuffer, float * zbuffer, int render_state)
{
	device->width = width;
	device->height = height;
	device->background = color;
	device->foreground = 0;
	device->framebuffer = framebuffer;
	device->zbuffer = zbuffer;
	device->render_state = render_state;
}

void Device_Set_RenderState(device_PTR device, int reander_state)
{
	device->render_state = reander_state;
}

void Device_Clear(device_PTR device)
{
	if (device->framebuffer != NULL)
	{
		for (int y = 0; y < device->height; y++)
		{
			for (int x = 0; x < device->width;x++)
			{
				device->framebuffer[y * device->width + x] = device->background;
			}
		}
	}

	if (device->zbuffer != NULL)
	{
		memset(device->zbuffer, 0, device->width * device->height * sizeof(float));
	}
}



//向frambuffer填充像素
void Device_Draw_Pixel(device_PTR device, int x, int y, IUINT32 color)
{
	device->framebuffer[y * device->width + x] = color;
}

//划线
void Device_Draw_Line(device_PTR device, int x0, int y0, int x1, int y1, IUINT32 color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int ux = ((dx > 0) << 1) - 1;
	int uy = ((dy > 0) << 1) - 1;
	int error = 0;
	dx = abs(dx);
	dy = abs(dy);

	if (dx > dy)
	{
		for (int x = x0, y = y0; x != x1 + ux; x += ux)
		{
			Device_Draw_Pixel(device, x, y, color);
			error += dy;
			if ((error<< 1) >= dx)
			{
				y += uy;
				error -= dx;
			}
		}
	}
	else
	{
		for (int y = y0, x = x0; y != y1 + uy; y += uy)
		{
			Device_Draw_Pixel(device, x, y, color);
			error += dx;
			if ((error << 1) >= dy)
			{
				x += ux;
				error -= dy;
			}
		}
	}
}

void Reset_RENDERLIST(RENDERLIST_PTR  render_list)
{
	render_list->num_polys = 0;
}

//将多边形插入到渲染列表
int Insert_POLYF_RENDERLIST(RENDERLIST_PTR render_list, POLYF_PTR poly)
{
	if (render_list->num_polys == RENDERLIST_MAX_POLYS)
		return 0;

	int num = render_list->num_polys;
	render_list->poly_ptrs[num] =  &render_list->poly_data[num];
	memcpy((void *)&render_list->poly_data[num], (void *)poly, sizeof(POLYF));
	render_list->num_polys++;
	return 1;
}

int Insert_POLY_RENDERLIST(RENDERLIST_PTR render_list, POLY_PTR poly)
{
	if (render_list->num_polys == RENDERLIST_MAX_POLYS)
		return 0;

	int num = render_list->num_polys;
	render_list->poly_ptrs[num] = &render_list->poly_data[num];

	render_list->poly_data[num].state = poly->state;
	render_list->poly_data[num].attr = poly->attr;
	render_list->poly_data[num].color = poly->color;

	Vector4D_Copy(&render_list->poly_data[num].tvlist[0], &poly->vertex_list[poly->vert[0]]);
	Vector4D_Copy(&render_list->poly_data[num].tvlist[1], &poly->vertex_list[poly->vert[1]]);
	Vector4D_Copy(&render_list->poly_data[num].tvlist[2], &poly->vertex_list[poly->vert[2]]);

	Vector4D_Copy(&render_list->poly_data[num].vlist[0], &poly->vertex_list[poly->vert[0]]);
	Vector4D_Copy(&render_list->poly_data[num].vlist[1], &poly->vertex_list[poly->vert[1]]);
	Vector4D_Copy(&render_list->poly_data[num].vlist[2], &poly->vertex_list[poly->vert[2]]);

	render_list->num_polys++;
	return 1;
}

//物体插入到渲染列表
int Insert_OBJECT_RENDERLIST(RENDERLIST_PTR render_list, OBJECT_PTR obj, bool insert_local)
{
	if (!(obj->state & OBJECT_STATE_ACTIVE) ||
		(obj->state & OBJECT_STATE_CULLED) ||
		!(obj->state & OBJECT_STATE_VISIBLE))
		return(0);

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY_PTR curr_poly = &obj->plist[poly];

		if (!(curr_poly->state & POLY_STATE_ACTIVE) ||
			(curr_poly->state & POLY_STATE_CLIPPED) ||
			(curr_poly->state & POLY_STATE_BACKFACE))
			continue; // move onto next poly

		POINT4D_PTR vlist_old = curr_poly->vertex_list;
		if (insert_local)
		{
			curr_poly->vertex_list = obj->vlist_local; //多边形的顶点列表是局部的
		}
		else
		{
			curr_poly->vertex_list = obj->vlist_trans; //多边形的顶点列表是变换后的
		}

		Insert_POLY_RENDERLIST(render_list, curr_poly);

		curr_poly->vertex_list = vlist_old;

	}
	return 1;
}

void PrintPoint(POINT4D point)
{
	cout << point.x << ":" << point.y << ":" << point.z   << ":" << point.w << endl;
}

//通用的渲染列表 变换函数
void Transform_RENDERLIST(RENDERLIST_PTR render_list, MATRIX4X4_PTR mt, int coord_select)
{
	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int poly = 0; poly < render_list->num_polys; poly++)
		{
			POLYF_PTR curr_poly = render_list->poly_ptrs[poly];

			if (curr_poly == NULL || !(curr_poly->state & POLY_STATE_ACTIVE) || curr_poly->state & POLY_STATE_CLIPPED || curr_poly->state & POLY_STATE_BACKFACE)
				continue;

			for (int vertex = 0; vertex < 3; vertex++)
			{
				Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex], mt, &curr_poly->tvlist[vertex]);
				cout << "cur poly" << vertex << "vlist:" << endl;
				PrintPoint(curr_poly->vlist[vertex]);
				cout << " change : " << endl;
				PrintPoint(curr_poly->tvlist[vertex]);
			}
		}
	}
	else if (coord_select == TRANSFORM_TRANS_ONLY)
	{
		for (int poly = 0; poly < render_list->num_polys; poly++)
		{
			POLYF_PTR curr_poly = render_list->poly_ptrs[poly];

			if (curr_poly == NULL || !(curr_poly->state & POLY_STATE_ACTIVE) || curr_poly->state & POLY_STATE_CLIPPED || curr_poly->state & POLY_STATE_BACKFACE)
				continue;

			for (int vertex = 0; vertex < 3; vertex++)
			{
				POINT4D presult;
				Mat_Mul_VECTOR4D_4X4(&curr_poly->tvlist[vertex], mt, &presult);
				Vector4D_Copy(&curr_poly->tvlist[vertex], &presult);
				cout << "cur poly" << vertex << "vlist:" << endl;
				PrintPoint(curr_poly->vlist[vertex]);
				cout << " change : " << endl;
				PrintPoint(curr_poly->tvlist[vertex]);
			}
		}
	}
	
}



//构建局部到世界的变换矩阵
void Build_Model_To_World_Matrix4X4(VECTOR3D_PTR vpos, MATRIX4X4_PTR mt)
{
	Mat_Init_4X4(mt, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		vpos->x, vpos->y, vpos->z, 1);
}

//初始化相机的参数
void Init_Camera(CAMERA_PTR cam, int cam_attr, POINT4D_PTR cam_pos, VECTOR4D_PTR cam_dir, POINT4D_PTR cam_target, float near_clip_z, float far_clip_z, float fov, float  viewport_width, float viewport_height, bool isNormalize)
{
	cam->attr = cam_attr;

	Vector4D_Copy(&cam->pos, cam_pos);
	Vector4D_Copy(&cam->dir, cam_dir);

	Vector4D_Init_XYZ(&cam->u, 1, 0, 0);
	Vector4D_Init_XYZ(&cam->v, 0, 1, 0);
	Vector4D_Init_XYZ(&cam->n, 0, 0, 1);

	if (cam_target != NULL)
	{
		Vector4D_Copy(&cam->target, cam_target);
	}
	
	cam->near_clip_z = near_clip_z;
	cam->far_clip_z = far_clip_z;
	cam->fov = fov;
	cam->viewport_width = viewport_width;
	cam->viewport_height = viewport_height;
	cam->aspect_ratio = viewport_width / viewport_height;

	if (isNormalize)
	{
		cam->viewplane_width = 2.0f;
		cam->viewplane_height = 2.0f / cam->aspect_ratio;  //默认相机都是视平面归一化的
	}
	else
	{
		cam->viewplane_width = viewport_width;
		cam->viewplane_height = viewport_height;  //视平面于屏幕大小一样,这个时候，使用合并形式的透视和屏幕变换
	}
	
	float tan_fov_div2 = tan(DEG_TO_RAD(fov / 2));
	cam->view_dist = 0.5 * cam->viewplane_width * tan_fov_div2; //如果fov= 90， 按照视平面归一化，d = 1


}

//用欧拉模型来构建世界到相机的矩阵，坐标轴的顺序为zyx
void Build_World_To_Camera_Matrix_Euler(CAMERA_PTR cam)
{
	MATRIX4X4 mt,//相机的平移矩阵
		mx, my, mz,//相机的轴的旋转矩阵
		mrot, //旋转矩阵的积
		mtmp;

	//根据相机的位置来构建平移矩阵
	Mat_Init_4X4(&mt, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam->pos.x, -cam->pos.y, -cam->pos.z, 1);

	//提取欧拉角
	float theta_x = cam->dir.x;
	float theta_y = cam->dir.y;
	float theta_z = cam->dir.z;

	theta_x = DEG_TO_RAD(theta_x);
	theta_y = DEG_TO_RAD(theta_y);
	theta_z = DEG_TO_RAD(theta_z);

	float cos_theta = cos(theta_x);
	float sin_theta = -sin(theta_x);

	//构建x轴的旋转矩阵
	Mat_Init_4X4(&mx, 1, 0, 0, 0,
		0, cos_theta, sin_theta, 0,
		0, -sin_theta, cos_theta, 0,
		0, 0, 0, 1);

	cos_theta = cos(theta_y);
	sin_theta = -sin(theta_y);

	Mat_Init_4X4(&my, cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1);

	cos_theta = cos(theta_z);
	sin_theta = -sin(theta_z);

	Mat_Init_4X4(&mz, cos_theta, sin_theta, 0, 0,
		-sin_theta, cos_theta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	Mat_Mul_4X4(&mz, &my, &mtmp);
	Mat_Mul_4X4(&mtmp, &mx, &mrot);

	Mat_Mul_4X4(&mt, &mrot, &cam->view);
	
}

//用uvn模型来构建世界到相机变换矩阵
void Build_World_To_Camera_Matrix_UVN(CAMERA_PTR cam)
{
	MATRIX4X4 mt, mt_uvn, mtmp;

	//根据相机的位置来构建平移矩阵
	Mat_Init_4X4(&mt, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam->pos.x, -cam->pos.y, -cam->pos.z, 1);

	Vector4D_Build(&cam->pos, &cam->target, &cam->n);

	Vector4D_Cross(&cam->v, &cam->n, &cam->u);
	Vector4D_Cross(&cam->u, &cam->n, &cam->v);

	Vector4D_Normalize(&cam->u);
	Vector4D_Normalize(&cam->v);
	Vector4D_Normalize(&cam->n);

	Mat_Init_4X4(&mt_uvn, cam->u.x, cam->v.x, cam->n.x, 0,
		cam->u.y, cam->v.y, cam->n.y, 0,
		cam->u.z, cam->v.z, cam->n.z, 0,
		0, 0, 0, 1);

	Mat_Mul_4X4(&mt, &mt_uvn, &cam->view);

}

//构建透视矩阵，得到的坐标是齐次的，需要转换到 3D ,默认
void Build_Camera_To_Perspective_Matrix(CAMERA_PTR cam)
{
	Mat_Init_4X4(&cam->projection, cam->view_dist, 0, 0, 0,
		0, cam->view_dist * cam->aspect_ratio, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 0);
}

/// 齐次4D转3D，针对渲染列表的变换
void Convert_From_Homogeneous4D_Renderlist(RENDERLIST_PTR render_list)
{
	for (int poly = 0; poly < render_list->num_polys; poly++)
	{
		POLYF_PTR curr_poly = render_list->poly_ptrs[poly];

		if (curr_poly == NULL || !(curr_poly->state & POLY_STATE_ACTIVE) || curr_poly->state & POLY_STATE_CLIPPED || curr_poly->state & POLY_STATE_BACKFACE)
			continue;
		for (int vertex = 0; vertex < 3; vertex++)
		{
			Vector4D_Div_By_W(&curr_poly->tvlist[vertex]);
		}
	}
}

//认为视平面坐标已经是3D的
void Build_Perspective_To_Screen_Matrix(CAMERA_PTR cam)
{
	float alpha = 0.5 * cam->viewport_width;
	float beta = 0.5 * cam->viewport_height;

	Mat_Init_4X4(&cam->screen, alpha, 0, 0, 0,
		0, -beta, 0, 0,
		0, 0, 1, 0,
		alpha, beta, 0, 1);
}

//构建相机到屏幕的矩阵，得到的是4D坐标，需要齐次转3D,适用视平面于屏幕大小相同
void Build_Camera_To_Screen_Matrix(CAMERA_PTR cam)
{
	float alpha = 0.5 * cam->viewport_width;
	float beta = 0.5 * cam->viewport_height;

	Mat_Init_4X4(&cam->ps, cam->view_dist, 0, 0, 0,
		0, -cam->view_dist, 0, 0,
		alpha, beta, 1, 1,
		0, 0, 0, 0);


}
//适用于d = 1的情况
void Camera_To_Perspective_Renderlist(RENDERLIST_PTR render_list, CAMERA_PTR cam)
{
	for (int poly = 0; poly < render_list->num_polys; poly++)
	{
		POLYF_PTR curr_poly = render_list->poly_ptrs[poly];
		if (curr_poly == NULL || !(curr_poly->state & POLY_STATE_ACTIVE) || curr_poly->state & POLY_STATE_CLIPPED || curr_poly->state & POLY_STATE_BACKFACE)
			continue;


		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;
			curr_poly->tvlist[vertex].x = cam->view_dist * curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = cam->view_dist * curr_poly->tvlist[vertex].y  * cam->aspect_ratio/ z;

		}
	}
}

//适用于d = 1的情况，最后乘以缩放因子，到屏幕大小
void Perspective_To_Screen_Renderlist(RENDERLIST_PTR render_list, CAMERA_PTR cam)
{

	for (int poly = 0; poly < render_list->num_polys; poly++)
	{
		POLYF_PTR curr_poly = render_list->poly_ptrs[poly];
		if (curr_poly == NULL || !(curr_poly->state & POLY_STATE_ACTIVE) || curr_poly->state & POLY_STATE_CLIPPED || curr_poly->state & POLY_STATE_BACKFACE)
			continue;

		float alpha = 0.5 * cam->viewport_width;
		float beta = 0.5 * cam->viewport_height;

		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;
			

			curr_poly->tvlist[vertex].x = alpha + curr_poly->tvlist[vertex].x * alpha;
			curr_poly->tvlist[vertex].y = beta - beta * curr_poly->tvlist[vertex].y;
		}

	}
}


//合并透视变换和屏幕变换,没有用矩阵形成，适用于视平面于屏幕大小一样的情况
void Camera_To_Screen_Renderlist(RENDERLIST_PTR render_list, CAMERA_PTR cam)
{
	for (int poly = 0; poly < render_list->num_polys; poly++)
	{
		POLYF_PTR curr_poly = render_list->poly_ptrs[poly];
		if (curr_poly == NULL || !(curr_poly->state & POLY_STATE_ACTIVE) || curr_poly->state & POLY_STATE_CLIPPED || curr_poly->state & POLY_STATE_BACKFACE)
			continue;

		float alpha = 0.5 * cam->viewport_width;
		float beta = 0.5 * cam->viewport_height;

		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;
			curr_poly->tvlist[vertex].x = cam->view_dist * curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = cam->view_dist * curr_poly->tvlist[vertex].y / z;

			curr_poly->tvlist[vertex].x += alpha;
			curr_poly->tvlist[vertex].y = - curr_poly->tvlist[vertex].y + beta;
		}
		
	}
}

void Draw_Renderlist_Wire(RENDERLIST_PTR render_list, device_PTR device)
{
	for (int poly = 0; poly < render_list->num_polys; poly++)
	{
		POLYF_PTR curr_poly = render_list->poly_ptrs[poly];
		if (curr_poly == NULL || !(curr_poly->state & POLY_STATE_ACTIVE) || curr_poly->state & POLY_STATE_CLIPPED || curr_poly->state & POLY_STATE_BACKFACE)
			continue;

		//绘制三角形
		Device_Draw_Line(device, curr_poly->tvlist[0].x, curr_poly->tvlist[0].y, curr_poly->tvlist[1].x, curr_poly->tvlist[1].y, curr_poly->color);
		Device_Draw_Line(device, curr_poly->tvlist[1].x, curr_poly->tvlist[1].y, curr_poly->tvlist[2].x, curr_poly->tvlist[2].y, curr_poly->color);
		Device_Draw_Line(device, curr_poly->tvlist[2].x, curr_poly->tvlist[2].y, curr_poly->tvlist[0].x, curr_poly->tvlist[0].y, curr_poly->color);


	}
}

void Build_XYZ_Rotation_Matrix(float a, float b, float c, MATRIX4X4_PTR mt)
{
	/*a = DEG_TO_RAD(a);
	b = DEG_TO_RAD(b);*/

	MATRIX4X4 mx, my, mz, mtmp;
	//int seq = 0;
	//float sin_theta = 0, cos_theta = 0;

	//if (a > 0)
	//	seq = 1;
	//if (b > 0)
	//	seq = 2;

	//switch (seq)
	//{
	//case 0:
	//{
	//	return ;
	//}break;
	//case 1:
	//{
	//	cos_theta = cos(a);
	//	sin_theta = sin(a);

	//	// set the matrix up 
	//	Mat_Init_4X4(mt, 1, 0, 0, 0,
	//		0, cos_theta, sin_theta, 0,
	//		0, -sin_theta, cos_theta, 0,
	//		0, 0, 0, 1);

	//	return;
	//}break;
	//case 2:
	//{
	//	cos_theta = cos(b);
	//	sin_theta = -sin(b);

	//	// set the matrix up 
	//	Mat_Init_4X4(mt, cos_theta, 0, -sin_theta, 0,
	//		0, 1, 0, 0,
	//		sin_theta, 0, cos_theta, 0,
	//		0, 0, 0, 1);

	//}break;
	//default: break;
	//}

	a = DEG_TO_RAD(a);
	b = DEG_TO_RAD(b);
	c = DEG_TO_RAD(c);

	float cos_theta = cos(a);
	float sin_theta = sin(a);

	//构建x轴的旋转矩阵
	Mat_Init_4X4(&mx, 1, 0, 0, 0,
		0, cos_theta, sin_theta, 0,
		0, -sin_theta, cos_theta, 0,
		0, 0, 0, 1);

	cos_theta = cos(b);
	sin_theta = sin(b);

	Mat_Init_4X4(&my, cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1);

	cos_theta = cos(c);
	sin_theta = sin(c);

	Mat_Init_4X4(&mz, cos_theta, sin_theta, 0, 0,
		-sin_theta, cos_theta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	Mat_Mul_4X4(&mz, &my, &mtmp);
	Mat_Mul_4X4(&mtmp, &mx, mt);

}

void Transform_OBJECT(OBJECT_PTR obj, MATRIX4X4_PTR mt, int coord_select)
{
	switch (coord_select)
	{
		case TRANSFORM_LOCAL_ONLY:
		{
			for (int vertex = 0; vertex < obj->num_vertices; vertex++)
			{
				POINT4D presult;
				Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], mt, &presult);

				Vector4D_Copy(&obj->vlist_local[vertex], &presult);
			}
		}break;
		case TRANSFORM_LOCAL_TO_TRANS:
		{
			for (int vertex = 0; vertex < obj->num_vertices; vertex++)
			{
				POINT4D presult;
				Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], mt, &obj->vlist_trans[vertex]);

			}
		}break;
		case TRANSFORM_TRANS_ONLY:
		{
			for (int vertex = 0; vertex < obj->num_vertices; vertex++)
			{
				POINT4D presult;
				Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex], mt, &presult);

				Vector4D_Copy(&obj->vlist_trans[vertex], &presult);
			}
		}break;
		default: break;
	}
}

void Remove_Backface_RENDERLIST(RENDERLIST_PTR render_list, CAMERA_PTR cam)
{
	for (int poly = 0; poly < render_list->num_polys; poly++)
	{
		POLYF_PTR curr_poly = render_list->poly_ptrs[poly];
		if (curr_poly == NULL || !(curr_poly->state & POLY_STATE_ACTIVE) || curr_poly->state & POLY_STATE_CLIPPED || curr_poly->state & POLY_STATE_BACKFACE ||curr_poly->attr & POLY_ATTR_2SIDE )
			continue;

		VECTOR4D u, v, n;
		Vector4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
		Vector4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);

		//计算法线
		Vector4D_Cross(&u, &v, &n);

		VECTOR4D view;
		Vector4D_Build(&curr_poly->tvlist[0], &cam->pos, &view);

		float dp = Vector4D_Dot(&n, &view);
		if (dp <= 0.0)
		{
			SET_BIT(curr_poly->state, POLY_STATE_BACKFACE);
		}
	}
}

void Remove_Backface_OBJECT(OBJECT_PTR obj, CAMERA_PTR cam)
{
	//物体被剔除了
	if (obj->state & OBJECT_STATE_CULLED)
		return;

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY_PTR curr_poly = &obj->plist[poly];
		if (curr_poly == NULL || !(curr_poly->state & POLY_STATE_ACTIVE) || curr_poly->state & POLY_STATE_CLIPPED || curr_poly->state & POLY_STATE_BACKFACE || curr_poly->attr & POLY_ATTR_2SIDE)
			continue;
		int vindex_0 = curr_poly->vert[0];
		int vindex_1 = curr_poly->vert[1];
		int vindex_2 = curr_poly->vert[2];

		VECTOR4D u, v, n;
		Vector4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
		Vector4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);

		Vector4D_Cross(&u, &v, &n);

		VECTOR4D view;
		Vector4D_Build(&obj->vlist_trans[vindex_0], &cam->pos, &view);

		float dp = Vector4D_Dot(&n, &view);
		if (dp <= 0.0)
		{
			SET_BIT(curr_poly->state, POLY_STATE_BACKFACE);
		}

	}
}

void Reset_OBJECT(OBJECT_PTR obj)
{
	RESET_BIT(obj->state, OBJECT_STATE_CULLED);

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY_PTR curr_poly = &obj->plist[poly];
		if (!(curr_poly->state & POLY_STATE_ACTIVE))
			continue;

		RESET_BIT(curr_poly->state, POLY_STATE_BACKFACE);
		RESET_BIT(curr_poly->state, POLY_STATE_CLIPPED);
	}
}



