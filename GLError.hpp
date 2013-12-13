/* 
 * File:   GLError.hpp
 * Author: prompto
 *
 * Created on December 14, 2013, 3:33 AM
 */

#ifndef GLERROR_HPP
#define	GLERROR_HPP
// reference http://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
void _check_gl_error(const char *file, int line);
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

#endif	/* GLERROR_HPP */

