#pragma once
#include <iostream>

class LuaInterface;
struct _lua_state
{
	unsigned char	_ud[69];
	LuaInterface* m_interface;
};

typedef struct _lua_debug {
	int event;
	const char* name;           /* (n) */
	const char* namewhat;       /* (n) */
	const char* what;           /* (S) */
	const char* source;         /* (S) */
	int currentline;            /* (l) */
	int nups;                   /* (u) number of upvalues */
	int linedefined;            /* (S) */
	int lastlinedefined;        /* (S) */
	char short_src[60];			/* (S) */

	/* Private */
	struct lua_TObject* _func;  /* active function */
} _lua_debug;

typedef int(__cdecl* CLuaFunction)(_lua_state*);

typedef struct _luaL_reg {
	const char* name;
	CLuaFunction func;
} _luaL_Reg;

struct user_data {
	void* data;
	unsigned char	type;
};

enum
{
	SPECIAL_GLOB,		// Global table
	SPECIAL_ENV,		// Environment table
	SPECIAL_REG,		// Registry table
};

namespace LuaObjectType
{
	enum
	{
		INVALID = -1,
		NIL,
		BOOL,
		LIGHTUSERDATA,
		NUMBER,
		STRING,
		TABLE,
		FUNCTION,
		USERDATA,
		THREAD,

		// UserData
		ENTITY,
		VECTOR,
		ANGLE,
		PHYSOBJ,
		SAVE,
		RESTORE,
		DAMAGEINFO,
		EFFECTDATA,
		MOVEDATA,
		RECIPIENTFILTER,
		USERCMD,
		SCRIPTEDVEHICLE,

		// Client Only
		MATERIAL,
		PANEL,
		PARTICLE,
		PARTICLEEMITTER,
		TEXTURE,
		USERMSG,

		CONVAR,
		IMESH,
		MATRIX,
		SOUND,
		PIXELVISHANDLE,
		DLIGHT,
		VIDEO,
		FILE,

		COUNT
	};

	static const char* Name[] =
	{
		"nil",
		"bool",
		"lightuserdata",
		"number",
		"string",
		"table",
		"function",
		"userdata",
		"thread",
		"entity",
		"vector",
		"angle",
		"physobj",
		"save",
		"restore",
		"damageinfo",
		"effectdata",
		"movedata",
		"recipientfilter",
		"usercmd",
		"vehicle",
		"material",
		"panel",
		"particle",
		"particleemitter",
		"texture",
		"usermsg",
		"convar",
		"mesh",
		"matrix",
		"sound",
		"pixelvishandle",
		"dlight",
		"video",
		"file",

		0
	};
};

class _lua_object
{
public:
	virtual void			Set(_lua_object* obj) = 0;
	virtual void			SetFromStack(int i) = 0;
	virtual void			Unreference() = 0;

	virtual int				GetType(void) = 0;

	virtual const char* GetString(void) = 0;
	virtual float			GetFloat(void) = 0;
	virtual int				GetInt(void) = 0;

	virtual user_data* GetUserData(void) = 0;

	virtual void			SetMember(const char* name) = 0;
	virtual void			SetMember(const char* name, _lua_object* obj) = 0; // ( This is also used to set nil by passing NULL )
	virtual void			SetMember(const char* name, float val) = 0;
	virtual void			SetMember(const char* name, bool val) = 0;
	virtual void			SetMember(const char* name, const char* val) = 0;
	virtual void			SetMember(const char* name, CLuaFunction f) = 0;

	virtual bool			GetMemberBool(const char* name, bool b = true) = 0;
	virtual int				GetMemberInt(const char* name, int i = 0) = 0;
	virtual float			GetMemberFloat(const char* name, float f = 0.0f) = 0;
	virtual const char* GetMemberString(const char* name, const char* = "") = 0;
	virtual void* GetMemberUserData(const char* name, void* = 0) = 0;
	virtual void* GetMemberUserData(float name, void* = 0) = 0;
	virtual _lua_object* GetMember(const char* name) = 0;
	virtual _lua_object* GetMember(_lua_object*) = 0;

	virtual void			SetMetaTable(_lua_object* obj) = 0;
	virtual void			SetUserData(void* obj) = 0;

	virtual void			push(void) = 0;

	virtual bool			is_nil() = 0;
	virtual bool			is_table() = 0;
	virtual bool			is_string() = 0;
	virtual bool			is_number() = 0;
	virtual bool			is_function() = 0;
	virtual bool			is_user_data() = 0;

	virtual _lua_object* get_member(float fKey) = 0;

	virtual void* remove_me_1(const char* name, void* = 0) = 0;

	virtual void			set_member(float fKey) = 0;
	virtual void			set_member(float fKey, _lua_object* obj) = 0;
	virtual void			set_member(float fKey, float val) = 0;
	virtual void			set_member(float fKey, bool val) = 0;
	virtual void			set_member(float fKey, const char* val) = 0;
	virtual void			set_member(float fKey, CLuaFunction f) = 0;

