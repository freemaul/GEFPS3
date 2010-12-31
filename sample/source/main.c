/*
    This file is part of GEFPS3.

    GEFPS3 is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GEFPS3 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with GEFPS3.  If not, see <http://www.gnu.org/licenses/>.
*/

//standars libs
#include <assert.h>
#include <math.h>

#include <sysutil/video.h>
#include <rsx/reality.h>
#include <io/pad.h>
#include <lv2/process.h>

#include <cairo/cairo.h>

#include <GEFPS3/GEFPS3.h>

typedef struct {
	int height;
	int width;
	uint32_t *ptr;
	// Internal stuff
	uint32_t offset;
} buffer;

gcmContextData *context; // Context to keep track of the RSX buffer.
VideoResolution res; // Screen Resolution
console *cons;

int currentBuffer = 0;
buffer *buffers[2]; // The buffer we will be drawing into.

void waitFlip() { // Block the PPU thread untill the previous flip operation has finished.
	while(gcmGetFlipStatus() != 0) 
		usleep(200);  // Sleep, to not stress the cpu.
	gcmResetFlipStatus();
}

// Flip a buffer onto the screen.
void flip(s32 buffer) {
	assert(gcmSetFlip(context, buffer) == 0);
	realityFlushBuffer(context);
	gcmSetWaitFlip(context); // Prevent the RSX from continuing until the flip has finished.
}

void makeBuffer(int id, int size) {
	buffer *buf = malloc(sizeof(buffer));
	buf->ptr = rsxMemAlign(16, size);
	assert(buf->ptr != NULL);

	assert(realityAddressToOffset(buf->ptr, &buf->offset) == 0);
	// Register the display buffer with the RSX
	assert(gcmSetDisplayBuffer(id, buf->offset, res.width * 4, res.width, res.height) == 0);
	
	buf->width = res.width;
	buf->height = res.height;
	buffers[id] = buf;
}

// Initilize everything. You can probally skip over this function.
void init_screen() {
	// Allocate a 1Mb buffer, alligned to a 1Mb boundary to be our shared IO memory with the RSX.
	void *host_addr = memalign(1024*1024, 1024*1024);
	assert(host_addr != NULL);

	// Initilise Reality, which sets up the command buffer and shared IO memory
	context = realityInit(0x10000, 1024*1024, host_addr); 
	assert(context != NULL);

	VideoState state;
	assert(videoGetState(0, 0, &state) == 0); // Get the state of the display
	assert(state.state == 0); // Make sure display is enabled

	// Get the current resolution
	assert(videoGetResolution(state.displayMode.resolution, &res) == 0);
	
	// Configure the buffer format to xRGB
	VideoConfiguration vconfig;
	memset(&vconfig, 0, sizeof(VideoConfiguration));
	vconfig.resolution = state.displayMode.resolution;
	vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
	vconfig.pitch = res.width * 4;
	vconfig.aspect=state.displayMode.aspect;

	assert(videoConfigure(0, &vconfig, NULL, 0) == 0);
	assert(videoGetState(0, 0, &state) == 0); 

	s32 buffer_size = 4 * res.width * res.height; // each pixel is 4 bytes
	printf("buffers will be 0x%x bytes\n", buffer_size);
	
	gcmSetFlipMode(GCM_FLIP_VSYNC); // Wait for VSYNC to flip

	// Allocate two buffers for the RSX to draw to the screen (double buffering)
	makeBuffer(0, buffer_size);
	makeBuffer(1, buffer_size);

	gcmResetFlipStatus();
	flip(1);
}

