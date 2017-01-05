#define _XOPEN_SOURCE 600
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>



void get_width(char *filename){
  int fd = open(filename, O_RDONLY);
  if(fd==-1){
    fprintf(stderr, "Erreur ouverture du fichier\n");
    exit(1);
  }
  int width;
  read(fd, &width, sizeof(int));
  close(fd);
  printf("Map Width : %d\n", width);
}

void get_height(char *filename){
  int fd = open(filename, O_RDONLY);
  if(fd==-1){
    fprintf(stderr, "Erreur ouverture du fichier\n");
    exit(1);
  }
  int height;
  lseek(fd, sizeof(int), SEEK_SET);
  read(fd, &height, sizeof(int));
  close(fd);
  printf("Map Height : %d\n", height);
}

void get_objects(char *filename){
  int fd = open(filename, O_RDONLY);
  if(fd==-1){
    fprintf(stderr, "Erreur ouverture du fichier\n");
    exit(1);
  }
  int nb_obj;
  lseek(fd, sizeof(int)*2, SEEK_SET);
  read(fd, &nb_obj, sizeof(int));
  close(fd);
  printf("Map number of objects: %d\n", nb_obj);
}

void get_info(char *filename){
  int fd = open(filename, O_RDONLY);
  if(fd==-1){
    fprintf(stderr, "Erreur ouverture du fichier\n");
    exit(1);
  }
  int w, h, o;
  read(fd, &w, sizeof(int));
  read(fd, &h, sizeof(int));
  read(fd, &o, sizeof(int));
  close(fd);
  printf("Map infos : %dx%d with %d objects\n", w, h, o);
}

void replace_content(int dst, int src){
  char buf[1000];
  int r;
  int data;
  lseek(src, 0, SEEK_SET);
  lseek(dst, 0, SEEK_SET);
  while((r = read(src, buf, 1000)) > 0){
    write(dst, buf, r);
    data+=r;
  }
  ftruncate(dst, data);
}

void set_width(char *filename, int new_w){
  int fd = open(filename, O_RDWR);
  if(fd==-1){
    fprintf(stderr, "Erreur ouverture du fichier\n");
    exit(1);
  }
  int old_w, old_h, nb_items, buf_int;
  int map_none = -1;
  read(fd, &old_w, sizeof(int));
  if(old_w == new_w){
    close(fd);
    fprintf(stderr, "Same width");
    exit(1);
  }
  int tmp = open("tmp.map", O_RDWR | O_CREAT);
  if(fd==-1){
    fprintf(stderr, "Erreur ouverture du fichier\n");
    exit(1);
  }
  read(fd, &old_h, sizeof(int));
  write(tmp, &new_w, sizeof(int));
  write(tmp, &old_h, sizeof(int));
  read(fd, &nb_items, sizeof(int));
  write(tmp, &nb_items, sizeof(int));
  if(new_w > old_w){
    for(int y = 0; y < old_h; ++y){
      for(int x = 0; x < new_w; ++x){
        if(x < old_w){
	  read(fd, &buf_int, sizeof(int));
	  write(tmp, &buf_int, sizeof(int));
	}
	else{
	  write(tmp, &map_none, sizeof(int));
	}
      }
    }
    char buf[4096];
    int r;
    while((r = read(fd, buf, 4096)) > 0){
      write(tmp, buf, r);
    }
  }
  replace_content(fd, tmp);
  close(fd);
  close(tmp);
  remove("tmp.map");
}

void set_height(char *filename, int new_h){
  int fd = open(filename, O_RDWR);
  if(fd==-1){
    fprintf(stderr, "Erreur ouverture du fichier\n");
    exit(1);
  }
  int old_w, old_h, nb_items, buf_int;
  int map_none = -1;
  read(fd, &old_w, sizeof(int));
  if(old_h == new_h){
    close(fd);
    fprintf(stderr, "Same width");
    exit(1);
  }
  int tmp = open("tmp.map", O_RDWR | O_CREAT);
  if(fd==-1){
    fprintf(stderr, "Erreur ouverture du fichier\n");
    exit(1);
  }
  read(fd, &old_h, sizeof(int));
  write(tmp, &new_w, sizeof(int));
  write(tmp, &old_h, sizeof(int));
  read(fd, &nb_items, sizeof(int));
  write(tmp, &nb_items, sizeof(int));
  if(new_h > old_h){
    for(int y = 0; y < new_h; ++y){
      for(int x = 0; x < old_w; ++x){
        if(y < old_h){
	  read(fd, &buf_int, sizeof(int));
	  write(tmp, &buf_int, sizeof(int));
	}
	else{
	  write(tmp, &map_none, sizeof(int));
	}
      }
    }
    char buf[4096];
    int r;
    while((r = read(fd, buf, 4096)) > 0){
      write(tmp, buf, r);
    }
  }
  replace_content(fd, tmp);
  close(fd);
  close(tmp);
  remove("tmp.map");
}

int main(int argc, char *argv[]){
  if(argc < 3){
    fprintf(stderr, "maputil <file> --parameter (...)");
    exit(1);
  }
  char param[15];
  strcpy(param, argv[2]);
  if(!strcmp(param, "--getwidth"))
    get_width(argv[1]);
  else if(!strcmp(param, "--getheight"))
    get_height(argv[1]);
  else if(!strcmp(param, "--getobjects"))
    get_objects(argv[1]);
  else if(!strcmp(param, "--getinfo"))
    get_info(argv[1]);
  else if(!strcmp(param, "--setwidth")){
    if(argc < 4){
      fprintf(stderr, "maputil <file> --setparam <size>");
      exit(1);
    }
    set_width(argv[1], atoi(argv[3]));
  }
  else if(!strcmp(param, "--setheight")){
    if(argc < 4){
      fprintf(stderr, "maputil <file> --setparam <size>");
      exit(1);
    }
    set_height(argv[1], atoi(argv[3]));
  }
  return 0;
}
