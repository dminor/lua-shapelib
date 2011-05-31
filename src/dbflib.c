/*
Copyright (c) 2011 Daniel Minor 

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

#define checkdbfhandle(ls) *((DBFHandle *)luaL_checkudata(ls, 1, "dbflib.dbfhandle"))

static int dbflib_open(lua_State *ls)
{
    const char *filename = luaL_checkstring(ls, 1);

    DBFHandle dbf = DBFOpen(filename, "rb+");

    if (dbf) {
        DBFHandle *dbf_ptr = lua_newuserdata(ls, sizeof(DBFHandle *));
        *dbf_ptr = dbf;

        luaL_getmetatable(ls, "dbflib.dbfhandle");
        lua_setmetatable(ls, -2);

        return 1;
    }

    return luaL_error(ls, "could not open dbf file: %s", filename);
}

static int dbflib_create(lua_State *ls)
{
    const char *filename = luaL_checkstring(ls, 1);

    DBFHandle dbf = DBFCreate(filename);

    if (dbf) {
        DBFHandle *dbf_ptr = lua_newuserdata(ls, sizeof(DBFHandle *));
        *dbf_ptr = dbf;

        luaL_getmetatable(ls, "dbflib.dbfhandle");
        lua_setmetatable(ls, -2);

        return 1;
    }

    return luaL_error(ls, "could not create dbf file: %s", filename);
}

static int dbflib_addfield(lua_State *ls)
{
    DBFHandle dbf = checkdbfhandle(ls); 
    const char *name = luaL_checkstring(ls, 2);
    int type = luaL_checkinteger(ls, 3);
    int width = luaL_checkinteger(ls, 4);
    int decimals = luaL_checkinteger(ls, 5);

    lua_pushnumber(ls, DBFAddField(dbf, name, type, width, decimals));

    return 1;
}

static int dbflib_recordcount(lua_State *ls)
{
    DBFHandle dbf = checkdbfhandle(ls); 
    lua_pushnumber(ls, DBFGetRecordCount(dbf));

    return 1;
} 
 
static int dbflib_fieldcount(lua_State *ls)
{
    DBFHandle dbf = checkdbfhandle(ls); 
    lua_pushnumber(ls, DBFGetFieldCount(dbf));

    return 1;
}

static int dbflib_fieldinfo(lua_State *ls)
{
    DBFHandle dbf = checkdbfhandle(ls); 
    int field = luaL_checkinteger(ls, 2) - 1;

    int type;
    char name[12];
    int width;
    int decimals;

    type = DBFGetFieldInfo(dbf, field, name, &width, &decimals);
    lua_pushnumber(ls, type);
    lua_pushstring(ls, name);
    lua_pushnumber(ls, width);
    lua_pushnumber(ls, decimals);

    return 4;
}

static int dbflib_readfield(lua_State *ls)
{
    DBFHandle dbf = checkdbfhandle(ls);
    int record = luaL_checkinteger(ls, 2) - 1; 
    int field = luaL_checkinteger(ls, 3) - 1;

    const char *value = DBFReadStringAttribute(dbf, record, field);

    lua_pushstring(ls, value);
    return 1;
}

static int dbflib_writefield(lua_State *ls)
{
    DBFHandle dbf = checkdbfhandle(ls);
    int record = luaL_checkinteger(ls, 2) - 1; 
    int field = luaL_checkinteger(ls, 3) - 1; 

    int result = 0;
    int type = DBFGetFieldInfo(dbf, field, 0,  0, 0);
    switch (type) {

        case FTInteger:
            result = DBFWriteIntegerAttribute(dbf, record, field,
                luaL_checkinteger(ls, 4));
            break;

        case FTDouble:
            result = DBFWriteDoubleAttribute(dbf, record, field,
                luaL_checknumber(ls, 4));
            break;

        case FTString:
            result = DBFWriteStringAttribute(dbf, record, field,
                luaL_checkstring(ls, 4));
            break;

    }

    lua_pushboolean(ls, result);
    return 1; 
}

static int dbflib_close(lua_State *ls)
{
    DBFHandle dbf = checkdbfhandle(ls);
    DBFClose(dbf);

    return 0;
}

static luaL_Reg dbflib_f[] = {
    {"open", dbflib_open},
    {"create", dbflib_create},
    {"addfield", dbflib_addfield},
    {"recordcount", dbflib_recordcount},
    {"fieldcount", dbflib_fieldcount},
    {"fieldinfo", dbflib_fieldinfo},
    {"readfield", dbflib_readfield},
    {"writefield", dbflib_writefield},
    {"close", dbflib_close},
    {0, 0}
};

int luaopen_dbflib(lua_State *ls)
{
    luaL_newmetatable(ls, "dbflib.dbfhandle");

    luaL_register(ls, "dbflib", dbflib_f); 

    lua_pushnumber(ls, FTString);
    lua_setfield(ls, -2, "String");

    lua_pushnumber(ls, FTInteger);
    lua_setfield(ls, -2, "Integer");

    lua_pushnumber(ls, FTDouble);
    lua_setfield(ls, -2, "Double");

    lua_pushnumber(ls, FTLogical);
    lua_setfield(ls, -2, "Logical");

    lua_pushnumber(ls, FTInvalid);
    lua_setfield(ls, -2, "Invalid");

    // set metatable index to dbflib table
    luaL_getmetatable(ls, "dbflib.dbfhandle");
    lua_getglobal(ls, "dbflib");
    lua_setfield(ls, -2, "__index");
    lua_pop(ls, 1);

    return 1;
}
