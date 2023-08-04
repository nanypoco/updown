#include <windows.h>
#include <commctrl.h>
#include <winuser.h>
#include "aviutl/filter.h"
#include <iostream>
#include <string>

FILTER_DLL filter = {
    FILTER_FLAG_ALWAYS_ACTIVE,
    NULL,NULL,
    const_cast<char*>("updown"),
    NULL, NULL, NULL,
    NULL, NULL,
    NULL, NULL, NULL,
    NULL,
    func_init, func_exit, NULL,
    NULL,
    NULL,NULL,
    NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,
    NULL, NULL, NULL,
};
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void) {
    return &filter;
}


static int exedit_base;
static WNDPROC exedit_SettingDialog_WndProc;
static char path[MAX_PATH + 1];
static UINT scale[6] = {1,1,1,1,1,1};
static const char section_default[] = "Default";
static const char section_shift[] = "Shift";


FILTER* get_exeditfp(FILTER* fp) {
    SYS_INFO si;
    fp->exfunc->get_sys_info(NULL, &si);

    for (int i = 0; i < si.filter_n; i++) {
        FILTER* tfp = (FILTER*)fp->exfunc->get_filterp(i);
        if (tfp->information != NULL) {
            if (!strcmp(tfp->information, "ägí£ï“èW(exedit) version 0.92 by ÇjÇdÇmÇ≠ÇÒ")) return tfp;
        }
    }
    return NULL;
}

BOOL exedit_Replace_Address(int exedit_address, const void* function) {
    DWORD oldProtect;
    byte* address = (byte*)(exedit_address + exedit_base);
    if (!VirtualProtect(address, 4, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }
    *(uint32_t*)address = (uint32_t)(function);
    return VirtualProtect(address, 4, oldProtect, &oldProtect);
}

void change_delta(int* data, LPNMUPDOWN& lpnmud) {
    int mul = data[3];
    int n = (GetKeyState(VK_SHIFT) & 0x8000) ? 3 : 0;
    if (data[9] > 1) { //ÉAÉjÉÅÅ[ÉVÉáÉìå¯â ÇÃèÍçá
        mul /= data[9];
    }
    switch (mul) {
    default:
        // êÆêîÇÃèÍçá
        lpnmud->iDelta *= scale[n];
        break;
    case 10:
        // 0.0ÇÃèÍçá
        lpnmud->iDelta *= scale[n + 1];
        break;
    case 100:
        // 0.00ÇÃèÍçá
        lpnmud->iDelta *= scale[n + 2];
        break;
    }
}

LRESULT CALLBACK exedit_SettingDialog_WndProc_wrap(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_NOTIFY) {
        //0x2cde0,WM_HSCROLL(0x114),SB_THUMBPOSITION(0x04)
        int tracknum = *(int*)(exedit_base + 0x14d4b0);
        if (tracknum == 0) return exedit_SettingDialog_WndProc(hwnd, message, wparam, lparam);
        LPNMUPDOWN lpnmud = (LPNMUPDOWN)lparam;
        if (lpnmud->hdr.code == UDN_DELTAPOS) {
            int* hwnd1 = (int*)(exedit_base + 0x14d4e4);
            int* hwnd2 = (int*)(exedit_base + 0x14df0c);
            int* data1 = (int*)(exedit_base + 0x14d4c8);
            int* data2 = (int*)(exedit_base + 0x14def0);
            for (int i = 0; i < tracknum; i++) {
                if ((HWND) * (hwnd1 + i * 10) == lpnmud->hdr.hwndFrom) {
                    change_delta(data1 + i * 10, lpnmud);
                    break;
                }
                else if ((HWND) * (hwnd2 + i * 10) == lpnmud->hdr.hwndFrom) {
                    change_delta(data2 + i * 10, lpnmud);
                    break;
                }
            }
        }
    }
    return exedit_SettingDialog_WndProc(hwnd, message, wparam, lparam);
}

BOOL func_init(FILTER* fp) {
    FILTER* exeditfp = get_exeditfp(fp);
    if (exeditfp == NULL) {
        MessageBoxA(fp->hwnd, "ägí£ï“èW0.92Ç™å©Ç¬Ç©ÇËÇ‹ÇπÇÒÇ≈ÇµÇΩ", fp->name, MB_OK);
        return TRUE;
    }
    exedit_base = (int)exeditfp->dll_hinst;

    GetModuleFileNameA(fp->dll_hinst, path, MAX_PATH);
    lstrcpyA(&path[lstrlenA(path) - 4], ".ini");
    scale[0] = GetPrivateProfileIntA(section_default, "scale_0", 1, path);
    scale[1] = GetPrivateProfileIntA(section_default, "scale_0.0", 1, path);
    scale[2] = GetPrivateProfileIntA(section_default, "scale_0.00", 1, path);
    scale[3] = GetPrivateProfileIntA(section_shift, "scale_0", 1, path);
    scale[4] = GetPrivateProfileIntA(section_shift, "scale_0.0", 1, path);
    scale[5] = GetPrivateProfileIntA(section_shift, "scale_0.00", 1, path);

    exedit_SettingDialog_WndProc = *(WNDPROC*)(exedit_base + 0x2e804);
    exedit_Replace_Address(0x2e804, exedit_SettingDialog_WndProc_wrap);
    return TRUE;
}

// iniÇ…ê›íËÇï€ë∂
BOOL func_exit(FILTER* fp) {
    WritePrivateProfileStringA(section_default, "scale_0", std::to_string(scale[0]).c_str(), path);
    WritePrivateProfileStringA(section_default, "scale_0.0", std::to_string(scale[1]).c_str(), path);
    WritePrivateProfileStringA(section_default, "scale_0.00", std::to_string(scale[2]).c_str(), path);
    WritePrivateProfileStringA(section_shift, "scale_0", std::to_string(scale[3]).c_str(), path);
    WritePrivateProfileStringA(section_shift, "scale_0.0", std::to_string(scale[4]).c_str(), path);
    WritePrivateProfileStringA(section_shift, "scale_0.00", std::to_string(scale[5]).c_str(), path);
    return 0;
}

