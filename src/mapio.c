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
  int fdmap = open(filename, O_WRONLY | O_TRUNC |O_CREAT, 0640);
  int width = map_width();
  int height = map_height();
  int nb_obj = map_objects();
  write(fdmap, &width, sizeof(int));
  write(fdmap, &height, sizeof(int));
  write(fdmap, &nb_obj, sizeof(int));

  //Puis on cherche chaque objets existant pour les enregistrer.
  int obj, solidity, is_destruct, is_collect, is_gen, nb_frames;
  
  for(int y = 0; y < height; ++y){
    for(int x = 0; x < width; ++x){
      obj = map_get(x, y);
      write(fdmap, &obj, sizeof(obj));
    }
  }

  //Enfin, on mémorise les propriétés des objets
  for(int i = 0; i < nb_obj; ++i){
    char *name_obj = map_get_name(i);
    int name_len = 0;
    while(name_obj[name_len] != '\0'){
      name_len++;
    }
    name_len++;
    nb_frames = map_get_frames(i);
    solidity = map_get_solidity(i);
    is_destruct = map_is_destructible(i) * MAP_OBJECT_DESTRUCTIBLE;
    is_collect = map_is_collectible(i) * MAP_OBJECT_COLLECTIBLE;
    is_gen = map_is_generator(i) * MAP_OBJECT_GENERATOR;
    
    write(fdmap, &name_len, sizeof(int));
    write(fdmap, name_obj, (name_len*sizeof(char)));
    write(fdmap, &nb_frames, sizeof(int));
    write(fdmap, &solidity, sizeof(int));
    write(fdmap, &is_destruct, sizeof(int));
    write(fdmap, &is_collect, sizeof(int));
    write(fdmap, &is_gen, sizeof(int));
  }
  
  close(fdmap);
  printf("Map successfully saved at: %s.\n", filename);
}

void map_load (char *filename)
{
  // TODO
  //exit_with_error ("Map load is not yet implemented\n");
  int width, height, nb_obj, nb_frames, obj, name_len, solidity, destruct, collect, gen;

  int fdmap = open(filename, O_RDONLY);
  if(fdmap==-1){
    exit_with_error("Impossible d'ouvrir le fichier\n");
  }
  
  read(fdmap, &width, sizeof(int));
  read(fdmap, &height, sizeof(int));
  read(fdmap, &nb_obj, sizeof(int));

  map_allocate(width, height);
  //On définit chaque case
  for(int y = 0; y < height ; ++y){
    for(int x = 0; x < width; ++x){
      read(fdmap, &obj, sizeof(int));
      map_set(x, y, obj);
    }
  }
  
  map_object_begin(nb_obj);

  //Puis on définit chaque objets.
  for(int i = 0; i < nb_obj; ++i){
    read(fdmap, &name_len, sizeof(int));
    char name_obj[name_len];
    read(fdmap, name_obj, (name_len*sizeof(char)));
    read(fdmap, &nb_frames, sizeof(int));
    read(fdmap, &solidity, sizeof(int));
    read(fdmap, &destruct, sizeof(int));
    read(fdmap, &collect, sizeof(int));
    read(fdmap, &gen, sizeof(int));
    map_object_add(name_obj, nb_frames, solidity | destruct | collect | gen);
  }

  map_object_end();
  close(fdmap);
  printf("Map successfully loaded.\n");
}

#endif
