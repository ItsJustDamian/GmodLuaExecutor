#pragma once
#include <Windows.h>
#include "../globals.hpp"

#include "../helpers/simplehook.hpp"
#include "../sdk/interface.hpp"


namespace NetHooks
{
	struct LuaStackGuard {
		LuaInterface* lua;
		int base;
		bool suppressed = false;

		LuaStackGuard(LuaInterface* l) : lua(l), base(l ? l->Top() : 0) {}
		~LuaStackGuard() {
			if (lua && !suppressed) {
				int current = lua->Top();
				if (current > base)
					lua->Pop(current - base); // Pop everything added since the base
			}
		}

		void Suppress()
		{
			suppressed = true;
		}
	};

	struct LuaFuncHook
	{
		std::string table;
		std::string func;
		void* hookFunc = nullptr;
		SimpleHook* hook;

		LuaFuncHook() = default;
		LuaFuncHook(LuaInterface* lua, const std::string& tbl, const std::string& fnc, void* detourFunc)
			: table(tbl), func(fnc), hookFunc(detourFunc) {
		}

		void* hookFunction(LuaInterface* lua) {
			if (!lua || !hookFunc) return nullptr;
			LuaStackGuard guard(lua);
			lua->PushSpecial(SPECIAL_GLOB);
			lua->GetField(-1, table.c_str());
			lua->GetField(-1, func.c_str());
			if (lua->GetType(-1) == LuaObjectType::FUNCTION) {
				hook = new SimpleHook(lua->GetCFunction(-1));
				hook->hook(hookFunc);
				logs->println("Hooked Lua function %s.%s at: 0x%llX", table.c_str(), func.c_str(), (uintptr_t)hookFunc);
				return reinterpret_cast<void*>(hook->get_original());
			}
			else {
				logs->println("Failed to hook Lua function %s.%s: Not a function", table.c_str(), func.c_str());
				return nullptr;
			}
		}

		void removeHook() {
			if (hook) {
				hook->unhook();
				delete hook;
				hook = nullptr;
				logs->println("Removed Lua function hook %s.%s", table.c_str(), func.c_str());
			}
			else {
				logs->println("No Lua function hook to remove for %s.%s", table.c_str(), func.c_str());
			}
		}
	};

	inline std::unordered_map<std::string, LuaFuncHook> luaHooks;

	enum class NetVariableType
	{
		INVALID = -1,
		BIT,
		ENTITY,
		FLOAT,
		INT,
		STRING
	};

	struct NetVariable
	{
		NetVariable()
		{
			value.bit = false; // Default to false for bit type
		}

		NetVariableType type = NetVariableType::INVALID;
		union
		{
			bool bit;
			float floatValue;
			int intValue;
		} value;

		std::string strValue;
	};

	struct NetTransaction
	{
		std::string messageName;
		std::vector<NetVariable*> variables;
		bool finished = false;
	};

	std::vector<NetTransaction*> netTransactions = {};
	std::vector<std::string> skipNetMessages = {};

	typedef bool(__fastcall* netStartFn)(_lua_state*);
	typedef bool(__fastcall* netSendToServerFn)(_lua_state*);
	typedef bool(__fastcall* netWriteBitFn)(_lua_state*);
	typedef bool(__fastcall* netWriteEntityFn)(_lua_state*);
	typedef bool(__fastcall* netWriteFloatFn)(_lua_state*);
	typedef bool(__fastcall* netWriteIntFn)(_lua_state*);
	typedef bool(__fastcall* netWriteStringFn)(_lua_state*);
	typedef bool(__fastcall* netWriteTableFn)(_lua_state*);

	netStartFn oNetStartFn = nullptr;
	netSendToServerFn oNetSendToServerFn = nullptr;
	netWriteBitFn oNetWriteBitFn = nullptr;
	netWriteEntityFn oNetWriteEntityFn = nullptr;
	netWriteFloatFn oNetWriteFloatFn = nullptr;
	netWriteIntFn oNetWriteIntFn = nullptr;
	netWriteStringFn oNetWriteStringFn = nullptr;
	netWriteTableFn oNetWriteTableFn = nullptr;

	NetTransaction* currentTransaction = nullptr;

