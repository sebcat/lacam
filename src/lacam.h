#ifndef LACAM_LIB_H__
#define LACAM_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <lauxlib.h>

LUALIB_API int luaopen_lacam(lua_State *L);

#ifdef __cplusplus
}
#endif
#endif /* LACAM_LIB_H__ */
