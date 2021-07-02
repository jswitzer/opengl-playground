################################################################
# lua
################################################################
cmake_minimum_required(VERSION 2.6)
project(lua)

set(CORE_H
  lapi.h
	lctype.h
	ldebug.h
	ldo.h 
	lfunc.h 
	lgc.h 
	lmem.h 
	lobject.h 
	lopcodes.h 
	lstate.h 
	lstring.h 
	ltable.h 
	ltm.h 
	lvm.h
	lzio.h
	lcode.h
	llex.h 
	llimits.h
	lparser.h 
	lundump.h 
	lua.h
	luaconf.h
	)
set(CORE_C
	lctype.c 
	ldebug.c 
	ldo.c 
	lfunc.c 
	lgc.c 
	lmem.c 
	lobject.c 
	lopcodes.c 
	lstate.c 
	lstring.c 
	ltable.c 
	ltm.c 
	lvm.c 
	lzio.c 
	lcode.c 
	lapi.c 
	lundump.c 
	lparser.c 
	ldump.c 
	llex.c 
	)
set(LIBS 
	lualib.h
	lauxlib.h
	lauxlib.c 
	lbaselib.c 
	lbitlib.c 
	lcorolib.c 
	ldblib.c 
	liolib.c 
	lmathlib.c 
	loslib.c 
	lstrlib.c 
	ltablib.c 
	loadlib.c 
	lutf8lib.c 
	linit.c)
source_group(core_h FILES ${CORE_H})
source_group(core_c FILES ${CORE_C})
source_group(libs FILES ${LIBS})
add_library(liblua STATIC ${CORE_H} ${CORE_C} ${LIBS})
add_executable(lua lua.c)
target_link_libraries(lua liblua)
add_executable(luac luac.c)
target_link_libraries(luac liblua)