	typedef const char* (__fastcall* lua_tolstring_t)(_lua_state*, int, uintptr_t*);
	typedef bool(__fastcall* lua_toboolean_t)(_lua_state*, int);
	typedef double(__fastcall* lua_tonumber_t)(_lua_state*, int);
	typedef int(__fastcall* lua_tointeger_t)(_lua_state*, int);
	typedef __int64(__fastcall* lua_gettable_t)(_lua_state*, int);
	std::string luaL_tostring(_lua_state* L, int idx)
	{
		static HMODULE mod = GetModuleHandleA("lua_shared.dll");
		static uintptr_t func = reinterpret_cast<uintptr_t>(GetProcAddress(mod, "lua_tolstring"));

		if (!func)
		{
			logs->println("Failed to get lua_tolstring function address.");
			return "";
		}

		uintptr_t size = 0;
		const char* result = reinterpret_cast<lua_tolstring_t>(func)(L, idx, &size);

		return result ? std::string(result) : "ERR";
	}
	bool luaL_toboolean(_lua_state* L, int idx)
	{
		static HMODULE mod = GetModuleHandleA("lua_shared.dll");
		static uintptr_t func = reinterpret_cast<uintptr_t>(GetProcAddress(mod, "lua_toboolean"));
		if (!func)
		{
			logs->println("Failed to get lua_toboolean function address.");
			return false;
		}
		return reinterpret_cast<lua_toboolean_t>(func)(L, idx);
	}
	double luaL_tonumber(_lua_state* L, int idx)
	{
		static HMODULE mod = GetModuleHandleA("lua_shared.dll");
		static uintptr_t func = reinterpret_cast<uintptr_t>(GetProcAddress(mod, "lua_tonumber"));
		if (!func)
		{
			logs->println("Failed to get lua_tonumber function address.");
			return 0.0;
		}
		return reinterpret_cast<lua_tonumber_t>(func)(L, idx);
	}
	int luaL_tointeger(_lua_state* L, int idx)
	{
		static HMODULE mod = GetModuleHandleA("lua_shared.dll");
		static uintptr_t func = reinterpret_cast<uintptr_t>(GetProcAddress(mod, "lua_tointeger"));
		if (!func)
		{
			logs->println("Failed to get lua_tointeger function address.");
			return 0;
		}
		return reinterpret_cast<lua_tointeger_t>(func)(L, idx);
	}
	__int64 lua_gettable(_lua_state* state, int index)
	{
		static HMODULE mod = GetModuleHandleA("lua_shared.dll");
		static uintptr_t func = reinterpret_cast<uintptr_t>(GetProcAddress(mod, "lua_gettable"));
		if (!func)
		{
			logs->println("Failed to get lua_gettable function address.");
			return 0;
		}
		return reinterpret_cast<lua_gettable_t>(func)(state, index);
	}

	bool __fastcall hkNetStart(_lua_state* state)
	{
		auto msgName = luaL_tostring(state, 1);

		if (currentTransaction != nullptr)
		{
			logs->println("NetStart called while another transaction is in progress. finishing transaction.");
			currentTransaction->finished = true;
			netTransactions.push_back(currentTransaction);
			currentTransaction = nullptr;
		}
		else if (std::find(skipNetMessages.begin(), skipNetMessages.end(), msgName) == skipNetMessages.end()) {
			currentTransaction = new NetTransaction();
			currentTransaction->messageName = msgName;
		}

		return oNetStartFn(state);
	}
	bool __fastcall hkNetSendToServer(_lua_state* state)
	{

		if (currentTransaction != nullptr)
		{
			currentTransaction->finished = true;
			netTransactions.push_back(currentTransaction);
			currentTransaction = nullptr;
			Notifications::AddNotification("Net message sent: " + netTransactions.back()->messageName);
		}

		return oNetSendToServerFn(state);
	}
	bool __fastcall hkNetWriteBit(_lua_state* state)
	{
		auto bit = luaL_toboolean(state, 1);

		if (currentTransaction != nullptr)
		{
			NetVariable* var = new NetVariable();
			var->type = NetVariableType::BIT;
			var->value.bit = bit;
			currentTransaction->variables.push_back(var);
		}

		return oNetWriteBitFn(state);
	}
	bool __fastcall hkNetWriteEntity(_lua_state* state)
	{
		if (currentTransaction != nullptr)
		{
			NetVariable* var = new NetVariable();
			var->type = NetVariableType::ENTITY;
			currentTransaction->variables.push_back(var);
		}

		return oNetWriteEntityFn(state);
	}
	bool __fastcall hkNetWriteFloat(_lua_state* state)
	{
		auto value = luaL_tonumber(state, 1);

		if (currentTransaction != nullptr)
		{
			NetVariable* var = new NetVariable();
			var->type = NetVariableType::FLOAT;
			var->value.floatValue = static_cast<float>(value);
			currentTransaction->variables.push_back(var);
		}

		return oNetWriteFloatFn(state);
	}
	bool __fastcall hkNetWriteInt(_lua_state* state)
	{
		auto value = luaL_tointeger(state, 1);

		if (currentTransaction != nullptr)
		{
			NetVariable* var = new NetVariable();
			var->type = NetVariableType::INT;
			var->value.intValue = value;
			currentTransaction->variables.push_back(var);
		}

		return oNetWriteIntFn(state);
	}
	bool __fastcall hkNetWriteString(_lua_state* state)
	{
		auto str = luaL_tostring(state, 1);

		if (currentTransaction != nullptr)
		{
			NetVariable* var = new NetVariable();
			var->type = NetVariableType::STRING;
			var->strValue = std::string(str);
			currentTransaction->variables.push_back(var);
		}

		return oNetWriteStringFn(state);
	}
	bool __fastcall hkNetWriteTable(_lua_state* state)
	{
		if (currentTransaction != nullptr)
		{
			NetVariable* var = new NetVariable();
			var->type = NetVariableType::INVALID; // Table type not defined, can be extended later
			currentTransaction->variables.push_back(var);
		}
		return oNetWriteTableFn(state);
	}

