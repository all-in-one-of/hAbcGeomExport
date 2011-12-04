/*
* Copyright (c) 2011
*	Side Effects Software Inc.  All rights reserved.
*
* Redistribution and use of Houdini Development Kit samples in source and
* binary forms, with or without modification, are permitted provided that the
* following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. The name of Side Effects Software may not be used to endorse or
*    promote products derived from this software without specific prior
*    written permission.
*
* THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
* NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __hAbcGeomExport_h__
#define __hAbcGeomExport_h__

#include <OBJ/OBJ_Node.h>
#include <ROP/ROP_Node.h>
#include <SOP/SOP_Node.h>
#include <OP/OP_Node.h>

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include <vector>
#include <string>
#include <map>

#include <boost/shared_ptr.hpp>





#define STR_PARM(name, idx, vi, t) \
		{ evalString(str, name, &ifdIndirect[idx], vi, (float)t); }

#define STR_SET(name, idx, vi, t) \
		{ setString(str, name, ifdIndirect[idx], vi, (float)t); }

#define STR_GET(name, idx, vi, t) \
		{ evalStringRaw(str, name, &ifdIndirect[idx], vi, (float)t); }






class OP_TemplatePair;
class OP_VariablePair;

namespace HDK_Sample
{
	/**		Class for storing all related stuff about an object to be exported.

			The object is an Obj/SOP (xform+geometry) combination.
			Currently only poly meshes are supported.
	*/
	class GeoObject
	{
	public:
		/**	Initialize data shared between all objects.
		*/
		static void init(
			Alembic::AbcGeom::OArchive *archive,
			Alembic::AbcGeom::TimeSamplingPtr & timesampling
		)
		{
			assert(archive && timesampling);
			_oarchive = archive;
			_ts = timesampling;
		}

	public:
		GeoObject( OP_Node *obj_node, GeoObject *parent=0 );
		~GeoObject();

	public:
		bool		writeSample( float time );
		char const *	pathname() const { return _path.c_str(); }
		char const *	sop_name() const { return _sopname.c_str(); }


	private:
		static Alembic::AbcGeom::OArchive *
						_oarchive;	// oarchive 'stream' to work in
		static Alembic::AbcGeom::TimeSamplingPtr
						_ts;		// time-sampling specs (boost shared_ptr)

	private:
		GeoObject *			_parent;	// hierarchy parent
		OBJ_Node *			_op_obj;	// geometry xform
		SOP_Node *			_op_sop;	// SOP node to export
		std::string			_name;		// obj (xform) name
		std::string			_path;		// obj full path
		std::string			_sopname;	// SOP name
		Alembic::AbcGeom::OXform *	_xform;		// output xform obj
		Alembic::AbcGeom::OPolyMesh *	_outmesh;	// output polymesh obj
	};



	/// Type: array of GeoObjects.
	typedef std::vector< boost::shared_ptr<GeoObject> > GeoObjects;



	/**	Alembic Geometry Export ROP node declaration.
	*/
	class hAbcGeomExport : public ROP_Node
	{
	public:

		/// Provides access to our parm templates.
		static OP_TemplatePair *getTemplatePair();

		/// Provides access to our variables.
		static OP_VariablePair *getVariablePair();

		/// Creates an instance of this node.
		static OP_Node *myConstructor(OP_Network * net, const char *name, OP_Operator * op);

	protected:
		  hAbcGeomExport(OP_Network * net, const char *name, OP_Operator * entry);
		  
		  virtual ~hAbcGeomExport();

		/// Called at the beginning of rendering to perform any intialization 
		/// necessary.
		/// @param  nframes     Number of frames being rendered.
		/// @param  s           Start time, in seconds.
		/// @param  e           End time, in seconds.
		/// @return             True of success, false on failure (aborts the render).
		virtual int startRender( int nframes, float s, float e );

		/// Called once for every frame that is rendered.
		/// @param  time        The time to render at.
		/// @param  boss        Interrupt handler.
		/// @return             Return a status code indicating whether to abort the
		///                     render, continue, or retry the current frame.
		virtual ROP_RENDER_CODE renderFrame( float time, UT_Interrupt * boss );

		/// Called after the rendering is done to perform any post-rendering steps
		/// required.
		/// @return             Return a status code indicating whether to abort the
		///                     render, continue, or retry.
		virtual ROP_RENDER_CODE endRender();

	public:
		/// A convenience method to evaluate our custom file parameter.

		inline void get_str_parm( char const *parm, float t, UT_String & out )
		{
			//evalString(out, parm, &ifdIndirect[0], 0, (float)t);
			evalString(out, parm, 0, (float)t);
		}

		inline void OUTPUT(UT_String & str, float t)
		{
			STR_PARM("file", 0, 0, t)
		}

	private:
		static int *			ifdIndirect;

	private:
		bool				export_geom(
							char const *sopname,
							SOP_Node *sop,
							float time
						);

	private:

		float				_start_time;
		float				_end_time;
		int				_num_frames;

		std::string			_objpath;
		std::string			_abcfile;

		Alembic::AbcGeom::OArchive *		_oarchive;
		Alembic::AbcGeom::TimeSamplingPtr	_ts;

		GeoObjects				_objs;
	};

}				// End HDK_Sample namespace


#undef STR_PARM
#undef STR_SET
#undef STR_GET

#endif


