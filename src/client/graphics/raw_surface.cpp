/***************************************************************************
  raw_surface.cpp  -  used to store the lowlevel graphics rendering routines
  --------------------------------------------------------------------------
  begin                : 2000-01-03
  by                   : Flemming Frandsen
  email                : dion@swamp.dk

  last changed         : 2004-01-25
  by                   : Christoph Brill
  email                : egore@gmx.de

  last changed         : 2004-09-18
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : added logmsg.h header.

    copyright            : (C) 2000 by Flemming Frandsen
    email                : dion@swamp.dk

 ***************************************************************************/

/***************************************************************************
  Note that pixels are ALWAYS 32 bit and in the NiL format
 ***************************************************************************/

#include "client/graphics/raw_surface.h"
#include "common/misc.h"
#include "common/console/logmsg.h"
#include "common/nil_math.h"

//! ???
struct AlphaTable {
	//! 2 dimensional array
	int32 tab[16][256];
	//! Constructor
	AlphaTable() {
		int32 alpha,val;
		for (alpha=0;alpha<16;alpha++) {
			for (val=0;val<256;val++) {
				tab[alpha][val]=(val*alpha+0x7)/0xf;
			}
		}
	}
	/*! inline prescale function
	    @param src Source pixel*/
	inline int32 prescale(int32 src) const {
		int32 alpha=GETALPHA(src);
		return PIXEL(tab[alpha][GETRED(src)], tab[alpha][GETGREEN(src)], tab[alpha][ GETBLUE(src)]) | GETFLAG(src);
	}
	/*! prescale buffer funtion
	    @param src Source pixel
	    @param count Count */
	void prescale_buffer(int32 *src, int count) const {
		while(count--) {
			*src++=prescale(*src);
		}
	}
	/*! Blending of prescaled
	    @param alpha Alpha channel value
	    @param src Source
	    @param tgt Target */
	inline int32 blend_prescaled_source(int32 alpha,int32 src,int32 tgt) const {
		return
			(src&PF_RGB_BITS)+
			PIXEL(tab[15-alpha][GETRED(tgt)], tab[15-alpha][GETGREEN(tgt)], tab[15-alpha][ GETBLUE(tgt)]);
	}
	/*! Blending function
	    @param src Source 
	    @param tgt Target */
	inline int32 blend(int32 src, int32 tgt) const {
		int32 alpha=GETALPHA(src);
		return PIXEL(tab[alpha][GETRED(src)]+tab[15-alpha][GETRED(tgt)], tab[alpha][GETGREEN(src)]+tab[15-alpha][GETGREEN(tgt)], tab[alpha][ GETBLUE(src)]+tab[15-alpha][ GETBLUE(tgt)]);
	}
};

static AlphaTable s_alpha;

//! ???
struct BlendFuncSRC_A {
	/*! ???
	    @param srcA ???
	    @param srcB ??? */
	inline int32 blend(int32 srcA,int32 srcB) const {
		return srcA;
	}
};

//! ???
struct BlendFuncSRC_B {
	/*! ???
	    @param srcA ???
	    @param srcB ??? */
	inline int32 blend(int32 srcA,int32 srcB) const {
		return srcB;
	}
};

//! ???
struct BlendFuncRED {
	/*! ???
	    @param srcA ???
	    @param srcB ??? */
	inline int32 blend(int32 srcA,int32 srcB) const {
		return 0x00ff0000;
	}
};

//! ???
struct BlendFuncAverage {
	/*! ???
	    @param srcA ???
	    @param srcB ??? */
	inline int32 blend(int32 srcA, int32 srcB) const {
		return ((srcA>>1)&0x7f7f7f)+((srcB>>1)&0x7f7f7f);
	}
};

//! ???
struct BlendFuncSaturatedAdd {
	/*! ???
	    @param srcA ???
	    @param srcB ??? */
	inline int32 blend(int32 srcA, int32 srcB) const {
		int32 sum,carry,mask;
		sum=srcA+srcB;
		carry=sum^srcA^srcB;
		sum=sum-((carry&0x1010100)&~((carry>>8)&0x1010100));
		mask=(carry>>1)&0x80808080;
		return sum|mask|(mask-(mask>>7));
	}
};

//! ???
struct BlendFuncAlphaSaturatedAdd {
	/*! ???
	    @param srcA ???
	    @param srcB ??? */
	inline int32 blend(int32 srcA, int32 srcB) const {
		if (GETALPHA(srcA)) {
			int32 sum,carry,mask;

			srcA=s_alpha.prescale(srcA);
			sum=srcA+srcB;
			carry=sum^srcA^srcB;
			sum=sum-((carry&0x1010100)&~((carry>>8)&0x1010100));
			mask=(carry>>1)&0x80808080;
			return sum|mask|(mask-(mask>>7));
		} else {
			return srcB;
		}
	}
};

