#pragma once
#include "Globals.h"
#include "Game/Game.h"
#include <iostream>
#include <thread>
#include "Game/aimbot/Aimbot.h"

// Outline check function call
bool ChangedTeams = false;
bool HasDoneThisRound = false;
void UpdateOutline()
{
	if (Options::bOutlineESPStatus)
	{
		SetOutline();
		/*BYTE RoundState = GetRoundState();
		if (!HasDoneThisRound && RoundState == 3)
		{
			BYTE ArrayWrite = NULL;
			if (!ChangedTeams)
			{
				bool IsScan = IsScanning();
				if (IsScan)
				{
					SetColour(0);
					ArrayWrite = GetArray();
					std::this_thread::sleep_for(std::chrono::seconds(2));
					WriteArray(ArrayWrite);
				}

				if (ArrayWrite != 0)
				{
					std::this_thread::sleep_for(std::chrono::seconds(3));
					SwapTeams();
					ChangedTeams = true;
					HasDoneThisRound = true;
					SetColour(1);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		if (RoundState != 3)
		{
			HasDoneThisRound = false;
			ChangedTeams = false;
		}*/
	}
	else
	{
		return;
	}
}

// Glow check function call
void UpdateFeatures()
{
	RainbowSix::Esp(Options::CavESPStat);
	RainbowSix::GlowEsp(Options::f_GlowStat);
	RainbowSix::Recoil();
	RainbowSix::SpecialGlowBlue(Options::f_BGlowStat);
	RainbowSix::SpecialGlowPurple(Options::f_PGlowStat);
	RainbowSix::NoFlash(Options::f_NoFlash);
	RainbowSix::WeaponFOV(Options::f_WeaponFov);
	RainbowSix::CharacterFOV(Options::f_CharFov);
	RainbowSix::Aimbot(Options::f_Aimbot);
	return;
}
