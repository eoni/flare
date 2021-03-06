/*
Copyright 2011 Clint Bellanger

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * MenuExperience
 *
 * Handles the display of the Experience bar on the HUD
 */

#include "MenuExperience.h"
#include "SharedResources.h"
#include "WidgetLabel.h"

#include <SDL_mixer.h>

#include <sstream>


MenuExperience::MenuExperience() {
	loadGraphics();
	
	
	// move these settings to a config file
	hud_position.x = 0;
	hud_position.y = 32;
	hud_position.w = 106;
	hud_position.h = 26;
	background_offset.x = 0;
	background_offset.y = 0;
	background_size.x = 106;
	background_size.y = 10;
	bar_offset.x = 3;
	bar_offset.y = 3;
	bar_size.x = 100;
	bar_size.y = 4;
	text_offset.x = 2;
	text_offset.y = 12;
	text_justify = JUSTIFY_LEFT;
}

void MenuExperience::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/menu_xp.png").c_str());
	bar = IMG_Load(mods->locate("images/menus/bar_xp.png").c_str());
	
	if(!background || !bar) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		Mix_CloseAudio();
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);	
	
	cleanup = bar;
	bar = SDL_DisplayFormatAlpha(bar);
	SDL_FreeSurface(cleanup);
}

/**
 * Display the XP bar background and current progress.
 * On mouseover, display progress in text form.
 */
void MenuExperience::render(StatBlock *stats, Point mouse) {

	SDL_Rect src;
	SDL_Rect dest;
	int xp_bar_length;

	// don't display anything if max level
	if (stats->level < 1 || stats->level == MAX_CHARACTER_LEVEL) return;
	
	// lay down the background image first
	src.x = 0;
	src.y = 0;
	src.w = background_size.x;
	src.h = background_size.y;
	dest.x = hud_position.x + background_offset.x;
	dest.y = hud_position.y + background_offset.y;
	SDL_BlitSurface(background, &src, screen, &dest);
	
	// calculate the length of the xp bar
	// when at a new level, 0% progress
	src.x = 0;
	src.y = 0;
	int required = stats->xp_table[stats->level] - stats->xp_table[stats->level-1];
	int current = stats->xp - stats->xp_table[stats->level-1];
	xp_bar_length = (current * bar_size.x) / required;
	src.w = xp_bar_length;
	src.h = bar_size.y;
	dest.x = hud_position.x + bar_offset.x;
	dest.y = hud_position.y + bar_offset.y;
		
	// draw xp bar
	SDL_BlitSurface(bar, &src, screen, &dest);


	string text_label;
	
	// if mouseover, draw text
	if (isWithin(hud_position, mouse)) {

		if (stats->level < MAX_CHARACTER_LEVEL) {
			text_label = msg->get("XP: %d/%d", stats->xp, stats->xp_table[stats->level]);
		}
		else {
			text_label = msg->get("XP: %d", stats->xp);
		}

		WidgetLabel label;
		label.set(hud_position.x + text_offset.x, hud_position.y + text_offset.y, text_justify, VALIGN_TOP, text_label, FONT_WHITE);
		label.render();
	}
}

MenuExperience::~MenuExperience() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(bar);
}