//! ???
struct BlendFuncAlphaView {
	/*! ???
	    @param srcA ???
	    @param srcB ??? */
	inline int32 blend(int32 srcA, int32 srcB) const {
		if (GETALPHA(srcA)) {
			return s_alpha.blend(srcA,srcB) | PF_TYPE_EARTH;
		} else {
			return srcB;
		}
	}
};

//! ???
struct BlendFuncModifyType {
	/*! ???
	    @param srcA ???
	    @param srcB ??? */
	inline int32 blend(int32 srcA, int32 srcB) const {
		return (srcA & PF_TYPE_BITS) | (srcB & ~PF_TYPE_BITS);
	}
};

//! ???
struct BlendFuncAlphaSrcAType {
	/*! ???
	    @param srcA ???
	    @param srcB ??? */
	inline int32 blend(int32 srcA, int32 srcB) const {
		if (GETALPHA(srcA)) {
			return s_alpha.blend(srcA,srcB) | (srcA | PF_TYPE_BITS);
		} else {
			return srcB;
		}
	}
};

//! blending type to show only a shadow (Uwe)
struct BlendFuncShadowOnly {
	/*! ???
	    @param srcA ???
	    @param srcB ??? */
	inline int32 blend( int32 srcA, int32 srcB ) const {
		if (GETALPHA(srcA)) {
			return s_alpha.blend(srcA & 0x40ffffff,srcB) | PF_TYPE_EARTH;
		} else {
			return srcB;
		}
	}
};

//! 3 way blending functions.
struct BlendFunc3Map {
	/*! ???
	    @param srcA ???
	    @param srcB ???
	    @param srcC ??? */
	inline int32 blend3(int32 srcA, int32 srcB, int32 srcC) const {
		int32 type;
		if (!IS_STONE(srcC)) {
			if (IS_HOLE(srcA)) {
				return srcB;
			}
			if (GETALPHA(srcA)) {
				if (IS_HOLE(srcA) || IS_EARTH(srcA) || IS_STONE(srcA)) {
					type = srcA & PF_TYPE_BITS;
				} else {
					type = srcC & PF_TYPE_BITS;
				}
				if (IS_EARTH(type) || IS_STONE(type)) {
					return s_alpha.blend(srcA,srcC)|type|GETALPHA(srcA);
				}
			}
		}
		return srcC;
	}
};

// Blending function instansiations.
static BlendFuncSRC_A             s_copy;
static BlendFuncAlphaView         s_alpha_view;
static BlendFuncShadowOnly        s_shadow_only;
static BlendFuncAlphaSaturatedAdd s_alpha_add;
static BlendFuncSaturatedAdd      s_add;
static BlendFuncAverage           s_average;
static BlendFunc3Map              s_map;
//static BlendFuncSRC_B             s_null;
//static BlendFuncRED               s_red;
//static BlendFuncAlphaSrcAType     s_alpha_keep_type;
//static BlendFuncModifyType        s_modify_type;

//! Bilinear interpolation.
inline int32 blend(int32 a, int32 b, int32 c, int32 d, int u, int v) {
	// a b
	// c d
	int32 pixel,pixel2;
	int ca=((256-u)*(256-v))>>8;
	int cb=(u*(256-v))>>8;
	int cc=((256-u)*v)>>8;
	int cd=(u*v)>>8;
	pixel =((a&0xff00ff)*ca>>8)&0xff00ff;
	pixel+=((b&0xff00ff)*cb>>8)&0xff00ff;
	pixel+=((c&0xff00ff)*cc>>8)&0xff00ff;
	pixel+=((d&0xff00ff)*cd>>8)&0xff00ff;
	pixel2 =((a&0xff00)*ca>>8)&0xff00;
	pixel2+=((b&0xff00)*cb>>8)&0xff00;
	pixel2+=((c&0xff00)*cc>>8)&0xff00;
	pixel2+=((d&0xff00)*cd>>8)&0xff00;
	return pixel|pixel2;
}

