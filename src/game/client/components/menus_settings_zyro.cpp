#include <game/localization.h>

#include <game/client/components/cheat/features/aimbot.h>
#include <game/client/gameclient.h>
#include <game/client/ui.h>
#include <game/client/ui_scrollregion.h>

#include "menus.h"

static int s_CurTab = 0;
static int s_CurAimbotTab = -1;
enum
{
	ZYRO_TAB_PAGE_1 = 0,
	NUMBER_OF_ZYRO_TABS
};

enum
{
	WEAPON_TAB_HAMMER = 0,
	WEAPON_TAB_GUN,
	WEAPON_TAB_SHOTGUN,
	WEAPON_TAB_GRENADE,
	WEAPON_TAB_LASER,
	WEAPON_TAB_NINJA,
	WEAPON_TAB_HOOK,
	NUM_WEAPON_TABS
};

int CMenus::RenderTablist(CUIRect TabBar)
{
	// Tab button containers
	static CButtonContainer s_aPageTabs[NUMBER_OF_ZYRO_TABS] = {};
	static int s_CurZyroTab = 0;

	// Calculate tab width
	float TabWidth = TabBar.w / NUMBER_OF_ZYRO_TABS;
	CUIRect Tab;

	// Render tab buttons
	for(int i = 0; i < NUMBER_OF_ZYRO_TABS; i++)
	{
		TabBar.VSplitLeft(TabWidth, &Tab, &TabBar);

		// Calculate corners for rounded edges
		int Corners = 0;
		if(i == 0)
			Corners |= IGraphics::CORNER_L;
		if(i == NUMBER_OF_ZYRO_TABS - 1)
			Corners |= IGraphics::CORNER_R;

		// Get tab text
		const char *pTabText = "";
		switch(i)
		{
		case ZYRO_TAB_PAGE_1: pTabText = "Page 1"; break;
		}

		if(DoButton_MenuTab(&s_aPageTabs[i], Localize(pTabText), s_CurZyroTab == i, &Tab, Corners))
			s_CurZyroTab = i;
	}

	return s_CurZyroTab;
}