	virtual const char* get_member_str(float name, const char* = "") = 0;

	virtual void			set_member(_lua_object* k, _lua_object* v) = 0;
	virtual bool			get_bool(void) = 0;

	// Push members to table from stack
	virtual bool			push_member_fast(int iStackPos) = 0;
	virtual void			set_member_fast(int iKey, int iValue) = 0;

	virtual void			set_float(float val) = 0;
	virtual void			set_string(const char* val) = 0;
	virtual double			get_double(void) = 0;

	virtual void			set_member_fix_key(char  const*, float) = 0;
	virtual void			set_member_fix_key(char  const*, char  const*) = 0;
	virtual void			set_member_fix_key(char  const*, _lua_object*) = 0;

	virtual bool			is_bool(void) = 0;
	virtual void			set_member_double(char  const*, double) = 0;
	virtual void			set_member_nil(char  const*) = 0;
	virtual void			set_member_nil(float) = 0;

	virtual bool			debug_is_unreferenced() = 0;

	virtual void			init(void) = 0;
	virtual void			set_from_global(char  const*) = 0;

	virtual void			set_member(char  const*, unsigned long long) = 0;
	virtual void			set_reference(int i) = 0;

	virtual void			remove_member(char  const*) = 0;
	virtual void			remove_member(float) = 0;
	virtual bool			member_is_nil(char  const*) = 0;

	virtual void			set_member_double(float, double) = 0;
	virtual double			get_member_double(char  const*, double) = 0;
};

class c_lua_object : public _lua_object
{
public:
	int						_unk1;				//0004
	int						_unk2;				//0008
	int						_unk3;				//000C
	LuaInterface* lua;				//0010
};

class _threaded_call
{
public:
};

class _lua_callback
{
public:
};

namespace bootil
{
	struct _buffer
	{
		unsigned char u[0x1000];
	};
};

class LuaInterface
{
public:
	virtual int				Top(void) = 0;																			//0000
	virtual void			Push(int iStackPos) = 0;																	//0004
	virtual void			Pop(int iAmt = 1) = 0;																	//0008
	virtual void			GetTable(int iStackPos) = 0;																//000C
	virtual void			GetField(int iStackPos, const char* strName) = 0;											//0010
	virtual void			SetField(int iStackPos, const char* strName) = 0;											//0014
	virtual void			create_table() = 0;																			//0018
	virtual void			set_table(int i) = 0;																		//001C
	virtual void			set_meta_table(int i) = 0;																	//0020
	virtual bool			get_meta_table(int i) = 0;																	//0024
	virtual void			Call(int iArgs, int iResults) = 0;
	virtual int				pCall(int iArgs, int iResults, int iErrorFunc) = 0;
	virtual int				equal(int iA, int iB) = 0;
	virtual int				raw_equal(int iA, int iB) = 0;
	virtual void			insert(int iStackPos) = 0;
	virtual void			remove(int iStackPos) = 0;
	virtual int				Next(int iStackPos) = 0;
	virtual void* new_user_data(unsigned int iSize) = 0;
	virtual void			throw_error(const char* strError) = 0;
	virtual void			check_type(int iStackPos, int iType) = 0;
	virtual void			arg_error(int iArgNum, const char* strMessage) = 0;
	virtual void			raw_get(int iStackPos) = 0;
	virtual void			raw_set(int iStackPos) = 0;
	virtual const char* GetString(int iStackPos = -1, unsigned int* iOutLen = NULL) = 0;
	virtual double			GetNumber(int iStackPos = -1) = 0;
	virtual bool			GetBool(int iStackPos = -1) = 0;
	virtual CLuaFunction	GetCFunction(int iStackPos = -1) = 0;
	virtual void* get_user_data(int iStackPos = -1) = 0;
	virtual void			PushNil() = 0;
	virtual void			PushString(const char* val, unsigned int iLen = 0) = 0;
	virtual void			PushNumber(double val) = 0;
	virtual void			push_bool(bool val) = 0;
	virtual void			push_c_function(CLuaFunction val) = 0;
	virtual void			push_c_closure(CLuaFunction val, int iVars) = 0;
	virtual void			push_user_data(void*) = 0;
	virtual int				reference_create() = 0;
	virtual void			reference_free(int i) = 0;
	virtual void			reference_push(int i) = 0;
	virtual void			PushSpecial(int iType) = 0;
	virtual bool			IsType(int iStackPos, int iType) = 0;
	virtual int				GetType(int iStackPos) = 0;
	virtual const char* get_type_name(int iType) = 0;
	virtual void			create_meta_table_type(const char* strName, int iType) = 0;
	virtual const char* check_string(int iStackPos = -1) = 0;
	virtual double			check_number(int iStackPos = -1) = 0;
	virtual size_t			obj_len(int index) = 0;