//! Bilinear stretch the source bitmap and blend onto the target bitmap using the supplied blending function.
template<class BlendFunc>
void draw_stretch(const Raw_surface *source, Mutable_raw_surface *target, int source_xpos, int source_ypos, int source_xsize, int source_ysize, int target_xpos, int target_ypos, int target_xsize, int target_ysize, const BlendFunc &bf) {
	int x_ratio,y_ratio;
	x_ratio=(source_xsize<<8)/target_xsize;
	y_ratio=(source_ysize<<8)/target_ysize;

	if (target_xpos>=target->_get_xsize() ||
		 target_ypos>=target->_get_ysize() ||
		 target_xpos+target_xsize<0 ||
		 target_ypos+target_ysize<0) {
		return;
	}

	int x_base,y_base;
	x_base=0;
	y_base=0;
	if (target_xpos<0) {
		x_base=x_ratio*-target_xpos;
		target_xsize+=target_xpos;
		target_xpos=0;
	}
	if (target_ypos<0) {
		y_base=y_ratio*-target_ypos;
		target_ysize+=target_ypos;
		target_ypos=0;
	}
	if (target_xsize+target_xpos>target->_get_xsize()) {
		target_xsize=target->_get_xsize()-target_xpos;
	}
	if (target_ysize+target_ypos>target->_get_ysize()) {
		target_ysize=target->_get_ysize()-target_ypos;
	}

	int x_coord,y_coord;
	int x,y;
	int32 *tgt,*t;
	const int32 *src,*s1,*s2;

	src=source->get_pixels_read();
	tgt=target->_get_pixels_write()+target_xpos+target_ypos*target->_get_xsize();

	for (y=0,y_coord=y_base;y<target_ysize;y++) {
		t=tgt;
		s1=src+(y_coord>>8)*source->get_xsize();
		s2=s1+source->get_xsize();
		for (x=0,x_coord=x_base;x<target_xsize;x++) {
			*t++=bf.blend(blend(s1[x_coord>>8],s1[(x_coord>>8)+1],
									  s2[x_coord>>8],s2[(x_coord>>8)+1],
									  x_coord&0xff,y_coord&0xff),
							  *t);
			x_coord+=x_ratio;
		}
		y_coord+=y_ratio;
		tgt+=target->_get_xsize();
	}
}

/*! Copy the source bitmap to the target bitmap at the specified position, using the 3 way
    blending function supplied. The three arguments to the blending function are:
    1) source (source coordinates),
    2) bgmap (target coordinates),
    3) target (target coordinates) */
template<class BlendFunc3>
void draw3(const Raw_surface *source, Mutable_raw_surface *target_bgmap, Mutable_raw_surface *target_map, int xpos, int ypos, const BlendFunc3 &bf) {
	int32 *target_line = target_map->_get_pixels_write();
	int32 *bgmap_line = target_bgmap->_get_pixels_write();
	const int32 *source_line = source->get_pixels_read();
	if (!target_line || !bgmap_line || !source_line) {
		return;
	}
	//clipping variables:
	int clipped_xstart = 0;
	int clipped_ystart = 0;
	if (xpos < 0)	clipped_xstart = -xpos;
	if (ypos < 0)	clipped_ystart = -ypos;
	int clipped_xsize = source->get_xsize()-clipped_xstart;
	int clipped_ysize = source->get_ysize()-clipped_ystart;
	//just a bit faster than using raw_target->_get_xsize() each time.
	int xsize_source = source->get_xsize();
	int xsize_target = target_map->_get_xsize();
	if (xpos+xsize_source >= xsize_target) {
		clipped_xsize -= (xpos+xsize_source-xsize_target);
	}
	if (ypos+source->get_ysize() >= target_map->_get_ysize()) {
		clipped_ysize -= (ypos+source->get_ysize()-target_map->_get_ysize());
	}
	target_line+=xpos+clipped_xstart+(ypos+clipped_ystart)*xsize_target;
	bgmap_line+= xpos+clipped_xstart+(ypos+clipped_ystart)*xsize_target;
	source_line+=clipped_ystart*xsize_source+clipped_xstart;
	for (int yc=0; yc < clipped_ysize; yc++) {
		int32 *target = target_line;
		const int32 *source = source_line;
		int32 *bgmap = bgmap_line;
		for (int xc=0; xc < clipped_xsize; xc++) {
			*target=bf.blend3(*source,*bgmap,*target);
			target++;
			source++;
			bgmap++;
		}
		target_line += xsize_target;
		bgmap_line += xsize_target;
		source_line += xsize_source;
	}
}

//! blend the source bitmap onto the target bitmap at the location specified.
template <class BlendFunc>
void draw(const Raw_surface *source, Mutable_raw_surface *target, int xpos, int ypos, const BlendFunc &bf) {
	int32 *target_line = target->_get_pixels_write();
	const int32 *source_line=source->get_pixels_read();

	if (!target_line || !source_line) {
		return;
	}

	//clipping variables:
	int clipped_xstart = 0;
	int clipped_ystart = 0;

	if (xpos < 0)	clipped_xstart = -xpos;
	if (ypos < 0)	clipped_ystart = -ypos;

	int clipped_xsize = source->get_xsize()-clipped_xstart;
	int clipped_ysize = source->get_ysize()-clipped_ystart;

	int xsize_source = source->get_xsize();  //just a bit faster than using source->get_xsize() each time.
	int xsize_target = target->_get_xsize(); //just a bit faster than using target->_get_xsize() each time.
	if (xpos+xsize_source >= xsize_target) {
		clipped_xsize -= (xpos+xsize_source-xsize_target);
	}
	if (ypos+source->get_ysize() >= target->_get_ysize()) {
		clipped_ysize -= (ypos+source->get_ysize()-target->_get_ysize());
	}

	source_line+=clipped_ystart*xsize_source+ clipped_xstart;
	target_line+=xpos+clipped_xstart+(ypos+clipped_ystart)*xsize_target;

	for (int yc=0; yc < clipped_ysize; yc++) {
		int32 *target = target_line;
		const int32 *source = source_line;
		for (int xc=0; xc < clipped_xsize; xc++) {
			// old simultaneous blend & pointer increase seems to confuse gcc3?
			*target=bf.blend(*source,*target); 
			target++; 
			source++;
		}
		target_line += xsize_target;
		source_line += xsize_source;
	}
}

