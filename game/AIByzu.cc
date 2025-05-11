#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Mandem_girl

struct PLAYER_NAME : public Player {

    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
  static Player* factory () { 
    return new PLAYER_NAME; 
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  const vector<Dir> direcciones = {BOTTOM, RIGHT, TOP, LEFT};

  Dir vuelta(Dir d) {
    if (d == BOTTOM) return TOP;
    if (d == TOP)    return BOTTOM;
    if (d == LEFT)   return RIGHT;
    return LEFT;
  }

  Dir camino_retroceso(const map<Pos,Dir>& path, Pos ini, Pos act) {
    Dir d = path.at(act);
    if (act == ini + d) return d;
    return camino_retroceso(path, ini, act + vuelta(d));
  }

  pair<Dir,int> ciudad_cercana(int id, Pos p) {
    queue<Pos> bfs;
    map<Pos, Dir> camino;
    camino[p] = NONE;
    bfs.push(p);
    int distancia = 0;
    while (!bfs.empty()) {
      int nivel_size = bfs.size();
      for (int i = 0; i < nivel_size; ++i) {
        Pos ahora = bfs.front(); bfs.pop();
        for (int j = 0; j < 4; ++j) {
          Pos nueva = ahora + direcciones[j];
          if (pos_ok(nueva)) {
            Cell celda = cell(nueva);
            if (celda.type != WALL) {
              if (camino.count(nueva) == 0) {
                camino[nueva] = direcciones[j];
                if (celda.type == CITY and city_owner(celda.city_id) != me()) {
                  return { camino_retroceso(camino, p, nueva), distancia + 1 };
                }
                bfs.push(nueva);
              }
            }
          }
        }
      }
      distancia++;
    }
    return { NONE, -1 };
  }

  pair<Dir,int> camino_cercano(int id, Pos p) {
    queue<Pos> bfs;
    map<Pos, Dir> camino;
    camino[p] = NONE;
    bfs.push(p);
    int distancia = 0;
    while (!bfs.empty()) {
      int nivel_size = bfs.size();
      for (int i = 0; i < nivel_size; ++i) {
        Pos ahora = bfs.front(); bfs.pop();
        for (int j = 0; j < 4; ++j) {
          Pos nueva = ahora + direcciones[j];
          if (pos_ok(nueva)) {
            Cell celda = cell(nueva);
            if (celda.type != WALL) {
              if (camino.count(nueva) == 0) {
                camino[nueva] = direcciones[j];
                if (celda.type == PATH and path_owner(celda.path_id) != me()) {
                  return { camino_retroceso(camino, p, nueva), distancia + 1 };
                }
                bfs.push(nueva);
              }
            }
          }
        }
      }
      distancia++;
    }
    return { NONE, -1 };
  }

  pair<Dir,int> enemigo_muy_cerca(int id, Pos p) {
    queue<Pos> bfs;
    map<Pos, Dir> camino;
    camino[p] = NONE;
    bfs.push(p);
    int distancia = 0;
    while (!bfs.empty()) {
      int nivel_size = bfs.size();
      for (int i = 0; i < nivel_size; ++i) {
        Pos ahora = bfs.front(); bfs.pop();
        for (int j = 0; j < 4; ++j) {
          Pos nueva = ahora + direcciones[j];
          if (pos_ok(nueva)) {
            Cell celda = cell(nueva);
            if (celda.type != WALL) {
              if (camino.count(nueva) == 0) {
                camino[nueva] = direcciones[j];
                if (celda.unit_id != -1 and celda.unit_id != me()) {
                  return { camino_retroceso(camino, p, nueva), distancia + 1 };
                }
                bfs.push(nueva);
              }
            }
          }
        }
      }
      distancia++;
    }
    return { NONE, -1 };
  }

  pair<Dir,int> enemigo_muy_debil(int id, Pos p, Unit u) {
    queue<Pos> bfs;
    map<Pos, Dir> camino;
    camino[p] = NONE;
    bfs.push(p);
    int distancia = 0;
    while (!bfs.empty()) {
      int nivel_size = bfs.size();
      for (int i = 0; i < nivel_size; ++i) {
        Pos ahora = bfs.front(); bfs.pop();
        for (int j = 0; j < 4; ++j) {
          Pos nueva = ahora + direcciones[j];
          if (pos_ok(nueva)) {
            Cell celda = cell(nueva);
            if (celda.type != WALL) {
              if (camino.count(nueva) == 0) {
                camino[nueva] = direcciones[j];
                if (celda.unit_id != -1 and celda.unit_id != me()) {
                  Unit e = unit(celda.unit_id);
                  if (e.health <= 40 and u.health > e.health + 40) {
                    return { camino_retroceso(camino, p, nueva), distancia + 1 };
                  }
                }
                bfs.push(nueva);
              }
            }
          }
        }
      }
      distancia++;
    }
    return { NONE, -1 };
  }

  pair<Dir,int> mascarilla_muy_cerca(int id, Pos p) {
    queue<Pos> bfs;
    map<Pos, Dir> camino;
    camino[p] = NONE;
    bfs.push(p);
    int distancia = 0;
    while (!bfs.empty()) {
      int nivel_size = bfs.size();
      for (int i = 0; i < nivel_size; ++i) {
        Pos ahora = bfs.front(); bfs.pop();
        for (int j = 0; j < 4; ++j) {
          Pos nueva = ahora + direcciones[j];
          if (pos_ok(nueva)) {
            Cell celda = cell(nueva);
            if (celda.type != WALL) {
              if (camino.count(nueva) == 0) {
                camino[nueva] = direcciones[j];
                if (celda.mask) {
                  return { camino_retroceso(camino, p, nueva), distancia + 1 };
                }
                bfs.push(nueva);
              }
            }
          }
        }
      }
      distancia++;
    }
    return { NONE, -1 };
  }

  virtual void play () {
    vector<int> ids = my_units(me());
    for (int id : ids) {
      Unit u = unit(id);
      Pos p = u.pos;
      pair<Dir, int>  enemigo_adyacente = enemigo_muy_cerca(id, p);
      pair<Dir, int>  ciudad_adyacente = ciudad_cercana(id, p);
      pair<Dir, int>  camino_mas_proximo = camino_cercano(id, p);
      pair<Dir, int>  enemigo_debil = enemigo_muy_debil(id, p, u);
      pair<Dir, int>  mascarilla_adyacente = mascarilla_muy_cerca(id, p);

      bool moved = false;
      // 1) Atacar enemigo adyacente
      if (enemigo_adyacente.second == 1) {
        move(id, enemigo_adyacente.first);
        moved = true;
      }
      // 2) Recoger mascarilla si no inmune y cerca
      else if (!u.immune and mascarilla_adyacente.second == 1) {
        move(id, mascarilla_adyacente.first);
        moved = true;
      }
      // no hay preferencia
      else {
        if (!moved and enemigo_debil.second != -1 and enemigo_debil.second <= 3) {
          move(id, enemigo_debil.first);
          moved = true;
        }
        else if (!moved and ciudad_adyacente.second != -1 and ciudad_adyacente.second <= 5 and (camino_mas_proximo.second == -1 or ciudad_adyacente.second <= camino_mas_proximo.second)) {
          move(id, ciudad_adyacente.first);
          moved = true;
        }
        else if (!moved and camino_mas_proximo.second != -1) {
          move(id, camino_mas_proximo.first);
          moved = true;
        }
      }
      //
      if (!moved) {
        for (int j = 0; j < 4; ++j) {
          Dir d = direcciones[j];
          Pos s = p + d;
          if (pos_ok(s) and cell(s).type != WALL and !moved) {
            move(id, d);
            moved = true;
          }
        }
      }
      if (status(me()) >= 0.95) return;
    }
  }
};

RegisterPlayer(PLAYER_NAME);