// Draw a single frame, do all your drawing/animation from in here.
void drawFrame(buffer *buffer, long frame) {
	cairo_t *cr;

	/* We are just going to attach cairo directly to the buffer in the rsx memory.
	 * If this gets too slow later with blending, we will need to create a buffer
         * on cell then copy the finished result accross.
	 */
	cairo_surface_t *surface = cairo_image_surface_create_for_data((u8 *) buffer->ptr, 
		CAIRO_FORMAT_RGB24, buffer->width, buffer->height, buffer->width * 4);
	assert(surface != NULL);
	cr = cairo_create(surface);
	assert(cr != NULL);

	// Lets start by clearing everything
	cairo_set_source_rgb (cr, 0.0, 1.0, 1.0); // White
	cairo_paint(cr);

	cairo_set_line_width(cr, 30.0); // 30 pixel wide line
	cairo_set_source_rgb (cr, 1.0, 0.5, 0.0); // Orange

	float angle = (frame % 600) / 300.0; // Rotation animaton, rotate once every 10 seconds.
	cairo_arc(cr, res.width/2, res.height/2, res.height/3, angle*M_PI, (angle-0.3)*M_PI);
	cairo_stroke(cr); // Stroke the arc with our 30px wide orange line

	cairo_destroy(cr); // Realease Surface
	cairo_surface_finish(surface);
	cairo_surface_destroy(surface); // Flush and destroy the cairo surface

	Console_draw(cons,(char*)buffer->ptr,buffer->width,buffer->height);
}

s32 main(s32 argc, const char* argv[])
{
	PadInfo padinfo;
	PadData paddata;
	int i;
	int exit=0;
	
	init_screen();
	ioPadInit(7);

	long frame = 0; // To keep track of how many frames we have rendered.


	console_attribute cons_attr=CONSOLE_ATTR_DEFAULT;
	cons_attr.textsize 	= 50;
	cons_attr.height	= 500;
	cons_attr.width		= 500;
	cons_attr.posx		= 300;
	cons_attr.posy		= 300;
	cons_attr.mode		= ARGB;

	FT_Init_FreeType( &(cons_attr.ft_library) );
	FT_New_Face( cons_attr.ft_library, "/dev_hdd0/tmp/arial.ttf", 0, &(cons_attr.ft_face) );

	cons = Create_console(cons_attr);
	if(cons == NULL)
		return 0;

	Console_printf(cons,"ta\no");

	// Ok, everything is setup. Now for the main loop.

	int hit_square=0;
	int hit_cross=0;
	int hit_circle = 0;
	int hit_triangle = 0;

	while(exit==0){
		// Check the pads.
		ioPadGetInfo(&padinfo);
		for(i=0; i<MAX_PADS; i++){
			if(padinfo.status[i]){
				ioPadGetData(i, &paddata);
				if(paddata.BTN_SELECT){
					exit=1;
				}
				
				if(paddata.BTN_START){
					return 0;
				}


				if(paddata.BTN_TRIANGLE){
					if(hit_triangle==0)
					{
						Console_clear(cons);
					}
					hit_triangle=1;
				}
				else
					hit_triangle=0;

				if(paddata.BTN_CIRCLE){
					if(hit_circle==0)
					{
						Console_printf(cons,"(//)");
					}
					hit_circle=1;
				}
				else
					hit_circle=0;


				if(paddata.BTN_CROSS){
					if(hit_cross==0)
					{
						Console_printf(cons,"XX\n");
					}
					hit_cross=1;
				}
				else
					hit_cross=0;

				if(paddata.BTN_SQUARE){
					if(hit_square==0)
					{
						Console_printf(cons,"[]");
					}
					hit_square=1;
				}
				else
					hit_square=0;
			}
			
		}

		waitFlip(); // Wait for the last flip to finish, so we can draw to the old buffer
		drawFrame(buffers[currentBuffer], frame++); // Draw into the unused buffer
		flip(currentBuffer); // Flip buffer onto screen
		currentBuffer = !currentBuffer; 
	}

	char **launchargv=NULL;
	char **launchenvp=NULL;
	char *bootpath="/dev_hdd0/tmp/REALps3load.self";

	sysProcessExitSpawn2(bootpath, (const char**)launchargv, (const char**)launchenvp, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M);
	
	return 0;
}