/*! Stretch the source bitmap and blend using integer magnification and no interpolation onto
    the target bitmap using the supplied blending function. */
template <class BlendFunc>
void draw_stretch(const Raw_surface *source, Mutable_raw_surface *target, int source_xpos,  int source_ypos, int source_xsize, int source_ysize, int target_xpos,  int target_ypos, int enlargement, const BlendFunc &bf) {
	int32 *target_pixel = target->_get_pixels_write();
	const int32 *source_pixel = source->get_pixels_read();
	if (!target_pixel || !source_pixel) {
		return;
	}

	int t_xsize = target->_get_xsize();
	int s_xsize = source->get_xsize();

	target_pixel+=source_xpos+ source_ypos*t_xsize;
	source_pixel+=target_xpos+target_ypos*s_xsize;

	if (enlargement == 1) {
		int bytes_pr_line = source_xsize*sizeof(int32);
		for(int y=0;y<source_ysize;y++) {
			for (int x=0;x<source_xsize;x++) {
				*target_pixel=bf.blend(*source_pixel,*target_pixel);
				target_pixel++; 
				source_pixel++;
			}
			target_pixel+=t_xsize-source_xsize;
			source_pixel+=s_xsize-source_xsize;
		}
	} else if (enlargement == 2) {
		for(int y=0;y<source_ysize;y++) {
			for(int x=0;x<source_xsize;x++) {
				int32 pixel=*source_pixel;
				*target_pixel++ = bf.blend(pixel,*target_pixel);
				*target_pixel++ = bf.blend(pixel,*target_pixel);
				source_pixel++;
				target_pixel[t_xsize-2] = bf.blend(pixel,target_pixel[t_xsize-2]);
				target_pixel[t_xsize-1] = bf.blend(pixel,target_pixel[t_xsize-1]);
			}
			target_pixel+=(t_xsize<<1)-(source_xsize<<1);
			source_pixel+=s_xsize-source_xsize;
		}
	} else {
		for(int y=0;y<source_ysize;y++) {
			for (int e=0;e<enlargement;e++) {
				const int32 *sp = source_pixel;
				int32 *tp = target_pixel;
				for(int x=0;x<source_xsize;x++) {
					switch (enlargement) {
					case 7: *tp++=bf.blend(*sp,*tp);
					case 6: *tp++=bf.blend(*sp,*tp);
					case 5: *tp++=bf.blend(*sp,*tp);
					case 4: *tp++=bf.blend(*sp,*tp);
					case 3: *tp++=bf.blend(*sp,*tp);
					case 2: *tp++=bf.blend(*sp,*tp);
					default:
					case 1: *tp++=bf.blend(*sp,*tp);
					}
					sp++;
				}
				target_pixel+=t_xsize;
			}
			source_pixel+=s_xsize;
		}
	}
}

