#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/celiu.cpp"
#else

// To load this lib in LUA:
// require 'libceliu'

#ifdef LUAJIT
 extern "C" {
#include <luaT.h>
#include <TH.h>
 }
#else
#include <luaT.h>
#include <TH.h>
#endif

#include "project.h"
#include "Image.h"
#include "OpticalFlow.h"
#include <iostream>

using namespace std;

// conversion functions
static DImage *libceliu_(Main_tensor_to_image)(THTensor *tensor) {
  // create output
  int c = tensor->size[0];
  int h = tensor->size[1]; 
  int w = tensor->size[2];
  DImage *img = new DImage(w,h,c);
  // copy data
  int i0,i1,i2;
  double *dest = img->data();
  int offset = 0;
  for (i2=0; i2<h; i2++) {  
    for (i1=0; i1<w; i1++) {
      for (i0=0; i0<c; i0++) {
        dest[offset++] = THTensor_(get3d)(tensor, i0, i2, i1);
      }
    }
  }

  return img;
}

static THTensor *libceliu_(Main_image_to_tensor)(DImage *img) {
  // create output
  THTensor *tensor = THTensor_(newWithSize3d)(img->nchannels(),
					      img->height(),
					      img->width());
  // copy data
  int i0,i1,i2;
  double *src = img->data();
  int offset = 0;
  for (i2=0; i2<img->height(); i2++) {  
    for (i1=0; i1<img->width(); i1++) {
      for (i0=0; i0<img->nchannels(); i0++) {
        THTensor_(set3d)(tensor, i0, i2, i1, (real)src[offset++]);
      }
    }
  }

  return tensor;
}

int libceliu_(Main_optflow)(lua_State *L) {
  // defaults
  double alpha=0.01;
  double ratio=0.75;
  int minWidth=30;
  int nOuterFPIterations=15;
  int nInnerFPIterations=1;
  int nCGIterations=40;
  // get args
  THTensor *ten1 =  (THTensor *)luaT_checkudata(L, 1, torch_(Tensor_id));  
  THTensor *ten2 =  (THTensor *)luaT_checkudata(L, 2, torch_(Tensor_id));  
  if (lua_isnumber(L, 3)) alpha = lua_tonumber(L, 3);
  if (lua_isnumber(L, 4)) ratio = lua_tonumber(L, 4);
  if (lua_isnumber(L, 5)) minWidth = lua_tonumber(L, 5);
  if (lua_isnumber(L, 6)) nOuterFPIterations = lua_tonumber(L, 6);
  if (lua_isnumber(L, 7)) nInnerFPIterations = lua_tonumber(L, 7);
  if (lua_isnumber(L, 8)) nCGIterations = lua_tonumber(L, 8);
  
// copy tensors to images
  DImage *img1 =  libceliu_(Main_tensor_to_image)(ten1);
  DImage *img2 =  libceliu_(Main_tensor_to_image)(ten2);
  
  // declare outputs, and process
  DImage vx,vy,warpI2;
  OpticalFlow::Coarse2FineFlow(vx,vy,warpI2,   // outputs
                               *img1,*img2,      // inputs
                               alpha,ratio,minWidth,  // params...
                               nOuterFPIterations,nInnerFPIterations,nCGIterations);
  
  // return result
  THTensor *ten_vx   = libceliu_(Main_image_to_tensor)(&vx);
  THTensor *ten_vy   = libceliu_(Main_image_to_tensor)(&vy);
  THTensor *ten_warp = libceliu_(Main_image_to_tensor)(&warpI2);
  luaT_pushudata(L, ten_vx, torch_(Tensor_id));
  luaT_pushudata(L, ten_vy, torch_(Tensor_id));;
  luaT_pushudata(L, ten_warp, torch_(Tensor_id));
  
  // cleanup
  delete(img1);
  delete(img2);

  return 3;
}

int libceliu_(Main_warp)(lua_State *L) {
  // get args
  THTensor * ten_inp = (THTensor *)luaT_checkudata(L, 1, torch_(Tensor_id));  
  THTensor * ten_vx  =  (THTensor *)luaT_checkudata(L, 2, torch_(Tensor_id));  
  THTensor * ten_vy  =  (THTensor *)luaT_checkudata(L, 3, torch_(Tensor_id));  

  // copy tensors to images
  DImage *input =  libceliu_(Main_tensor_to_image)(ten_inp);
  DImage *vx    =  libceliu_(Main_tensor_to_image)(ten_vx);
  DImage *vy    =  libceliu_(Main_tensor_to_image)(ten_vy);

  // declare outputs, and process
  DImage warpedInput;
  OpticalFlow::warpFL(warpedInput,   // warped input
                      *input,*input, // input
                      *vx, *vy       // flow
                      );

  // return result
  THTensor *ten_warp =  libceliu_(Main_image_to_tensor)(&warpedInput);
  luaT_pushudata(L, ten_warp, torch_(Tensor_id));

  // cleanup
  delete(input);
  delete(vx);
  delete(vy);

  return 1;
}

extern "C" {
  // Register functions in LUA
  static const struct luaL_reg libceliu_(Main__) [] = {
    {"infer", libceliu_(Main_optflow)},
    {"warp", libceliu_(Main_warp)},
    {NULL, NULL}  /* sentinel */
  };
  
  DLL_EXPORT int libceliu_(Main_init) (lua_State *L) {
    luaT_pushmetaclass(L, torch_(Tensor_id));
    luaT_registeratname(L, libceliu_(Main__), "libceliu");
    return 1; 
  }
}

#endif
