void write_graph_file();
typedef struct {
  int(*xcoords), (*ycoords);
} Points;

Points init_points();