//! specialisation for the straight copy case, where we can cut down on reads from the target bitmap.
void draw_stretch(const Raw_surface *source, Mutable_raw_surface *target, int source_xpos,  int source_ypos, int source_xsize, int source_ysize, int target_xpos,  int target_ypos, int enlargement, const BlendFuncSRC_A &bf) {
	int32 *target_pixel = target->_get_pixels_write();
	const int32 *source_pixel = source->get_pixels_read();
	if (!target_pixel || !source_pixel) {
		return;
	}
	int t_xsize = target->_get_xsize();
	int s_xsize = source->get_xsize();
	target_pixel+=target_xpos+ target_ypos*t_xsize;
	source_pixel+=source_xpos+source_ypos*s_xsize;
	if (enlargement == 1) {
		int bytes_pr_line = source_xsize*sizeof(int32);
		for(int y=0;y<source_ysize;y++) {
			memcpy(target_pixel,source_pixel,bytes_pr_line);
			target_pixel+=t_xsize;
			source_pixel+=s_xsize;
		}
	} else if (enlargement == 2) {
		for(int y=0;y<source_ysize;y++) {
			for(int x=0;x<source_xsize;x++) {
				int32 pixel=*source_pixel;
				*target_pixel++ = pixel;
				*target_pixel++ = pixel;
				source_pixel++;
				target_pixel[t_xsize-2] = pixel;
				target_pixel[t_xsize-1] = pixel;
			}
			target_pixel+=(t_xsize<<1)-(source_xsize<<1);
			source_pixel+=s_xsize-source_xsize;
		}
	} else {
		for(int y=0;y<source_ysize;y++) {
			//! YYYY-MM-DD, @user: @reason
			// memcpy(target_pixel,source_pixel,bytes_pr_line);
			for (int e=0;e<enlargement;e++) {
				const int32 *sp = source_pixel;
				int32 *tp = target_pixel;
				for(int x=0;x<source_xsize;x++) {
					switch (enlargement) {
						case 7: *(tp++) = *sp;
						case 6: *(tp++) = *sp;
						case 5: *(tp++) = *sp;
						case 4: *(tp++) = *sp;
						case 3: *(tp++) = *sp;
						case 2: *(tp++) = *sp;
						default:
						case 1: *(tp++) = *sp;
					}
					sp++;
				}
				target_pixel+=t_xsize;
			}
			source_pixel+=s_xsize;
		}
	}
}

#define ERRORBITS 16
template <class BlendFunc>
void draw_line(Mutable_raw_surface *target, int x0, int y0, int x1, int y1, int32 color, unsigned int style, const BlendFunc &bf) {
	int32 *pixels;
	pixels=target->_get_pixels_write();
	if (!pixels) return;

	int xsize=target->_get_xsize();
	// 2004-01-25, egore: Commented out for clean compile with -Wall
	//int ysize=target->_get_ysize();

	if (!target->clip_line(x0,y0,x1,y1)) return;

	//get delta values
	int dx = x1-x0;
	int dy = y1-y0;

	//zero length is boring so get rid of it.
	if (dx == 0 && dy == 0) return;

	if (ABS(dx) > ABS(dy)) {
		// x major
	
		int ymove = xsize*SIGN(dy);          // The step to take with y when error hits 1
		dy = (ABS(dy) << ERRORBITS)/ABS(dx); // The value to add to error each x
		int e = 0;                           // error, fixed point with errorbits bits between 0 and 1.
		int x = x0;                          // x is just the x coordinate
		int y = y0*xsize;                    // y is offset into buffer, not the y coordinate (this saves many muls) 
		dx = SIGN(dx);                       // we just need the sign of dx
		int old_y = y;

		x1 += dx;
		while (x != x1) {

			//set the pixel
			pixels[x+y] = bf.blend(color,pixels[x+y]);

			//tally the error and ajust
			e += dy;
			if (e >= (1 << ERRORBITS)) {
				old_y = y;
				y += ymove;
				e -= (1 << ERRORBITS);
			}

			//move x one pixel in the right direction
			x += dx;
		} //while (x != x1)
	} else {
		//y major
		int xmove = SIGN(dx);                //The step to take with x when error hits 1 << ERRORBITS
		dx = (ABS(dx) << ERRORBITS)/ABS(dy); //The value to add to error each y
		int e = 0;                           //error, fixed point with ERRORBITS bits between 0 and 1.
		int x = x0;                          //x is just the x coordinate
		int y = y0*xsize;                    //y is offset into buffer, not the y coordinate (this saves many muls)
		int sign_dy = SIGN(dy);
		dy = xsize*sign_dy;                  //we just need the sign of dy scaled to one pixel up or down
		y1 = (y1+sign_dy)*xsize;
		int old_x = x;

		while (y != y1) {
			//set the color
			pixels[x+y] = bf.blend(color,pixels[x+y]);
			//tally the error and ajust
			e += dx;
			if (e >= (1 << ERRORBITS)) {
				old_x = x;
				x += xmove;
				e -= (1 << ERRORBITS);
			}
			//move y one pixel in the right direction
			y += dy;
		}
	}
}

template <class BlendFunc>
void draw_horizontal_line(Mutable_raw_surface *target, int x0, int y0, int x1, int32 color, const BlendFunc &bf) {
	int32 *pixels;
	pixels=target->_get_pixels_write();

	if (!pixels) return;

	//Make sure that the x'es are in the correct order:
	if (x0 > x1) {
		int tmp = x1;
		x1 = x0;
		x0 = tmp;
	}

	//Do some clipping:
	if (y0 < 0 || y0 >= target->_get_ysize()) return;
	x0 = CLAMP(x0,0,target->_get_xsize());
	x1 = CLAMP(x1,0,target->_get_xsize());
	if (x1 - x0 <= 0) return;

	//Then do the drawing:
	int32 *pixel = pixels+x0+y0*target->_get_xsize();

	for (int x=x0; x<x1; x++) {
		*pixel = bf.blend(color,*pixel);
		pixel++;
	}
}

