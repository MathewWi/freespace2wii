/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/


#include "globalincs/pstypes.h"
#include "graphics/gropengl.h"
#include "graphics/gropenglstate.h"
#include "graphics/gropengllight.h"

#include <vector>

extern GLfloat GL_max_anisotropy;


opengl_state GL_state;


opengl_texture_state::~opengl_texture_state()
{
	if (units != NULL) {
		vm_free(units);
	}
}

extern "C" void wiipause();

void opengl_texture_state::init(GLint n_units)
{
	Assert( n_units > 0 );
	units = (opengl_texture_unit*) vm_malloc(n_units * sizeof(opengl_texture_unit));
	num_texture_units = n_units;

	for (int i = 0; i < num_texture_units; i++) {
		units[i].active = GL_FALSE;
		units[i].enabled = GL_FALSE;

		units[i].texture_target = GL_TEXTURE_2D;
		units[i].texture_id = 0;
	
		units[i].texgen_S = GL_FALSE;
		units[i].texgen_T = GL_FALSE;
		units[i].texgen_R = GL_FALSE;
		units[i].texgen_Q = GL_FALSE;

		units[i].texgen_mode_S = GL_EYE_LINEAR;
		units[i].texgen_mode_T = GL_EYE_LINEAR;
		units[i].texgen_mode_R = GL_EYE_LINEAR;
		units[i].texgen_mode_Q = GL_EYE_LINEAR;

		units[i].wrap_S = GL_REPEAT;
		units[i].wrap_T = GL_REPEAT;
		units[i].wrap_R = GL_REPEAT;
	
		units[i].mag_filter = GL_LINEAR;
		units[i].min_filter = GL_NEAREST_MIPMAP_LINEAR;
		units[i].max_level = 1000;

		units[i].aniso_filter = 1.0f;

		units[i].env_mode = GL_MODULATE;
		units[i].env_combine_rgb = GL_MODULATE;
		units[i].env_combine_alpha = GL_MODULATE;

		units[i].rgb_scale = 1.0f;
		units[i].alpha_scale = 1.0f;
	}
}