void CMenus::RenderTabPage1(CUIRect MainView)
{
	// Split MainView to use only 1/4 of width
	CUIRect SettingsView;
	MainView.VSplitLeft(MainView.w / 4, &SettingsView, nullptr);

	CUIRect Button, Right, Row, Label;
	const float LineSize = 20.0f;
	const float Spacing = 2.0f;
	const float CheckboxWidth = 80.0f;
	const float SliderWidth = 150.0f;

	// Calculate weapon section height
	const float MaxWeaponHeight = 96.0f * 0.3f;
	const float CheckboxSize = 20.0f;
	const float CheckboxSpacing = 5.0f;
	const float TotalWeaponSectionHeight = MaxWeaponHeight + CheckboxSize + CheckboxSpacing;

	// Create weapons section
	CUIRect WeaponsSection;
	SettingsView.HSplitTop(MaxWeaponHeight, &WeaponsSection, &SettingsView);

	// Calculate weapon widths and spacing
	const int NumWeapons = 7;
	float WeaponWidths[7];
	float TotalWeaponsWidth = 0;
	const float MinSpacingFactor = 0.2f;

	for(int i = 0; i < NumWeapons; i++)
	{
		float Width;
		switch(i)
		{
		case 0: Width = 128.0f; break; // Hook
		case 1: Width = 128.0f; break; // Hammer
		case 2: Width = 128.0f; break; // Gun
		case 3: Width = 256.0f; break; // Shotgun
		case 4: Width = 256.0f; break; // Grenade
		case 5: Width = 224.0f; break; // Laser
		case 6: Width = 256.0f; break; // Ninja
		}
		WeaponWidths[i] = Width * 0.3f;
		TotalWeaponsWidth += WeaponWidths[i];
	}

	// Calculate minimum spacing based on widest weapon
	float MaxWeaponWidth = 0;
	for(int i = 0; i < NumWeapons; i++)
		MaxWeaponWidth = std::max(MaxWeaponWidth, WeaponWidths[i]);

	float MinSpacing = MaxWeaponWidth * MinSpacingFactor;
	float DesiredSpacing = (WeaponsSection.w - TotalWeaponsWidth) / (NumWeapons + 1);
	float WeaponSpacing = MinSpacing;
	float CurrentX = WeaponsSection.x;

	CUIRect WeaponRect = WeaponsSection;

	for(int i = 0; i < NumWeapons; i++)
	{
		WeaponRect.x = CurrentX;
		WeaponRect.w = WeaponWidths[i];

		switch(i)
		{
		case 0: RenderWeaponSection(WeaponRect, 6, WEAPON_TAB_HOOK, "Hook"); break;
		case 1: RenderWeaponSection(WeaponRect, WEAPON_HAMMER, WEAPON_TAB_HAMMER, "Hammer"); break;
		case 2: RenderWeaponSection(WeaponRect, WEAPON_GUN, WEAPON_TAB_GUN, "Gun"); break;
		case 3: RenderWeaponSection(WeaponRect, WEAPON_SHOTGUN, WEAPON_TAB_SHOTGUN, "Shotgun"); break;
		case 4: RenderWeaponSection(WeaponRect, WEAPON_GRENADE, WEAPON_TAB_GRENADE, "Grenade"); break;
		case 5: RenderWeaponSection(WeaponRect, WEAPON_LASER, WEAPON_TAB_LASER, "Laser"); break;
		case 6: RenderWeaponSection(WeaponRect, WEAPON_NINJA, WEAPON_TAB_NINJA, "Ninja"); break;
		}

		CurrentX += WeaponWidths[i] + WeaponSpacing;
	}

	SettingsView.HSplitTop(Spacing, nullptr, &SettingsView);

	// Show weapon-specific settings when a weapon is selected
	if(s_CurAimbotTab != -1)
	{
		int WeaponId = s_CurAimbotTab;
		const WeaponConfig *pConfig = m_pClient->m_Aimbot.GetWeaponConfig(WeaponId);

		// Create row for aim checkbox
		SettingsView.HSplitTop(LineSize, &Row, &SettingsView);

		// Aim checkbox (enables aimbot for this weapon)
		Row.VSplitLeft(CheckboxWidth, &Button, &Right);
		if(DoButton_CheckBox(pConfig->m_pEnabled, Localize("aim"), *pConfig->m_pEnabled, &Button))
			*pConfig->m_pEnabled ^= 1;

		// Draw FOV checkbox (no text)
		Right.VSplitLeft(20.0f, &Button, &Right);
		if(DoButton_CheckBox(&g_Config.m_ZrAimbotDrawFov, "", g_Config.m_ZrAimbotDrawFov, &Button))
			g_Config.m_ZrAimbotDrawFov ^= 1;

		// Global FOV slider
		Right.VSplitLeft(5.0f, nullptr, &Right);
		Right.VSplitLeft(SliderWidth, &Button, &Right);
		Ui()->DoScrollbarOption(&g_Config.m_ZrAimbotFoV, &g_Config.m_ZrAimbotFoV, &Button, Localize("fov"), 1, 360, &CUi::ms_LinearScrollbarScale, CUi::SCROLLBAR_OPTION_NOCLAMPVALUE, "°");

		// Aimbot mode dropdown
		Right.VSplitLeft(20.0f, nullptr, &Right);
		Right.VSplitLeft(80.0f, &Button, &Right);
		const char *apAimbotModes[] = {"Plain", "Silent"};
		static CUi::SDropDownState s_AimbotModeDropDownState;
		static CScrollRegion s_AimbotModeDropDownScrollRegion;
		s_AimbotModeDropDownState.m_SelectionPopupContext.m_pScrollRegion = &s_AimbotModeDropDownScrollRegion;
		g_Config.m_ZrAimbotMode = Ui()->DoDropDown(&Button, g_Config.m_ZrAimbotMode, apAimbotModes, std::size(apAimbotModes), s_AimbotModeDropDownState);

		// Accuracy settings for specific weapons
		if(WeaponId == WEAPON_TAB_HOOK) // Hook
		{
			SettingsView.HSplitTop(Spacing, nullptr, &SettingsView);
			SettingsView.HSplitTop(LineSize, &Row, &SettingsView);
			Row.VSplitLeft(SliderWidth, &Button, &Right);
			Ui()->DoScrollbarOption(&g_Config.m_ZrAimbotHookAccuracy, &g_Config.m_ZrAimbotHookAccuracy, &Button,
				Localize("accuracy"), 1, 200, &CUi::ms_LinearScrollbarScale,
				CUi::SCROLLBAR_OPTION_NOCLAMPVALUE, "%");
		}
		else if(WeaponId == WEAPON_LASER)
		{
			SettingsView.HSplitTop(Spacing, nullptr, &SettingsView);
			SettingsView.HSplitTop(LineSize, &Row, &SettingsView);

			// First row: bounce and bounce-only checkboxes
			Row.VSplitLeft(CheckboxWidth, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_ZrAimbotLaserUseBounce, Localize("bounce"), g_Config.m_ZrAimbotLaserUseBounce, &Button))
				g_Config.m_ZrAimbotLaserUseBounce ^= 1;

			if(g_Config.m_ZrAimbotLaserUseBounce)
			{
				// Bounce only checkbox on same row
				Right.VSplitLeft(CheckboxWidth + 20.0f, &Button, &Right);
				if(DoButton_CheckBox(&g_Config.m_ZrAimbotLaserBounceOnly, Localize("bounce only"), g_Config.m_ZrAimbotLaserBounceOnly, &Button))
					g_Config.m_ZrAimbotLaserBounceOnly ^= 1;

				// Second row: bounce count and path selection
				SettingsView.HSplitTop(Spacing, nullptr, &SettingsView);
				SettingsView.HSplitTop(LineSize, &Row, &SettingsView);

				// Bounce count slider
				Row.VSplitLeft(SliderWidth, &Button, &Right);
				Ui()->DoScrollbarOption(&g_Config.m_ZrAimbotLaserBounceCount, &g_Config.m_ZrAimbotLaserBounceCount, &Button,
					Localize("bounce count"), 1, 10, &CUi::ms_LinearScrollbarScale,
					CUi::SCROLLBAR_OPTION_NOCLAMPVALUE);

				// Path selection dropdown
				Right.VSplitLeft(20.0f, nullptr, &Right);
				Right.VSplitLeft(120.0f, &Button, &Right);
				const char *apBouncePaths[] = {"closest", "furthest", "random"};
				static CUi::SDropDownState s_BouncePathDropDownState;
				static CScrollRegion s_BouncePathDropDownScrollRegion;
				s_BouncePathDropDownState.m_SelectionPopupContext.m_pScrollRegion = &s_BouncePathDropDownScrollRegion;
				g_Config.m_ZrAimbotLaserBouncePath = Ui()->DoDropDown(&Button, g_Config.m_ZrAimbotLaserBouncePath, apBouncePaths, std::size(apBouncePaths), s_BouncePathDropDownState);
			}
		}
	}

	// Visual settings (plr box and plr tracer on same row)
	SettingsView.HSplitTop(Spacing, nullptr, &SettingsView);
	SettingsView.HSplitTop(LineSize, &Row, &SettingsView);
	Row.VSplitLeft(CheckboxWidth, &Button, &Right);
	if(DoButton_CheckBox(&g_Config.m_ZrPlrBox, Localize("plr box"), g_Config.m_ZrPlrBox, &Button))
		g_Config.m_ZrPlrBox ^= 1;

	Right.VSplitLeft(CheckboxWidth, &Button, &Right);
	if(DoButton_CheckBox(&g_Config.m_ZrPlrTracer, Localize("plr tracer"), g_Config.m_ZrPlrTracer, &Button))
		g_Config.m_ZrPlrTracer ^= 1;

	// Discord settings (RPC checkbox and dropdown on same row)
	SettingsView.HSplitTop(Spacing, nullptr, &SettingsView);
	SettingsView.HSplitTop(LineSize, &Row, &SettingsView);
	Row.VSplitLeft(CheckboxWidth, &Button, &Right);
	if(DoButton_CheckBox(&g_Config.m_ZrDiscordRPC, Localize("rpc"), g_Config.m_ZrDiscordRPC, &Button))
		g_Config.m_ZrDiscordRPC ^= 1;

	Right.VSplitLeft(80.0f, &Button, &Right);
	const char *apDiscordApps[] = {"ddnet", "tater", "cff", "krx", "zyro"};
	static CUi::SDropDownState s_DiscordDropDownState;
	static CScrollRegion s_DiscordDropDownScrollRegion;
	s_DiscordDropDownState.m_SelectionPopupContext.m_pScrollRegion = &s_DiscordDropDownScrollRegion;
	g_Config.m_ZrDiscord = Ui()->DoDropDown(&Button, g_Config.m_ZrDiscord, apDiscordApps, std::size(apDiscordApps), s_DiscordDropDownState);
}

