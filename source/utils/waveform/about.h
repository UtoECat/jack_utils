#pragma once

static const char* __info_contributors[] = {
	"Kuts Egor Pavlovich (UtoECat) <utopia.egor.cat.allandall@gmail.com>",
	NULL
};

static const char* const __info_license = 
"Copyright (C) 2022 Kuts Egor Pavlovich (UtoECat)\n"
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
"along with this program.  If not, see <https://www.gnu.org/licenses/>."
;

const struct program_info program_info = {
	.name = "waveform",
	.description = "jackutils waveform visualizer",
	.author = "UtoECat",
	.license = __info_license,
	.contributors = __info_contributors,
	.version = 0.3,
	.source_url = "https://github.com/UtoECat/jack_utils"
};