template <class BlendFunc>
void draw_vertical_line(Mutable_raw_surface *target, int x0, int y0, int y1, int32 color, const BlendFunc &bf) {
	int32 *pixels;
	pixels=target->_get_pixels_write();
	if (!pixels) return;

	// Make sure that the y's are in the correct order:
	if (y0 > y1) {
		int tmp = y1;
		y1 = y0;
		y0 = tmp;
	}

	int xsize=target->_get_xsize();

	// Do some clipping:
	if( (x0 < 0) || (x0 >= xsize) ) return;
	y0 = CLAMP(y0,0,target->_get_ysize());
	y1 = CLAMP(y1,0,target->_get_ysize());
	if (y1-y0 <= 0) return;

	// Then do the drawing:
	int32 *pixel = pixels+x0+y0*xsize;

	for (int y=y0; y<y1; y++) {
		*pixel = bf.blend(color,*pixel);
		pixel += xsize;
	}
}

template <class BlendFunc>
void draw_rectangle(Mutable_raw_surface *target, int x0, int y0, int x1, int y1, int32 color, const BlendFunc &bf) {
	int32 *pixels;
	pixels=target->_get_pixels_write();
	if (!pixels) return;

	if (y0 > y1) {
		int tmp = y1;
		y1 = y0;
		y0 = tmp;
	}

	if (x0 > x1) {
		int tmp = x1;
		x1 = x0;
		x0 = tmp;
	}

	// Do some clipping:
	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x0 >= target->_get_xsize() ) return;
	if (y0 >= target->_get_ysize() ) return;

	if (x1 < 0) return;
	if (y1 < 0) return;
	if (x1 > target->_get_xsize()-1) x1 = target->_get_xsize()-1;
	if (y1 > target->_get_ysize()-1) y1 = target->_get_ysize()-1;

	int xcount = x1-x0;
	int ycount = y1-y0;

	//This is a speed hack so we favour horizontal lines
	if (xcount > ycount/2) {
		for(int y=0;y<ycount;y++)
			draw_horizontal_line(target,x0,y0+y,x0+xcount,color,bf);
	} else {
		for(int x=0;x<xcount;x++)
			draw_vertical_line(target,x0+x,y0,y0+ycount,color,bf);
	}
}

template <class BlendFunc>
void draw_rectangle_border(Mutable_raw_surface *target, int x0, int y0, int x1, int y1, int32 color, const BlendFunc &bf) {
	draw_horizontal_line(target,x0+1,y0,x1,color,bf);
	draw_horizontal_line(target,x0,y1,x1+1,color,bf);
	draw_vertical_line(target,x0,y0,y1,color,bf);
	draw_vertical_line(target,x1,y0,y1,color,bf);
}

#define CALL_ALL()                                  \
	do {                                             \
		switch (bf) {                                 \
		case COPY:          CALL(s_copy);      break; \
		case ALPHA:         CALL(s_alpha);     break; \
		case AVERAGE:       CALL(s_average);   break; \
		case SAT_ADD:       CALL(s_add);       break; \
		case ALPHA_SAT_ADD: CALL(s_alpha_add); break; \
		}                                             \
	} while(0)

Raw_surface::Raw_surface() {
	xsize = -1;
	ysize = -1;
}

Raw_surface::~Raw_surface() {}

void Raw_surface::draw_copy(Mutable_raw_surface *target, int source_xpos, int source_ypos,  int source_xsize, int source_ysize, int target_xpos, int target_ypos) const {
	//Copy the pixels one line at the time
	int t_xsize = target->_get_xsize();
	int bytes_pr_line = source_xsize*sizeof(int32);
	int32 *target_pixel = target->_get_pixels_write();
	const int32 *source_pixel = get_pixels_read();
	if (!source_pixel || !target_pixel) {
		return;
	}
	target_pixel+=target_xpos+target_ypos*t_xsize;
	source_pixel+=source_xpos+source_ypos*xsize;
	for(int y=0;y<source_ysize;y++) {
		memcpy(target_pixel,source_pixel,bytes_pr_line);
		target_pixel+=t_xsize;
		source_pixel+=xsize;
	}
}

void Raw_surface::draw_copy_stretch(Mutable_raw_surface *target, int source_xpos,  int source_ypos, int source_xsize, int source_ysize, int target_xpos,  int target_ypos, int enlargement) const {
	::draw_stretch(this,target, source_xpos, source_ypos, source_xsize, source_ysize, target_xpos, target_ypos, enlargement, s_copy);
}