void CMenus::RenderWeaponSection(CUIRect &WeaponsSection, int WeaponId, int TabId, const char *Label)
{
	float WeaponWidth, WeaponHeight;

	// Set proper weapon sizes with correct proportions
	// These are the original sizes of the weapons taken from https://teedata.net/template/gameskin_clear
	switch(WeaponId)
	{
	case WEAPON_HAMMER:
		WeaponWidth = 128.0f;
		WeaponHeight = 96.0f;
		break;
	case WEAPON_GUN:
		WeaponWidth = 128.0f;
		WeaponHeight = 64.0f;
		break;
	case WEAPON_SHOTGUN:
		WeaponWidth = 256.0f;
		WeaponHeight = 64.0f;
		break;
	case WEAPON_GRENADE:
		WeaponWidth = 256.0f;
		WeaponHeight = 64.0f;
		break;
	case WEAPON_LASER:
		WeaponWidth = 224.0f;
		WeaponHeight = 96.0f;
		break;
	case WEAPON_NINJA:
		WeaponWidth = 256.0f;
		WeaponHeight = 64.0f;
		break;
	case WEAPON_TAB_HOOK:
		WeaponWidth = 128.0f;
		WeaponHeight = 32.0f;
		break;
	}

	// Scale down weapons to fit
	const float ScaleFactor = 0.3f;
	WeaponWidth *= ScaleFactor;
	WeaponHeight *= ScaleFactor;

	CUIRect WeaponRect, HighlightRect;
	static CButtonContainer s_aWeaponButtons[7] = {};

	// Create weapon rect with proper dimensions
	WeaponRect = WeaponsSection;
	WeaponRect.w = WeaponWidth;
	WeaponRect.x = WeaponsSection.x;
	WeaponRect.y += (96.0f * 0.3f - WeaponHeight) / 2;
	WeaponRect.h = WeaponHeight;

	HighlightRect = WeaponRect;

	if(s_CurAimbotTab == TabId)
		HighlightRect.Draw(ColorRGBA(1, 1, 1, 0.25f), IGraphics::CORNER_ALL, 5.0f);

	// Render weapon
	DoWeaponPreview(&WeaponRect, WeaponId);

	// Make entire section clickable using button logic
	if(Ui()->DoButtonLogic(&s_aWeaponButtons[TabId], s_CurAimbotTab == TabId, &WeaponsSection))
	{
		if(s_CurAimbotTab == TabId)
			s_CurAimbotTab = -1;
		else
			s_CurAimbotTab = TabId;
	}
}

