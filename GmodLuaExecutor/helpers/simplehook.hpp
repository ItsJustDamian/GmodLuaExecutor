#pragma once
#include <windows.h>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <cstring>
#include <cassert>
#include "../detours/detours.h"

#pragma comment(lib, "detours.lib")

class SimpleHook {
public:
    explicit SimpleHook(void* target)
        : target_(reinterpret_cast<void*>(target)), orig_(target), hooked_(false) {
    }

    ~SimpleHook() {
        unhook();
    }

    // Installs the detour, redirecting target to detour.
    bool hook(void* detour) {
        if (hooked_) return false;
        LONG err;
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        err = DetourAttach(&(PVOID&)orig_, detour);
        if (DetourTransactionCommit() == NO_ERROR && err == NO_ERROR) {
            hooked_ = true;
            return true;
        }
        return false;
    }

    // Removes the detour.
    bool unhook() {
        if (!hooked_) return false;
        LONG err;
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        err = DetourDetach(&(PVOID&)orig_, nullptr); // Detach latest detour
        if (DetourTransactionCommit() == NO_ERROR && err == NO_ERROR) {
            hooked_ = false;
            orig_ = target_;
            return true;
        }
        return false;
    }

    // Returns pointer to call the original function.
    template<typename t = void*>
    t get_original() const { return reinterpret_cast<t>(orig_); }

private:
    void* target_;
    void* orig_;
    bool hooked_;
};

class VmtHook64 {
public:
    // pInstance: pointer to class instance to hook
    VmtHook64(void* pInstance)
        : m_pInstance(reinterpret_cast<uintptr_t**>(pInstance)),
        m_originalVmt(nullptr),
        m_customVmt(nullptr),
        m_numMethods(0)
    {
        assert(m_pInstance != nullptr);
        m_originalVmt = *m_pInstance;
        // Count methods in original vmt
        while (reinterpret_cast<void*>(m_originalVmt[m_numMethods]) != nullptr) {
            ++m_numMethods;
        }

        // Copy original vmt
        m_customVmt = new uintptr_t[m_numMethods];
        std::memcpy(m_customVmt, m_originalVmt, m_numMethods * sizeof(uintptr_t));
    }

    ~VmtHook64() {
        Restore();
        delete[] m_customVmt;
    }

    // index: virtual method index
    // hookFunc: pointer to your function
    void HookMethod(void* hookFunc, size_t index) {
        assert(index < m_numMethods);
        m_customVmt[index] = reinterpret_cast<uintptr_t>(hookFunc);
        *m_pInstance = m_customVmt;
    }

    void UnhookMethod(size_t index) {
        assert(index < m_numMethods);
        m_customVmt[index] = m_originalVmt[index];
    }

    void Restore() {
        if (m_pInstance && m_originalVmt)
            *m_pInstance = m_originalVmt;
    }

    // Get pointer to original function at index
    void* GetOriginal(size_t index) const {
        assert(index < m_numMethods);
        return reinterpret_cast<void*>(m_originalVmt[index]);
    }

    // Optionally: returns vtable size (number of entries)
    size_t GetNumMethods() const { return m_numMethods; }

private:
    uintptr_t** m_pInstance;
    uintptr_t* m_originalVmt;
    uintptr_t* m_customVmt;
    size_t m_numMethods;
};