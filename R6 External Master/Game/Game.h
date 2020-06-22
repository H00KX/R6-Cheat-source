/*

	Declerations for features and whatnot

*/

#pragma once

#include "../Globals.h"

namespace RainbowSix
{
	uintptr_t GetLocalPlayer();
	void Esp(bool enable);
	void GlowEsp(bool enable);
	void SpecialGlowBlue(bool enable);
	void SpecialGlowPurple(bool enable);
	void NoFlash(bool enable);
	void CharacterFOV(float fov);
	void WeaponFOV(float fov);
	void Recoil();
}

// For outline
BYTE GetRoundState();
void SetColour(int colour);
bool IsScanning();
BYTE GetArray();
void WriteArray(BYTE writethis);
void SwapTeams();
void SetOutline();