void CMenus::DoWeaponPreview(const CUIRect *pRect, int WeaponID)
{
	float Width = pRect->w;
	float Height = pRect->h;
	float x = pRect->x + Width / 2;
	float y = pRect->y + Height / 2;

	if(WeaponID == WEAPON_TAB_HOOK)
	{
		// Draw hook chain first
		Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpriteHookChain);
		Graphics()->QuadsBegin();
		Graphics()->QuadsSetSubset(0, 0, 1, 1);
		Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

		// Chain segments - render multiple small segments for better look
		const float ChainWidth = Width * 0.1f; // Thinner chain
		const float ChainLength = Width * 0.6f; // Total chain length
		const int NumSegments = 3;
		const float SegmentLength = ChainLength / NumSegments;

		for(int i = 0; i < NumSegments; i++)
		{
			float SegX = x - ChainLength / 2 + i * SegmentLength;
			IGraphics::CQuadItem QuadItem(SegX, y - ChainWidth / 2, SegmentLength, ChainWidth);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
		}
		Graphics()->QuadsEnd();

		// Draw hook head
		Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpriteHookHead);
		Graphics()->QuadsBegin();
		Graphics()->QuadsSetSubset(0, 0, 1, 1);
		Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

		// Head at the end of chain
		float HeadSize = Width * 0.25f;
		IGraphics::CQuadItem QuadItem(x + ChainLength / 2 - HeadSize / 2, y - HeadSize / 2, HeadSize, HeadSize);
		Graphics()->QuadsDrawTL(&QuadItem, 1);
		Graphics()->QuadsEnd();
		return;
	}

	// Regular weapon rendering
	IGraphics::CTextureHandle WeaponTexture;

	switch(WeaponID)
	{
	case WEAPON_HAMMER:
		WeaponTexture = m_pClient->m_GameSkin.m_SpriteWeaponHammer;
		break;
	case WEAPON_GUN:
		WeaponTexture = m_pClient->m_GameSkin.m_SpriteWeaponGun;
		break;
	case WEAPON_SHOTGUN:
		WeaponTexture = m_pClient->m_GameSkin.m_SpriteWeaponShotgun;
		break;
	case WEAPON_GRENADE:
		WeaponTexture = m_pClient->m_GameSkin.m_SpriteWeaponGrenade;
		break;
	case WEAPON_LASER:
		WeaponTexture = m_pClient->m_GameSkin.m_SpriteWeaponLaser;
		break;
	case WEAPON_NINJA:
		WeaponTexture = m_pClient->m_GameSkin.m_SpriteWeaponNinja;
		break;
	default:
		return;
	}

	Graphics()->TextureSet(WeaponTexture);
	Graphics()->QuadsBegin();
	Graphics()->QuadsSetSubset(0, 0, 1, 1);
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	IGraphics::CQuadItem QuadItem(x - Width / 2, y - Height / 2, Width, Height);
	Graphics()->QuadsDrawTL(&QuadItem, 1);
	Graphics()->QuadsEnd();
}

void CMenus::RenderSettingsZyro(CUIRect MainView)
{
	// Constants for layout
	const float LineSize = 20.0f;
	const float SectionMargin = 5.0f;

	// Create tab bar
	CUIRect TabBar;
	MainView.HSplitTop(LineSize, &TabBar, &MainView);

	// Update current tab from tab list
	s_CurTab = RenderTablist(TabBar);

	MainView.HSplitTop(SectionMargin, nullptr, &MainView);

	// Add margins
	MainView.Margin(SectionMargin, &MainView);

	// Render current tab content
	switch(s_CurTab)
	{
	case ZYRO_TAB_PAGE_1:
		RenderTabPage1(MainView);
		break;
	}
}