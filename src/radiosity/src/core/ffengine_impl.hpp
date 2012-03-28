#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __FFENGINE_IMPL_HPP_20120328144810__
#define __FFENGINE_IMPL_HPP_20120328144810__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-28
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/assert.hpp>
#include <boost/exception/all.hpp>
#include <boost/format.hpp>

#include <GL/glew.h>
#include <GL/glut.h>

#include <ans/alpha/method.hpp>
#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>

#include "ffengine.hpp"

template<class Mesh>
struct cg::ffengine<Mesh>::data_type
{
    mesh_type *mesh;
    bool inited;
    bool gl_inited;

    data_type() :
        mesh(nullptr),
        inited(false),
        gl_inited(false)
    {}
};

namespace
{
    const int EDGE_1 = 256;	 ///< size (in pixels) of hemi-cube edge
    const int EDGE_2 = 2*EDGE_1;	///< EDGE_1 * 2 (size of important area in hemicube)
    const int EDGE_LENGTH = 3*EDGE_1;	 ///< size (pixels) of render viewport

    template<class Mesh>
    struct ffengine_method : cg::ffengine<Mesh>
    {
        void init_gl();

        /**
        * @param x Offset from left
        * @param y Offset from bottom
        * @param c Position of camera
        * @param at Direction of camera
        * @param up Up vector of camera
        */
        void render_viewport(
            const GLint x,
            const GLint y,
            const vector3r &c,
            const vector3r &at,
            const vector3r &up
            );

        /**
        * @param dest Index of destination patch.
        */
        void render_scene(patch_handle dest);

        /**
         *  Draw triangles.
         */
        void draw();
    };

    template<class T>
    inline auto method(T *p) ->
        decltype(ans::alpha::functional::method<ffengine_method>()(p))
    {
        return ans::alpha::functional::method<ffengine_method>()(p);
    }
}

template<class Mesh>
cg::ffengine<Mesh>::ffengine() : data(ans::alpha::pimpl::use_default_ctor())
{
    //method(this)->init_gl();
}

template<class Mesh>
cg::ffengine<Mesh>::~ffengine()
{
}

template<class Mesh>
void cg::ffengine<Mesh>::init(mesh_type *mesh)
{
    BOOST_ASSERT(mesh);
    data->mesh = mesh;
    if (!data->gl_inited)
    {
        method(this)->init_gl();
        data->gl_inited = true;
    }
    data->inited = true;
}

template<class Mesh>
void cg::ffengine<Mesh>::operator ()(patch_handle shooter, ffcontainer &ffs)
{
    BOOST_ASSERT(data->inited);
    method(this)->render_scene(shooter);
}

template<class Mesh>
void ffengine_method<Mesh>::render_viewport(
    const GLint x,
    const GLint y,
    const vector3r &c,
    const vector3r &at,
    const vector3r &up
    )
{
    glViewport(x,y, 256,256);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (double)EDGE_LENGTH/(double)EDGE_LENGTH, 1e-3, 50);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x(c), y(c), z(c), x(at), y(at), z(at), x(up), y(up), z(up));
    draw();
}

template<class Mesh>
void ffengine_method<Mesh>::render_scene(patch_handle dest)
{
	// clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// destination triangle
    auto &t0 = get_patch(*data->mesh, dest);

	// center of this triangle
    auto c = center(t0);
	
	// normal vector and inverse normal vector of this triangle
    auto norm = normal(t0);
    auto norm_m = -norm;
	
    auto side = cross(norm, vector3r(1, 2, 3));
	if (length(side) < 1e-8)
    {
	    side = cross(norm, vector3r(1, 1, 1)); // a neudelat Vector::operator= ??
    }
    auto side_m = -side;
	
	// side vectors
    vector3r vctD = side;
    vector3r vctC = side_m;
    vector3r vctA = cross(vctD, norm);
    vector3r vctB = cross(vctC, norm);
	
	// points for directions of camera (top and 4 side)
	vector3r at = c + norm;
	vector3r atA = c + vctA;
	vector3r atB = c + vctB;
	vector3r atC = c + vctC;
	vector3r atD = c + vctD;
	
	// top
	renderViewport(256, 256, c, at, vctA);
	
	// 1. side
	renderViewport(256, 512, c, atA, norm_m);
	
	// opposite side
	renderViewport(256, 0, c, atB, norm);
	
	// left side
	renderViewport(0, 256, c, atC, vctA);
	
	// right side
	renderViewport(512, 256, c, atD, vctA);
	
	// render
	//glFlush();
    glutSwapBuffers();
}

template<class Mesh>
void ffengine_method<Mesh>::init_gl()
{
    const int argc = 1;
    const char **argv = {"ffengine"};

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("glcu");
    glutDisplayFunc(&display);
    glutReshapeFunc(&reshape);
    //glutIdleFunc(&render_scene);

    // Init GLEW
    auto err = glewInit();
    if (GLEW_OK != err)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error(glewGetErrorString(err)));
    }
    const char *requirements =
        "GL_VERSION_3_1 " 
        "GL_ARB_pixel_buffer_object "
        "GL_ARB_framebuffer_object "
        "GL_ARB_copy_buffer " 
    if (!glewIsSupported(requirements))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error(str(
            boost::format("%s\ndo not satisfied.") % requirements
            )));
    }

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, EDGE_LENGTH, EDGE_LENGTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (double)EDGE_LENGTH/(double)EDGE_LENGTH, 1e-3, 50);
	glMatrixMode(GL_MODELVIEW);
}

namespace
{
    inline void encode_color(int i)
    {
		glColor3ub((i)&0xff, (i>>8)&0xff, (i>>16)&0xff);
    }

    template<class Patch>
    inline void ensure_triangle(const Patch &t)
    {
        BOOST_ASSERT(3 == vertex_count(t));
    }

    /// to select proper gl function
    inline void glVertex3(double x, double y, double z)
    {
        glVertex3d(x, y, z);
    }
    inline void glVertex3(float x, float y, float z)
    {
        glVertex3f(x, y, z);
    }
}

template<class Mesh>
void ffengine_method<Mesh>::draw()
{
	glBegin(GL_TRIANGLES);
    boost::for_each(patches(*data->mesh), [&](const patch &t)
	{
        ensure_triangle(t);
        encode_color(index(t));
        boost::for_each(vertices(t), [&](const vector3r &v)
        {
			glVertex3(x(v), y(v), z(v));
        });
	});
	glEnd();
}

#endif // __FFENGINE_IMPL_HPP_20120328144810__