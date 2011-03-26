/*
Copyright (c) 2010 Daniel Minor 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/ 

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include <shapefil.h>

#define checkshphandle(ls) *((SHPHandle *)luaL_checkudata(ls, 1, "shapelib.shphandle"))

static int shapelib_open(lua_State *ls)
{
    const char *filename = luaL_checkstring(ls, 1);

    SHPHandle shp = SHPOpen(filename, "rb+");

    if (shp) {
        SHPHandle *shp_ptr = lua_newuserdata(ls, sizeof(SHPHandle *));
        *shp_ptr = shp;

        luaL_getmetatable(ls, "shapelib.shphandle");
        lua_setmetatable(ls, -2);

        return 1;
    }

    return luaL_error(ls, "could not open shapefile: %s", filename);
}

static int shapelib_create(lua_State *ls)
{
    const char *filename = luaL_checkstring(ls, 1);
    int type = (int)luaL_checknumber(ls, 2);

    SHPHandle shp = SHPCreate(filename, type);

    if (shp) {
        SHPHandle *shp_ptr = lua_newuserdata(ls, sizeof(SHPHandle *));
        *shp_ptr = shp;

        luaL_getmetatable(ls, "shapelib.shphandle");
        lua_setmetatable(ls, -2);

        return 1;
    }

    return luaL_error(ls, "could not create shapefile: %s", filename);
}
static int shapelib_len(lua_State *ls)
{
    SHPHandle shp = checkshphandle(ls);

    int entcount;
    SHPGetInfo(shp, &entcount, 0, 0, 0);

    lua_pushnumber(ls, entcount);

    return 1;
} 
 
static int shapelib_getinfo(lua_State *ls)
{
    SHPHandle shp = checkshphandle(ls);

    int entcount;
    int shptype; 
    double min[4];
    double max[4];

    SHPGetInfo(shp, &entcount, &shptype, min, max);

    lua_pushnumber(ls, entcount);
    lua_pushnumber(ls, shptype);
    for (int i = 0; i < 4; ++i) lua_pushnumber(ls, min[i]);
    for (int i = 0; i < 4; ++i) lua_pushnumber(ls, max[i]);

    return 10;
}

static int shapelib_readobject(lua_State *ls)
{
    SHPHandle shp = checkshphandle(ls);
    int index = luaL_checknumber(ls, 2) - 1; 

    SHPObject *obj = SHPReadObject(shp, index);

    if (obj) {
        lua_newtable(ls);
        for (int i = 0; i < obj->nVertices; ++i) {
            lua_pushnumber(ls, i + 1);
            lua_newtable(ls);
            lua_pushnumber(ls, obj->padfX[i]);
            lua_setfield(ls, -2, "x");
            lua_pushnumber(ls, obj->padfY[i]);
            lua_setfield(ls, -2, "y");
            lua_pushnumber(ls, obj->padfZ[i]);
            lua_setfield(ls, -2, "z");
            lua_settable(ls, -3);
        }

        SHPDestroyObject(obj);
        return 1;
    }

    return 0; 
}

static int shapelib_writeobject(lua_State *ls)
{
    SHPHandle shp = checkshphandle(ls);
    int index = (int)luaL_checknumber(ls, 2); 

    //verify index
    int entcount;
    int shptype; 
    SHPGetInfo(shp, &entcount, &shptype, 0, 0); 

    if (index != -1 || (index - 1) >= entcount) {
        return luaL_error(ls, "invalid index"); 
    }

    //verify vertices
    if (!lua_istable(ls, 3)) {
        return luaL_error(ls, "bad vertices");
    }

    int nvertices = lua_objlen(ls, 3);
    if (nvertices <= 0) {
        return luaL_error(ls, "bad vertices");
    }

    //allocate memory for vertices
    double *x = malloc(nvertices * sizeof(double));
    double *y = malloc(nvertices * sizeof(double));
    double *z = malloc(nvertices * sizeof(double));

    //extract vertices
    for (size_t i = 0; i < nvertices; ++i) {
        lua_pushnumber(ls, i + 1);
        lua_gettable(ls, 3);

        lua_pushstring(ls, "x");
        lua_gettable(ls, -2);
        x[i] = lua_tonumber(ls, -1); 
        lua_pop(ls, 1);

        lua_pushstring(ls, "y");
        lua_gettable(ls, -2);
        y[i] = lua_tonumber(ls, -1); 
        lua_pop(ls, 1);

        lua_pushstring(ls, "z");
        lua_gettable(ls, -2);
        z[i] = lua_tonumber(ls, -1); 
        lua_pop(ls, 1);

        lua_pop(ls, 1);
    }

    //create and write object
    SHPObject *obj = SHPCreateSimpleObject(shptype, nvertices, x, y, z);
    SHPWriteObject(shp, index == - 1? index : index - 1, obj); 

    //clean up
    SHPDestroyObject(obj); 
    free(x); 
    free(y); 
    free(z); 

    return 0;
}

static int shapelib_close(lua_State *ls)
{
    SHPHandle shp = checkshphandle(ls);
    SHPClose(shp);

    return 0;
}

static luaL_Reg shapelib_f[] = {
    {"open", shapelib_open},
    {"create", shapelib_create},
    {"getinfo", shapelib_getinfo},
    {"close", shapelib_close},
    {0, 0}
};

static luaL_Reg shapelib_shpm[] = {
    {"__index", shapelib_readobject},
    {"__newindex", shapelib_writeobject},
    {"__len", shapelib_len},
    {0, 0} 
};

int luaopen_shapelib(lua_State *ls)
{
    luaL_newmetatable(ls, "shapelib.shphandle");
    luaL_register(ls, 0, shapelib_shpm);

    luaL_register(ls, "shapelib", shapelib_f); 

    lua_pushnumber(ls, SHPT_NULL);
    lua_setfield(ls, -2, "Null");

    lua_pushnumber(ls, SHPT_POINT);
    lua_setfield(ls, -2, "Point");

    lua_pushnumber(ls, SHPT_ARC);
    lua_setfield(ls, -2, "Arc");

    lua_pushnumber(ls, SHPT_POLYGON);
    lua_setfield(ls, -2, "Polygon");

    lua_pushnumber(ls, SHPT_MULTIPOINT);
    lua_setfield(ls, -2, "Multipoint");

    return 1;
}