	bool initialized = false;
	bool hooked = false;

	void init()
	{
		if (std::filesystem::exists("filter.txt"))
		{
			std::ifstream file("filter.txt");
			if (file.is_open())
			{
				std::string line;
				while (std::getline(file, line))
					if (!line.empty())
						skipNetMessages.push_back(line);
				file.close();
				logs->println("Loaded net message filter from filter.txt");
			}
			else
				logs->println("Failed to open filter.txt for reading");
		}
		else
			logs->println("filter.txt not found, skipping loading.");

		LuaFuncHook netStartHook(executor.luaInterface, "net", "Start", &hkNetStart);
		luaHooks["NetStart"] = netStartHook;

		LuaFuncHook netSendToServerHook(executor.luaInterface, "net", "SendToServer", &hkNetSendToServer);
		luaHooks["NetSendToServer"] = netSendToServerHook;

		LuaFuncHook netWriteBitHook(executor.luaInterface, "net", "WriteBit", &hkNetWriteBit);
		luaHooks["NetWriteBit"] = netWriteBitHook;

		LuaFuncHook netWriteEntityHook(executor.luaInterface, "net", "WriteEntity", &hkNetWriteEntity);
		luaHooks["NetWriteEntity"] = netWriteEntityHook;

		LuaFuncHook netWriteFloatHook(executor.luaInterface, "net", "WriteFloat", &hkNetWriteFloat);
		luaHooks["NetWriteFloat"] = netWriteFloatHook;

		LuaFuncHook netWriteIntHook(executor.luaInterface, "net", "WriteInt", &hkNetWriteInt);
		luaHooks["NetWriteInt"] = netWriteIntHook;

		LuaFuncHook netWriteStringHook(executor.luaInterface, "net", "WriteString", &hkNetWriteString);
		luaHooks["NetWriteString"] = netWriteStringHook;

		LuaFuncHook netWriteTableHook(executor.luaInterface, "net", "WriteTable", &hkNetWriteTable);
		luaHooks["NetWriteTable"] = netWriteTableHook;

		Notifications::AddNotification("NetHooks initialized. Net message filter loaded with " + std::to_string(skipNetMessages.size()) + " messages.");

		initialized = true;
	}

	void hook()
	{
		if (!executor.luaInterface)
			return;

		oNetStartFn = reinterpret_cast<netStartFn>(luaHooks["NetStart"].hookFunction(executor.luaInterface));
		oNetSendToServerFn = reinterpret_cast<netSendToServerFn>(luaHooks["NetSendToServer"].hookFunction(executor.luaInterface));
		oNetWriteBitFn = reinterpret_cast<netWriteBitFn>(luaHooks["NetWriteBit"].hookFunction(executor.luaInterface));
		oNetWriteEntityFn = reinterpret_cast<netWriteEntityFn>(luaHooks["NetWriteEntity"].hookFunction(executor.luaInterface));
		oNetWriteFloatFn = reinterpret_cast<netWriteFloatFn>(luaHooks["NetWriteFloat"].hookFunction(executor.luaInterface));
		oNetWriteIntFn = reinterpret_cast<netWriteIntFn>(luaHooks["NetWriteInt"].hookFunction(executor.luaInterface));
		oNetWriteStringFn = reinterpret_cast<netWriteStringFn>(luaHooks["NetWriteString"].hookFunction(executor.luaInterface));
		oNetWriteTableFn = reinterpret_cast<netWriteTableFn>(luaHooks["NetWriteTable"].hookFunction(executor.luaInterface));

		Notifications::AddNotification("NetHooks hooked successfully.");
		logs->println("hooked functions");

		hooked = true;
	}

	void unhook()
	{
		if (!executor.luaInterface)
			return;

		if (!hooked) return;

		for (auto& hook : luaHooks)
			hook.second.removeHook();

		Notifications::AddNotification("NetHooks unhooked successfully.");
		hooked = false;
	}

	void cleanup()
	{
		if (!hooked)
			return;

		unhook();
		luaHooks.clear();
		logs->println("NetHooks cleaned up.");
		initialized = false;
	}
}