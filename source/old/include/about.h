static const char* str_about = 
"JackUtils - collection of useful jack utilities and simplified API.\n"
"Copyright (C) 2022 UtoECat\n"
"\n"
"This program is free software: you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License as published by\n"
"the Free Software Foundation, either version 3 of the License, or\n"
"(at your option) any later version.\n"
"\n"
"This program is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License for more details.\n"
"\n"
"You should have received a copy of the GNU General Public License\n"
"along with this program.  If not, see <https://www.gnu.org/licenses/>.\n";

static int program_about = 0;

static void show_about() {
	program_about = 1;
}


/*
 * Put this function AFTER enumeration of your stuff
 */
static void process_about(jg_ctx_t* gui) {
// about popup
	if (program_about) {
		// this stops enumerating active main window! A bit dirty thing!
		nk_end(gui);
		const int flags = NK_WINDOW_CLOSABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MOVABLE | NK_WINDOW_BORDER | NK_WINDOW_TITLE;
		static struct nk_rect rect = {20, 20, 480, 320};
		if (nk_begin(gui, "about", rect, flags)) {
			// draw icon
			nk_layout_row_template_begin(gui, 64);
			nk_layout_row_template_push_variable(gui, 80);
			nk_layout_row_template_push_static(gui, 64);
			nk_layout_row_template_push_variable(gui, 80);
			nk_layout_row_template_end(gui);

			nk_label(gui, "", NK_TEXT_CENTERED); // dummy
			nk_image(gui, jg_jackutils_icon());
			nk_label(gui, "", NK_TEXT_CENTERED); // dummy
			// draw text
			nk_layout_row_dynamic(gui, 15, 1);
			size_t pos = 0, len = strlen(str_about);
			for (size_t i = pos; i < len; i++) if (str_about[i] == '\n') {
				size_t cnt = i - pos;
				char buff[cnt + 1];
				memcpy(buff, str_about + pos, cnt);
				buff[cnt] = '\0';
				nk_label(gui, buff, NK_TEXT_CENTERED);
				pos = i + 1;
			}
			// should be closed by jg_end()!
		} else {
			program_about = 0;
		}
	}
}
