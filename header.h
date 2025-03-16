#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <gdiplus.h>
#include <math.h>
#include <iostream>
#include <TlHelp32.h>
#include <sstream>

int swidth = GetSystemMetrics(SM_CXSCREEN);
int sheight = GetSystemMetrics(SM_CYSCREEN);



typedef struct {
	float x, y, z, w;
}vec4;

typedef struct {
	float x, y, z;
}vec3;

typedef struct {
	float x, y;
}vec2;



vec3 ehead, ebody;
vec2 screen, head;

HDC htdc;

HBRUSH color;








bool worldtoscreen(vec3 pos, vec2* screen, float matrix[16], int windowwidth, int windowheight) {
	
	vec4 clipCoords;

	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
	clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
	clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
	clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	vec3 ndc;

	ndc.x = clipCoords.x / clipCoords.w;
	ndc.y = clipCoords.y / clipCoords.w;
	ndc.z = clipCoords.z / clipCoords.w;

	screen->x = (windowwidth / 2 * ndc.x) + (ndc.x + windowwidth / 2);
	screen->y = -(windowheight / 2 * ndc.y) + (ndc.y + windowheight / 2);

	return true;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
    // Create a snapshot of the modules in the specified process
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap == INVALID_HANDLE_VALUE) return 0; // If snapshot creation fails, return 0

    // Initialize the MODULEENTRY32 structure to store module information
    MODULEENTRY32 modEntry = {};
    modEntry.dwSize = sizeof(modEntry);

    // Variable to hold the module's base address
    uintptr_t modBaseAddr = 0;

    // Retrieve information about the first module in the snapshot
    if (Module32First(hSnap, &modEntry)) {
        do {
            // Compare the module name (case insensitive) to the target module name
            if (lstrcmpiW(modEntry.szModule, modName) == 0) {
                // If the module matches, get its base address
                modBaseAddr = reinterpret_cast<uintptr_t>(modEntry.modBaseAddr);
                break; // Exit the loop once the target module is found
            }
        } while (Module32Next(hSnap, &modEntry)); // Move to the next module in the snapshot
    }

    // Close the snapshot handle to release resources
    CloseHandle(hSnap);

    // Return the base address of the module (or 0 if not found)
    return modBaseAddr;
}

void drawrect(int x, int y, int w, int h, HBRUSH color)
{
    RECT rect = { x, y, x + w, y + h };
    FillRect(htdc, &rect, color);

}
void box(int x, int y, int w, int h, int thick)
{
    drawrect(x + 2, y, w - 2, thick, color);
    drawrect(x, y, thick, h, color);
    drawrect((x + w), y, thick, h, color);
    drawrect(x, y + h, w + thick, thick, color);
}

void esp(vec3 epos, vec3 ehpos, float matrix[16])
{
    if (worldtoscreen(epos, &screen, matrix, swidth, sheight))
    {
        if (worldtoscreen(ehpos, &head, matrix, swidth, sheight))
        {
            float height = (head.y - 55.0f) - screen.y;
            float width = height / 2.0f;

            box(screen.x - (width / 2), screen.y, width, height, 2);
        }
    }
}
