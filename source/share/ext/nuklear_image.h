/*
 * I don't know why in WHOLE glfw3 and nuklear api function to load images does not exist... :/
 */

#include <ext/gl.h>

JG_API struct nk_image jg_image_load_from_memory(const unsigned char *ptr, int w, int h, int ch)
{
    struct nk_image s;
    nk_zero(&s, sizeof(s));
		GLuint id;
		GLenum fmt = GL_RED;
		switch(ch) {
			case 1: break;
			case 2: fmt = GL_RG; break;
			case 3: fmt = GL_RGB; break;
			case 4: fmt = GL_RGBA; break;
			default: 
				fmt = 0;
				return s; // error
		};

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, (GLsizei)w, (GLsizei)h, 0, fmt, GL_UNSIGNED_BYTE, ptr);
		s.handle.id = id;
    //s.w = w; s.h = h;
    s.region[0] = 0;
    s.region[1] = 0;
    s.region[2] = 0;
    s.region[3] = 0;
    return s;
}

JG_API void jg_image_free(struct nk_image img) {
	GLuint id = img.handle.id;
	if (!id) return;
	glDeleteTextures(1, &id);
}
