void generate_files(int do_io);
typedef struct {
  int(*xcoords), (*ycoords);
} Points;

Points init_points();