	// New
	virtual void			add_threaded_call(_threaded_call* tc) = 0;
	virtual bool			init(_lua_callback* pCallBack, bool bLoadJit) = 0;
	virtual void			shutdown(void) = 0;
	virtual void* cycle(void) = 0;
	virtual _lua_state* get_lua_state() = 0;
	virtual _lua_object* global() = 0;
	virtual _lua_object* get_object(int index) = 0;
	virtual void			_nullstub1(int) = 0;
	virtual void			_nullstub2(int) = 0;
	virtual void			push_lua_object(_lua_object* obj) = 0;
	virtual void			push_lua_function(CLuaFunction func) = 0;
	virtual void			lua_error(const char* err, int index) = 0;
	virtual void			type_error(const char* name, int index) = 0;
	virtual void			call_internal(int args, int rets) = 0;
	virtual void			call_internal_no_returns(int args) = 0;
	virtual bool			call_internal_get_bool(int args) = 0;
	virtual const char* call_internal_get_string(int args) = 0;
	virtual bool			call_internal_get(int args, _lua_object* obj) = 0;
	virtual void			_nullstub3(_lua_object*, void*) = 0;
	virtual void			new_global_table(const char* name) = 0;
	virtual _lua_object* new_temporary_object() = 0;
	virtual bool			is_user_data(int index) = 0;
	virtual _lua_object* get_meta_table_object(const char* name, int type) = 0;
	virtual _lua_object* get_meta_table_object(int index) = 0;
	virtual _lua_object* get_return(int index) = 0;
	virtual bool			is_server() = 0;
	virtual bool			is_client() = 0;
	virtual bool			is_dedicated_server() = 0;
	virtual void			destroy_object(_lua_object* obj) = 0;
	virtual _lua_object* create_object() = 0;
	virtual void			set_member(_lua_object* table, _lua_object* key, _lua_object* value) = 0;
	virtual void			get_new_table() = 0;
	virtual void			set_member(_lua_object* table, float key) = 0;
	virtual void			set_member(_lua_object* table, float key, _lua_object* value) = 0;
	virtual void			set_member(_lua_object* table, const char* key) = 0;
	virtual void			set_member(_lua_object* table, const char* key, _lua_object* value) = 0;
	virtual void			set_is_server(bool isServer) = 0;
	virtual void			push_long(long num) = 0;
	virtual int				get_flags(int index) = 0;
	virtual bool			find_on_objects_meta_table(int objIndex, int keyIndex) = 0;
	virtual bool			find_objects_on_table(int tableIndex, int keyIndex) = 0;
	virtual void			set_member_fast(_lua_object* table, int keyIndex, int valueIndex) = 0;
	virtual bool			run_string(const char* filename, const char* path, const char* stringToRun, bool run, bool showErrors) = 0;
	virtual bool			is_equal(_lua_object* objA, _lua_object* objB) = 0;
	virtual void			error(const char* err) = 0;
	virtual const char* get_string_or_error(int index) = 0;
	virtual bool			run_lua_module(const char* name) = 0;
	virtual bool			find_and_run_script(const char* filename, bool run, bool showErrors, const char*) = 0;
	virtual void			set_path_id(const char* pathID) = 0;
	virtual const char* get_path_id() = 0;
	virtual void			error_no_halt(const char* fmt, ...) = 0;
	virtual void			msg(const char* fmt, ...) = 0;
	virtual void			push_path(const char* path) = 0;
	virtual void			pop_path() = 0;
	virtual const char* get_path() = 0;
	virtual int				get_color(int index) = 0;
	virtual void			push_color(int r, int g, int b, int a) = 0;
	virtual int				get_stack(int level, _lua_debug* dbg) = 0;
	virtual int				get_info(const char* what, _lua_debug* dbg) = 0;
	virtual const char* get_local(_lua_debug* dbg, int n) = 0;
	virtual const char* get_up_value(int funcIndex, int n) = 0;
	virtual void			run_string_ex(const char* filename, const char* path, const char* stringToRun, bool run, bool showErrors, bool) = 0;
	virtual void			_nullstub4(const char*, int) = 0;
	virtual size_t			get_data_string(int index, const char** str) = 0;
	virtual void			error_from_lua(const char* fmt, ...) = 0;
	virtual void			get_current_location() = 0;
	virtual void			msg_colour(const void*& col, const char* fmt, ...) = 0;
	virtual void			set_state(_lua_state* state) = 0;
	virtual void			_nullstub5(void) = 0;
	virtual void			get_current_file(std::string& outStr) = 0;
	virtual void			compile_string(bootil::_buffer& dumper, const std::string& stringToCompile) = 0;
	virtual void			thread_lock() = 0;
	virtual void			thread_unlock() = 0;
	virtual bool			call_function_protected(int, int, bool) = 0;
	virtual void			require(const char* name) = 0;
	virtual const char* get_actual_type_name(int type) = 0;
	virtual void			setup_infinite_loop_protection(void);

	bool operator==(LuaInterface* inf)
	{
		return reinterpret_cast<void*>(this) == reinterpret_cast<void*>(inf);
	}
};