
#include <TH.h>
#include <luaT.h>

#define torch_(NAME) TH_CONCAT_3(torch_, Real, NAME)
#define torch_string_(NAME) TH_CONCAT_STRING_3(torch., Real, NAME)
#define libceliu_(NAME) TH_CONCAT_3(libceliu_, Real, NAME)

static const void* torch_FloatTensor_id = NULL;
static const void* torch_DoubleTensor_id = NULL;


#include "generic/celiu.cpp"
#include "THGenerateFloatTypes.h"


DLL_EXPORT int luaopen_libceliu(lua_State *L)
{
  torch_FloatTensor_id = luaT_checktypename2id(L, "torch.FloatTensor");
  torch_DoubleTensor_id = luaT_checktypename2id(L, "torch.DoubleTensor");

  libceliu_FloatMain_init(L);
  libceliu_DoubleMain_init(L);

  return 1;
}