GLboolean opengl_texture_state::TexgenS(GLint state)
{
	GLboolean save_state = units[active_texture_unit].texgen_S;

	if ( !((state == -1) || (state == units[active_texture_unit].texgen_S)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_TEXTURE_GEN_S);
			units[active_texture_unit].texgen_S = GL_TRUE;
		} else {
			glDisable(GL_TEXTURE_GEN_S);
			units[active_texture_unit].texgen_S = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_texture_state::TexgenT(GLint state)
{
	GLboolean save_state = units[active_texture_unit].texgen_T;

	if ( !((state == -1) || (state == units[active_texture_unit].texgen_T)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_TEXTURE_GEN_T);
			units[active_texture_unit].texgen_T = GL_TRUE;
		} else {
			glDisable(GL_TEXTURE_GEN_T);
			units[active_texture_unit].texgen_T = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_texture_state::TexgenR(GLint state)
{
	GLboolean save_state = units[active_texture_unit].texgen_R;

	if ( !((state == -1) || (state == units[active_texture_unit].texgen_R)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_TEXTURE_GEN_R);
			units[active_texture_unit].texgen_R = GL_TRUE;
		} else {
			glDisable(GL_TEXTURE_GEN_R);
			units[active_texture_unit].texgen_R = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_texture_state::TexgenQ(GLint state)
{
	GLboolean save_state = units[active_texture_unit].texgen_Q;

	if ( !((state == -1) || (state == units[active_texture_unit].texgen_Q)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_TEXTURE_GEN_Q);
			units[active_texture_unit].texgen_Q = GL_TRUE;
		} else {
			glDisable(GL_TEXTURE_GEN_Q);
			units[active_texture_unit].texgen_Q = GL_FALSE;
		}
	}

	return save_state;
}

void opengl_texture_state::SetTarget(GLenum tex_target)
{
	if (units[active_texture_unit].texture_target != tex_target) {
		Disable();

		if (units[active_texture_unit].texture_id) {
			glBindTexture(units[active_texture_unit].texture_target, 0);
			units[active_texture_unit].texture_id = 0;
		}

		units[active_texture_unit].texture_target = tex_target;

		// reset modes, since those were only valid for the previous texture target
		SetMagFilter(GL_LINEAR);
		SetMinFilter(GL_NEAREST_MIPMAP_LINEAR);

		SetWrapS(GL_REPEAT);
		SetWrapT(GL_REPEAT);
		SetWrapR(GL_REPEAT);

		SetMaxLevel(1000);

		AnisoFilter(1.0f);
	}
}

void opengl_texture_state::SetActiveUnit(GLuint id)
{
	if (id >= (uint)num_texture_units) {
		Int3();
		id = 0;
	}

	vglActiveTextureARB(GL_TEXTURE0 + id);

	active_texture_unit = id;
}

void opengl_texture_state::Enable(GLuint tex_id)
{
	if ( units[active_texture_unit].active && (units[active_texture_unit].texture_id == tex_id) ) {
		return;
	}

	if ( !shader_mode && (active_texture_unit < (uint)GL_supported_texture_units) ) {
		glEnable( units[active_texture_unit].texture_target );
		units[active_texture_unit].enabled = GL_TRUE;
	}

	if (units[active_texture_unit].texture_id != tex_id) {
		glBindTexture(units[active_texture_unit].texture_target, tex_id);
		units[active_texture_unit].texture_id = tex_id;
	}

	units[active_texture_unit].active = GL_TRUE;
}

void opengl_texture_state::Disable()
{
	if ( !units[active_texture_unit].active ) {
		return;
	}

	if (units[active_texture_unit].enabled) {
		glDisable( units[active_texture_unit].texture_target );
		units[active_texture_unit].enabled = GL_FALSE;
	}

	units[active_texture_unit].active = GL_FALSE;
}

void opengl_texture_state::DisableAll()
{
	for (int i = 0; i < num_texture_units; i++) {
		if (units[i].active) {
			SetActiveUnit(i);
			Disable();
		}
	}

	SetActiveUnit();

	Current_texture_source = TEXTURE_SOURCE_NONE;
}

void opengl_texture_state::Delete(GLuint tex_id)
{
	if (tex_id == 0) {
		Int3();
		return;
	}

	for (int i = 0; i < num_texture_units; i++) {
		if (units[i].texture_id == tex_id) {
		//	glBindTexture(units[i].texture_target, 0);
			glDisable( units[i].texture_target );

			units[i].enabled = GL_FALSE;
			units[i].active = GL_FALSE;
			units[i].texture_id = 0;
			units[i].texture_target = GL_TEXTURE_2D;

		//	if (i == active_texture_unit) {
		//		SetActiveUnit();
		//	}
		}
	}
}

GLfloat opengl_texture_state::AnisoFilter(GLfloat aniso)
{
	GLfloat rval = units[active_texture_unit].aniso_filter;

	if ( (aniso > 0.0f) /*&& (aniso != rval)*/ ) {
		if ( Is_Extension_Enabled(OGL_EXT_TEXTURE_FILTER_ANISOTROPIC) ) {
			units[active_texture_unit].aniso_filter = aniso;
			CLAMP(units[active_texture_unit].aniso_filter, 1.0f, GL_max_anisotropy);
	
			glTexParameterf(units[active_texture_unit].texture_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, units[active_texture_unit].aniso_filter);
		}

		if ( Is_Extension_Enabled(OGL_EXT_TEXTURE_LOD_BIAS) ) {
			if (units[active_texture_unit].aniso_filter > 1.0f) {
				glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 0.0f);
			} else {
				glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, -0.75f);
			}
		}
	}

	return rval;
}

opengl_state::~opengl_state()
{
	if (light_Status != NULL) {
		vm_free(light_Status);
	}
}

void opengl_state::init()
{
	int i;

	glDisable(GL_FOG);
	fog_Status = GL_FALSE;

	glDisable(GL_BLEND);
	blend_Status = GL_FALSE;

	glDisable(GL_ALPHA_TEST);
	alphatest_Status = GL_FALSE;

	glDisable(GL_DEPTH_TEST);
	depthtest_Status = GL_FALSE;

	glDisable(GL_SCISSOR_TEST);
	scissortest_Status = GL_FALSE;

	glDisable(GL_CULL_FACE);
	cullface_Status = GL_FALSE;

	glDisable(GL_POLYGON_OFFSET_FILL);
	polygonoffsetfill_Status = GL_FALSE;

	glDisable(GL_NORMALIZE);
	normalize_Status = GL_FALSE;

	for (i = 0; i < (int)(sizeof(clipplane_Status) / sizeof(GLboolean)); i++) {
		glDisable(GL_CLIP_PLANE0+i);
		clipplane_Status[i] = GL_FALSE;
	}

	Assert( GL_max_lights > 0 );
	light_Status = (GLboolean*) vm_malloc(GL_max_lights * sizeof(GLboolean));

	for (i = 0; i < GL_max_lights; i++) {
		glDisable(GL_LIGHT0+i);
		light_Status[i] = GL_FALSE;
	}

	glDepthMask(GL_FALSE);
	depthmask_Status = GL_FALSE;

	glDisable(GL_LIGHTING);
	lighting_Status = GL_FALSE;

	glFrontFace(GL_CCW);
	frontface_Value = GL_CCW;

	glCullFace(GL_BACK);
	cullface_Value = GL_BACK;

	glBlendFunc(GL_ONE, GL_ZERO);
	blendfunc_Value[0] = GL_ONE;
	blendfunc_Value[1] = GL_ZERO;

	glDepthFunc(GL_LESS);
	depthfunc_Value = GL_LESS;

	Current_alpha_blend_mode = ALPHA_BLEND_NONE;
	Current_zbuffer_type = ZBUFFER_TYPE_READ;
}

GLboolean opengl_state::Lighting(GLint state)
{
	GLboolean save_state = lighting_Status;

	if ( !((state == -1) || (state == lighting_Status)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_LIGHTING);
			lighting_Status = GL_TRUE;
		} else {
			glDisable(GL_LIGHTING);
			lighting_Status = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_state::Fog(GLint state)
{
	GLboolean save_state = fog_Status;

	if ( !((state == -1) || (state == fog_Status)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_FOG);
			fog_Status = GL_TRUE;
		} else {
			glDisable(GL_FOG);
			fog_Status = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_state::Blend(GLint state)
{
	GLboolean save_state = blend_Status;

	if ( !((state == -1) || (state == blend_Status)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_BLEND);
			blend_Status = GL_TRUE;
		} else {
			glDisable(GL_BLEND);
			blend_Status = GL_FALSE;
		}

		Current_alpha_blend_mode = (gr_alpha_blend)(-1);
	}

	return save_state;
}

GLboolean opengl_state::AlphaTest(GLint state)
{
	GLboolean save_state = alphatest_Status;

	if ( !((state == -1) || (state == alphatest_Status)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_ALPHA_TEST);
			alphatest_Status = GL_TRUE;
		} else {
			glDisable(GL_ALPHA_TEST);
			alphatest_Status = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_state::DepthTest(GLint state)
{
	GLboolean save_state = depthtest_Status;

	if ( !((state == -1) || (state == depthtest_Status)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_DEPTH_TEST);
			depthtest_Status = GL_TRUE;
		} else {
			glDisable(GL_DEPTH_TEST);
			depthtest_Status = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_state::ScissorTest(GLint state)
{
	GLboolean save_state = scissortest_Status;

	if ( !((state == -1) || (state == scissortest_Status)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_SCISSOR_TEST);
			scissortest_Status = GL_TRUE;
		} else {
			glDisable(GL_SCISSOR_TEST);
			scissortest_Status = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_state::CullFace(GLint state)
{
	GLboolean save_state = cullface_Status;

	if ( !((state == -1) || (state == cullface_Status)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_CULL_FACE);
			cullface_Status = GL_TRUE;
		} else {
			glDisable(GL_CULL_FACE);
			cullface_Status = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_state::PolygonOffsetFill(GLint state)
{
	GLboolean save_state = polygonoffsetfill_Status;

	if ( !((state == -1) || (state == polygonoffsetfill_Status)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_POLYGON_OFFSET_FILL);
			polygonoffsetfill_Status = GL_TRUE;
		} else {
			glDisable(GL_POLYGON_OFFSET_FILL);
			polygonoffsetfill_Status = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_state::Normalize(GLint state)
{
	GLboolean save_state = normalize_Status;

	if ( !((state == -1) || (state == normalize_Status)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_NORMALIZE);
			normalize_Status = GL_TRUE;
		} else {
			glDisable(GL_NORMALIZE);
			normalize_Status = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_state::Light(GLint num, GLint state)
{
	Assert( (light_Status != NULL) && (num >= 0) && (num < GL_max_lights) );

	GLboolean save_state = light_Status[num];

	if ( !((state == -1) || (state == light_Status[num])) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_LIGHT0+num);
			light_Status[num] = GL_TRUE;
		} else {
			glDisable(GL_LIGHT0+num);
			light_Status[num] = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_state::ClipPlane(GLint num, GLint state)
{
	Assert( (num >= 0) || (num < (int)(sizeof(clipplane_Status) / sizeof(GLboolean))) );

	GLboolean save_state = clipplane_Status[num];

	if ( !((state == -1) || (state == clipplane_Status[num])) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glEnable(GL_CLIP_PLANE0+num);
			clipplane_Status[num] = GL_TRUE;
		} else {
			glDisable(GL_CLIP_PLANE0+num);
			clipplane_Status[num] = GL_FALSE;
		}
	}

	return save_state;
}

GLboolean opengl_state::DepthMask(GLint state)
{
	GLboolean save_state = depthmask_Status;

	if ( !((state == -1) || (state == depthmask_Status)) ) {
		if (state) {
			Assert( state == GL_TRUE );
			glDepthMask(GL_TRUE);
			depthmask_Status = GL_TRUE;
		} else {
			glDepthMask(GL_FALSE);
			depthmask_Status = GL_FALSE;
		}

		Current_zbuffer_type = (gr_zbuffer_type)(-1);
	}

	return save_state;
}

void opengl_state::SetTextureSource(gr_texture_source ts)
{
	if (ts == Texture.Current_texture_source) {
		return;
	}

	switch (ts) {
		case TEXTURE_SOURCE_DECAL:
			GL_state.Texture.SetActiveUnit();
			GL_state.Texture.SetMinFilter(GL_LINEAR);
			GL_state.Texture.SetMagFilter(GL_LINEAR);
			GL_state.Texture.SetEnvMode(GL_MODULATE);
			break;

		case TEXTURE_SOURCE_NO_FILTERING:
			GL_state.Texture.SetActiveUnit();
			GL_state.Texture.SetMinFilter(GL_NEAREST);
			GL_state.Texture.SetMagFilter(GL_LINEAR);
			GL_state.Texture.SetEnvMode(GL_MODULATE);
			break;

		case TEXTURE_SOURCE_NONE:
			GL_state.Texture.DisableAll();
			break;

		default:
			Int3();
			break;
	}

	Texture.Current_texture_source = ts;
}

void opengl_state::SetAlphaBlendMode(gr_alpha_blend ab)
{
	if (ab == Current_alpha_blend_mode) {
		return;
	}

	switch (ab) {
		case ALPHA_BLEND_ALPHA_BLEND_ALPHA:
			GL_state.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;

		case ALPHA_BLEND_NONE:
			GL_state.BlendFunc(GL_ONE, GL_ZERO);
			break;

		case ALPHA_BLEND_ADDITIVE:
			GL_state.BlendFunc(GL_ONE, GL_ONE);
			break;

		case ALPHA_BLEND_ALPHA_ADDITIVE:
			GL_state.BlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;

		case ALPHA_BLEND_ALPHA_BLEND_SRC_COLOR:
			GL_state.BlendFunc(/*GL_SRC_COLOR*/GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
			break;

		default:
			break;
	}

	GL_state.Blend( (ab == ALPHA_BLEND_NONE) ? GL_FALSE : GL_TRUE );

	Current_alpha_blend_mode = ab;
}

void opengl_state::SetZbufferType(gr_zbuffer_type zt)
{
	if (zt == Current_zbuffer_type) {
		return;
	}

	switch (zt) {
		case ZBUFFER_TYPE_NONE:
			GL_state.DepthFunc(GL_ALWAYS);
			GL_state.DepthMask(GL_FALSE);
			break;

		case ZBUFFER_TYPE_READ:
			GL_state.DepthFunc(GL_LESS);
			GL_state.DepthMask(GL_FALSE);
			break;

		case ZBUFFER_TYPE_WRITE:
			GL_state.DepthFunc(GL_ALWAYS);
			GL_state.DepthMask(GL_TRUE);
			break;

		case ZBUFFER_TYPE_FULL:
			GL_state.DepthFunc(GL_LESS);
			GL_state.DepthMask(GL_TRUE);
			break;

		default:
			break;
	}

	GL_state.DepthTest( (zt == ZBUFFER_TYPE_NONE) ? GL_FALSE : GL_TRUE );

	Current_zbuffer_type = zt;
}


void opengl_setup_render_states(int &r, int &g, int &b, int &alpha, int &tmap_type, int flags, int is_scaler)
{
	gr_texture_source texture_source = (gr_texture_source)(-1);
	gr_alpha_blend alpha_blend = (gr_alpha_blend)(-1);
	gr_zbuffer_type zbuffer_type = (gr_zbuffer_type)(-1);
	
	if (gr_zbuffering) {
		if ( is_scaler || (gr_screen.current_alphablend_mode == GR_ALPHABLEND_FILTER) ) {
			zbuffer_type = ZBUFFER_TYPE_READ;
		} else {
			zbuffer_type = ZBUFFER_TYPE_FULL;
		}
	} else {
		zbuffer_type = ZBUFFER_TYPE_NONE;
	}

	tmap_type = TCACHE_TYPE_NORMAL;

	if (flags & TMAP_FLAG_TEXTURED) {
		r = g = b = 255;
	} else {
		r = gr_screen.current_color.red;
		g = gr_screen.current_color.green;
		b = gr_screen.current_color.blue;
	}

	if ( gr_screen.current_alphablend_mode == GR_ALPHABLEND_FILTER ) {
		if ( (gr_screen.current_bitmap >= 0) && bm_has_alpha_channel(gr_screen.current_bitmap) ) {
			tmap_type = TCACHE_TYPE_XPARENT;

			alpha_blend = ALPHA_BLEND_ALPHA_BLEND_ALPHA;

			// Blend with screen pixel using src*alpha+dst
			float factor = gr_screen.current_alpha;

			if (factor >= 1.0f) {
				alpha = 255;
			} else {
				alpha = fl2i(gr_screen.current_alpha*255.0f);
			}
		} else {
			tmap_type = TCACHE_TYPE_NORMAL;
			alpha_blend = ALPHA_BLEND_ADDITIVE;	// ALPHA_BLEND_ALPHA_ADDITIVE;

			// Blend with screen pixel using src*alpha+dst
			float factor = gr_screen.current_alpha;

			alpha = 255;

			if (factor < 1.0f) {
				r = fl2i(r * gr_screen.current_alpha);
				g = fl2i(g * gr_screen.current_alpha);
				b = fl2i(b * gr_screen.current_alpha);
			}
		}
	} else {
		alpha_blend = ALPHA_BLEND_ALPHA_BLEND_ALPHA;
		alpha = fl2i(gr_screen.current_alpha * 255.0f);
	}

	
	if (flags & TMAP_FLAG_TEXTURED) {
		// use nonfiltered textures for interface graphics
		if (flags & TMAP_FLAG_INTERFACE) {
			tmap_type = TCACHE_TYPE_INTERFACE;
			texture_source = TEXTURE_SOURCE_NO_FILTERING;
		} else {
			texture_source = TEXTURE_SOURCE_DECAL;
		}
	} else {
		texture_source = TEXTURE_SOURCE_NONE;
	}

	GL_state.SetTextureSource(texture_source);
	GL_state.SetAlphaBlendMode(alpha_blend);
	GL_state.SetZbufferType(zbuffer_type);
}
