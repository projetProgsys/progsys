#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

void map_new (unsigned width, unsigned height)
{
  map_allocate (width, height);

  for (int x = 0; x < width; x++)
    map_set (x, height - 1, 0); // Ground

  for (int y = 0; y < height - 1; y++) {
    map_set (0, y, 1); // Wall
    map_set (width - 1, y, 1); // Wall
  }

  map_object_begin (6);

  // Texture pour le sol
  map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
  // Mur
  map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
  // Gazon
  map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
  // Marbre
  map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
  // Fleur
  map_object_add ("images/flower.png", 1, MAP_OBJECT_AIR);
  // Pieces
  map_object_add ("images/coin.png", 20, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE);

  map_object_end ();

}

void map_save (char *filename)
{
  // TODO
  //fprintf (stderr, "Sorry: Map save is not yet implemented\n");
  //En premier lieu incrit les infos générales de la carte
  FILE* fdmap = fopen(filename, "w+");
  unsigned width = map_width();
  unsigned height = map_height();
  unsigned nb_obj = map_objects();
  fwrite(&width, sizeof(width), 1, fdmap);
  fwrite(&height, sizeof(height), 1, fdmap);
  fwrite(&nb_obj, sizeof(nb_obj), 1, fdmap);

  //Puis on cherche chaque objets existant pour les enregistrer.
  int name_size, solidity, is_destruct, is_collect, is_gen;
  unsigned nb_frames;
  char *name_obj;
  for(int x = 0; x < width ; ++x){
    for(int y = 0; y < height; ++y){
      int obj = map_get(x, y);
      if(obj != MAP_OBJECT_NONE){
	name_size = strlen(map_get_name(obj));
	name_obj = malloc (name_size * sizeof(char));
	strcpy (name_obj, map_get_name(obj));
	
	nb_frames = map_get_frames(obj);
	solidity = map_get_solidity(obj);
	is_destruct = map_is_destructible(obj);
	is_collect = map_is_collectible(obj);
	is_gen = map_is_generator(obj);
	
	fwrite(&obj, sizeof(obj), 1, fdmap);
	fwrite(&x, sizeof(x), 1, fdmap);
	fwrite(&y, sizeof(y), 1, fdmap);
	fwrite(&name_size, sizeof(name_size), 1, fdmap);
	fwrite(&name_obj, sizeof(name_obj), name_size,fdmap);
	fwrite(&nb_frames, sizeof(nb_frames), 1, fdmap);
	fwrite(&solidity, sizeof(solidity), 1, fdmap);
	fwrite(&is_destruct, sizeof(is_destruct), 1, fdmap);
	fwrite(&is_collect, sizeof(is_collect), 1, fdmap);
	fwrite(&is_gen, sizeof(is_gen), 1, fdmap);
	
	free(name_obj);
	}
    }
  }
  
  fclose(fdmap);
  printf("Map successfully saved at: %s.\n", filename);
}

void map_load (char *filename)
{
  // TODO
  exit_with_error ("Map load is not yet implemented\n");
}

#endif