void Raw_surface::draw_copy_stretch(Mutable_raw_surface *target, int source_xpos, int source_ypos, int source_xsize, int source_ysize, int target_xpos, int target_ypos, int target_xsize, int target_ysize, const Blend &bf) const {
#define CALL(_x) do{::draw_stretch(this, target, source_xpos,  source_ypos, source_xsize, source_ysize, target_xpos,  target_ypos, target_xsize, target_ysize, _x);}while(0)
	CALL_ALL();
#undef CALL
}

void Raw_surface::draw_alpha(Mutable_raw_surface *target, int xpos, int ypos) const {
	::draw(this,target,xpos,ypos,s_alpha);
}

void Raw_surface::draw_view(Mutable_raw_surface *target, int xpos, int ypos) const {
	xpos -= get_xsize() >> 1;
	ypos -= get_ysize() >> 1;
	::draw(this,target,xpos,ypos,s_alpha_view);
}

void  Raw_surface::draw_shadow_only(Mutable_raw_surface *target, int xpos, int ypos) const {
	xpos -= get_xsize() >> 1;
	ypos -= get_ysize() >> 1;
	::draw( this, target, xpos, ypos, s_shadow_only );
}

void Raw_surface::draw_map(Mutable_raw_surface *target_map, Mutable_raw_surface *target_bgmap, int xpos, int ypos) const {
	xpos -= get_xsize() >> 1;
	ypos -= get_ysize() >> 1;
	::draw3(this,target_bgmap,target_map,xpos,ypos,s_map);
}

bool Raw_surface::save_to_file(char *filename) {
	if (!get_pixels_read()) return false;

	FILE *f=fopen(filename, "wc");
	if (!f) return false;

	Rgbf_header header;
	header.magic = RGBF_HEADER_MAGIC;
	header.xsize = xsize;
	header.ysize = ysize;
	header.swap();

	if (fwrite(&header, sizeof(header), 1, f) != 1) {fclose(f); return false;}
	if (fwrite(get_pixels_read(),xsize*ysize*sizeof(int32),1,f) != 1) {fclose(f); return false;}

	fclose(f);

	return true;
}

bool Raw_surface::clip_line(int &x0,int &y0,int &x1,int &y1) const {
	if (xsize<0 || ysize<0) {
		return false;
	}
	Area_code code0 = area_code(x0,y0);
	Area_code code1 = area_code(x1,y1);
	while (code0 != AC_INSIDE || code1 != AC_INSIDE) {
		// While both ends are not inside the clipping region
		if (code0 & code1) {
			return false; // Trivial Reject (each end is off to the same side)
		} else {
			// Failed both tests, so calculate the line segment to clip
			Area_code codeout;
			if (code0)
				codeout = code0; // Clip the first point
			else
				codeout = code1; // Clip the last point
			int x=0;
			int y=0;
			if (codeout & AC_BOTTOM) {
				// Clip the line to the bottom of the viewport
				y = ysize-1;
				x = x0+(x1-x0)*(y-y0) / (y1 - y0);
			} else if (codeout & AC_TOP) {
				// Clip the line to the top of the viewport
				y = 0;
				x = x0+(x1-x0)*(y-y0) / (y1 - y0);
			} else if (codeout & AC_RIGHT) {
				// Clip the line to the right edge of the viewport
				x = xsize-1;
				y = y0+(y1-y0)*(x-x0) / (x1-x0);
			} else if (codeout & AC_LEFT) {
				// Clip the line to the left edge of the viewport
				x = 0;
				y = y0+(y1-y0)*(x-x0) / (x1-x0);
			}
			if (codeout == code0) {
				// Modify the first coordinate
				x0 = x; y0 = y;
				code0 = area_code(x0, y0);
			} else {
				//Modify the second coordinate
				x1 = x; y1 = y;
				code1 = area_code(x1, y1);
			}
		} // if (!(code0 & code1))
	} // while (code0 <> 0 || code1 <> 0) do {
	return true;
}

Raw_surface::Area_code Raw_surface::area_code(int x, int y) const {
	int code = AC_INSIDE;
	if (y > ysize-1)
		code = AC_BOTTOM;
	else if (y < 0)
		code = AC_TOP;
	if (x > xsize-1)
		code |= AC_RIGHT;
	else if (x < 0)
		code |= AC_LEFT;
	return (Area_code)code;
}

Mutable_raw_surface::Mutable_raw_surface() : Raw_surface() {
	pixels = NULL;
	attached = false;
}

Mutable_raw_surface::~Mutable_raw_surface() {
	free_pixels();
}

void Mutable_raw_surface::free_pixels() {
	// TODO - what if surface is attached???
	if (pixels && !attached) {
		free(pixels);
	}

	attached = false;
	xsize = -1;
	ysize = -1;
	pixels = NULL;
}

