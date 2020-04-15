objects = main.o softgl.o softgl_matrixstack.o mat16fv.o vec3fv.o \
          softgl_lighting_state.o softgl_material_state.o \
          softgl_texture_state.o softgl_clipdrawtri_flat.o \
          softgl_clipdrawtri_smooth.o softgl_clipdrawtri_smooth_tex.o  \
          softgl_homoclip_flat.o softgl_trirast_flat.o \
          softgl_homoclip_smooth.o softgl_trirast_smooth.o \
          softgl_homoclip_smooth_tex.o softgl_trirast_smooth_tex.o \
          softgl_trirast_smooth_pc.o softgl_trirast_smooth_tex_pc.o \
          softgl_lighting.o \
          trimodel.o
libs = -lm
includes = 
exec = main
cflags = -n32 -O3
CC=CC
.SUFFIXES: .o .cpp

.cpp.o:
	$(CC) -c $(cflags) $(includes) $*.cpp -o $*.o

$(exec): $(objects)
	$(CC) $(cflags) $(objects) $(libs) -o $(exec)

clean:
	/bin/rm -rf `find . -name "*.o"` $(exec)


# the following  was created with: "makedepend *.cpp"
