/*
 * quick compile of modules or extending using lua
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libtcc.h"


#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// Here is the same function from the previous example
int multiplication(lua_State *L) {
    int a = luaL_checkinteger(L, 1);
    int b = luaL_checkinteger(L, 2);
    lua_Integer c = a * b;
    lua_pushinteger(L, c);
    return 1;
}

// See https://lucasklassmann.com/blog/2019-02-02-how-to-embeddeding-lua-in-c/
int module_luatest() {

    lua_State *L;
    char * code = "print(MyMath.mul(7, 8))";

    // First, we need to define an array with
    // all functions that will be available inside our namespace 
    const struct luaL_Reg MyMathLib[] = {
        { "mul", multiplication }
    };

	L = luaL_newstate();
    luaL_openlibs(L);

    // We create a new table
    lua_newtable(L);

    // Here we set all functions from MyMathLib array into
    // the table on the top of the stack
    luaL_setfuncs(L, &MyMathLib[0], 0);

    // We get the table and set as global variable
    lua_setglobal(L, "MyMath");

    // Now we can call from Lua using the namespace MyMath

    if (luaL_dostring(L, code) == LUA_OK) {
        lua_pop(L, lua_gettop(L));
    }

    lua_close(L);
    return 0;
}


static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {
    (void)ud;  (void)osize;  /* not used */
    if (nsize == 0) {
        free(ptr);
        return NULL;
    } else {
        return realloc(ptr, nsize);
    }
}

/* this function is called by the generated code */
int add(int a, int b)
{
    return a + b;
}

/* this strinc is referenced by the generated code */
const char hello[] = "Hello World!";

char my_program[] =
"#include <tcclib.h>\n" /* include the "Simple libc header for TCC" */
"extern int add(int a, int b);\n"
"#ifdef _WIN32\n" /* dynamically linked data needs 'dllimport' */
" __attribute__((dllimport))\n"
"#endif\n"
"extern const char hello[];\n"
"int fib(int n)\n"
"{\n"
"    if (n <= 2)\n"
"        return 1;\n"
"    else\n"
"        return fib(n-1) + fib(n-2);\n"
"}\n"
"\n"
"int foo(int n)\n"
"{\n"
"    printf(\"%s\\n\", hello);\n"
"    printf(\"fib(%d) = %d\\n\", n, fib(n));\n"
"    printf(\"add(%d, %d) = %d\\n\", n, 2 * n, add(n, 2 * n));\n"
"    return 0;\n"
"}\n";

int module_test()
{
    TCCState *s;
    int i;
    int (*func)(int);

    s = tcc_new();
    if (!s) {
        fprintf(stderr, "Could not create tcc state\n");
        exit(1);
    }
    tcc_set_lib_path(s, "./../tcc/child_lib");
    tcc_add_include_path(s, "./../tcc/child_include");
    //tcc_add_library_path(s, a+2); */

    /* MUST BE CALLED before any compilation */
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

    if (tcc_compile_string(s, my_program) == -1)
        return 1;

    /* as a test, we add symbols that the compiled program can use.
       You may also open a dll with tcc_add_dll() and use symbols from that */
    tcc_add_symbol(s, "add", add);
    tcc_add_symbol(s, "hello", hello);

    /* relocate the code */
    if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0)
        return 1;

    /* get entry symbol */
    func = tcc_get_symbol(s, "foo");
    if (!func)
        return 1;

    /* run the code */
    func(32);

    /* delete the state */
    tcc_delete(s);

	module_luatest();
    return 0;
}