void Mutable_raw_surface::create(int xsize_, int ysize_) {
	free_pixels();

	if (!xsize_) return;
	if (!ysize_) return;

	xsize = xsize_;
	ysize = ysize_;
	pixels = (int32*)malloc(xsize*ysize*sizeof(int32));	
	attached = false;
}

void Mutable_raw_surface::create(const Raw_surface *original) {
	const int32 *to_copy;
	int xs,ys;
	create(xs=original->get_xsize(),ys=original->get_ysize());
	if (!(to_copy=original->get_pixels_read())) {
		logmsg(lt_error, "Could not get pixels of surface %p. might be optimised.", original);
		return;
	}
	memcpy(pixels,to_copy,xs*ys*sizeof(int32));
}

void Mutable_raw_surface::attach(int xsize_, int ysize_, int32 *pixels_) {
	free_pixels();
	
	if (!pixels_) return;
	if (!xsize_) return;
	if (!ysize_) return;
	
	xsize = xsize_;
	ysize = ysize_;
	pixels = pixels_;	
	attached = true;
}

bool Mutable_raw_surface::load_from_file(char *filename) {
	FILE *f = fopen(filename,"r");
	if (!f) {
		logmsg(lt_error, "Could not load %s.", filename);
		return false;
	}

	Rgbf_header header;
	if (fread(&header,sizeof(header),1,f) != 1) {
		logmsg(lt_error, "Could not read header for %s.", filename);
		fclose(f); 	
		return false;
	}
	header.swap();

	if (header.magic != RGBF_HEADER_MAGIC) {
		logmsg(lt_error, "%s is not an NiL rgbf file.", filename);
		fclose(f);
		return false;
	}

	create(header.xsize,header.ysize);

	if (fread(pixels,xsize*ysize*sizeof(int32),1,f) != 1) {
		logmsg(lt_error, "Unable to read data in %s.", filename);
		free_pixels();
		fclose(f);
		return false;
	}
	fclose(f);
	return true;
}

void Mutable_raw_surface::draw_line(int x0, int y0, int x1, int y1, int32 color, const Blend &bf, unsigned int style) {
#define CALL(_x) do {::draw_line(this,x0,y0,x1,y1,color,style,_x);}while(0)
	CALL_ALL();
#undef CALL
}

void Mutable_raw_surface::draw_horizontal_line(int x0,  int y0, int x1, int32 color, const Blend &bf) {
#define CALL(_x) do{::draw_horizontal_line(this,x0,y0,x1,color,_x);}while(0)
	CALL_ALL();
#undef CALL
}

void Mutable_raw_surface::draw_vertical_line(int x0, int y0, int y1, int32 color, const Blend &bf) {
#define CALL(_x) do{::draw_vertical_line(this,x0,y0,y1,color,_x);}while(0)
	CALL_ALL();
#undef CALL
}

void Mutable_raw_surface::draw_rectangle(int x0, int y0, int x1, int y1, int32 color, const Blend &bf) {
#define CALL(_x) do{::draw_rectangle(this,x0,y0,x1,y1,color,_x);}while(0)
	CALL_ALL();
#undef CALL
}

void Mutable_raw_surface::draw_rectangle_border(int x0, int y0, int x1, int y1, int32 color, const Blend &bf) {
#define CALL(_x) do{::draw_rectangle_border(this,x0,y0,x1,y1,color,_x);}while(0)
	CALL_ALL();
#undef CALL
}

void Mutable_raw_surface::apply_color_under_offset(int32 color, int base) {
	// pixel pointer
	int32 *s = _get_pixels_write();
	
	// quick ref colors
	int cr = GETRED(color);
	int cg = GETGREEN(color);
	int cb = GETBLUE(color);
	
	// for every pixel
	for (int y=0; y < ysize; y++) {
		for (int x=0; x < xsize; x++) {
				
			int a = GETALPHA(*s);
			if ( a > 0 ) {
				int r = GETRED(*s);
				int g = GETGREEN(*s);
				int b = GETBLUE(*s);
				
				if ( r == g && g == b ) {
					// grey, take as gun so leave alone
					// this might change ( weapon color? )
				} else {
					// take the average component color
					int o = r + g + b;
					o /= 3;
					
					// the average pixel color is taken to be base
					// want lighter colors to be + and
					// darker colors to be -
					o -= base;
					
					// apply this offset to the default color
					r = cr + o;
					g = cg + o;
					b = cb + o;
					
					// a few limit checks
					if ( r > 255 ) r = 255;
					if ( r < 0 ) r = 0;
					if ( g > 255 ) g = 255;
					if ( g < 0 ) g = 0;
					if ( b > 255 ) b = 255;
					if ( b < 0 ) b = 0;
					
					// and that's all done
					*s = (*s & 0x0F000000) | PIXELA(r,g,b,15);
				}
			}
			s++;
		}
	}
}
