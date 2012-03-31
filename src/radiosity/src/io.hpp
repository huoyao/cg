#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __IO_HPP_20120329152228__
#define __IO_HPP_20120329152228__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-29
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <string>

#include "adapt/openmesh.hpp"

namespace cg
{
    /**
     *  Output mesh.
     *  
     *  Mesh face color will be set according to reflectivity.
     *  Color channel value range from 0 to 255.
     */
    void output(openmesh::trimesh &mesh, const std::string &path);
}

#endif // __IO_HPP_20120329152228__