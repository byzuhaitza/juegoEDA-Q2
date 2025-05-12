#include "Player.hh"
#include <queue>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME PedroSanchez

struct PLAYER_NAME : public Player {
  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory() { return new PLAYER_NAME; }
  /**
   * Types and attributes for your player can be defined here.
   */
  const vector<Dir> direcciones = {BOTTOM, RIGHT, TOP, LEFT};

  Dir vuelta(Dir d) {
    if (d == BOTTOM) return TOP;
    if (d == TOP) return BOTTOM;
    if (d == LEFT) return RIGHT;
    return LEFT;
  }

  Dir camino_retroceso(const map<Pos,Dir>& path, Pos ini, Pos act) {
    Dir d = path.at(act);
    if (act == ini + d) return d;
    return camino_retroceso(path, ini, act + vuelta(d));
  }

  pair<Dir,int> bfs_generico(Pos p, function<bool(const Cell&)> cond) {
    queue<Pos> bfs;
    map<Pos,Dir> camino;
    camino[p] = NONE;
    bfs.push(p);
    int distancia = 0;

    while (!bfs.empty()) {
      int nivel_size = bfs.size();
      for (int i = 0; i < nivel_size; ++i) {
        Pos ahora = bfs.front();
        bfs.pop();
        for (Dir d : direcciones) {
          Pos nueva = ahora + d;
          if (pos_ok(nueva) and camino.count(nueva) == 0 and cell(nueva).type != WALL) {
            camino[nueva] = d;
            if (cond(cell(nueva))) {
              return make_pair(camino_retroceso(camino, p, nueva), distancia + 1);
            }
            bfs.push(nueva);
          }
        }
      }
      distancia++;
    }

    return make_pair(NONE, -1);
  }

  pair<Dir,int> ciudad_cercana(Pos p) {
    //devuelve true si la celda contiene una ciudad que no es mia
    return bfs_generico(p,
      function<bool(const Cell&)>(
        [&](const Cell& c) {
          return c.type == CITY and city_owner(c.city_id) != me();
        }
      )
    );
  }

  pair<Dir,int> camino_cercano(Pos p) {
    //devuelve true si la celda contiene una camino que no es mio
    return bfs_generico(p,
      function<bool(const Cell&)>(
        [&](const Cell& c) {
          return c.type == PATH and path_owner(c.path_id) != me();
        }
      )
    );
  }

  pair<Dir,int> enemigo_muy_cerca(Pos p) {
    //devuelve true si la celda contiene una enemigo
    return bfs_generico(p,
      function<bool(const Cell&)>(
        [&](const Cell& c) {
          return c.unit_id != -1 and unit(c.unit_id).player != me();
        }
      )
    );
  }

  pair<Dir,int> enemigo_muy_debil(Pos p, Unit u) {
    //devuelve true si la celda contiene una enemigo debil
    return bfs_generico(p,
      function<bool(const Cell&)>(
        [&](const Cell& c) {
          if (c.unit_id != -1 and unit(c.unit_id).player != me()) {
            Unit e = unit(c.unit_id);
            return e.health <= 40 and u.health > e.health + 40;
          }
          return false;
        }
      )
    );
  }

  pair<Dir,int> mascarilla_muy_cerca(Pos p) {
    //devuelve true si la celda contiene una mascarilla
    return bfs_generico(p,
      function<bool(const Cell&)>(
        [&](const Cell& c) {
          return c.mask;
        }
      )
    );
  }

  struct NodoAccion {
    int id;
    Dir accion;
    int prioridad;
    bool operator<(const NodoAccion& a) const {
      return prioridad < a.prioridad;
    }
  };

  virtual void play() {
    priority_queue<NodoAccion> acciones;
    vector<int> unidades = my_units(me());
    for (int i = 0; i < (int)unidades.size(); ++i) {
      int id = unidades[i];
      Unit u = unit(id);
      Pos p = u.pos;
      pair<Dir,int> enemigo_ady = enemigo_muy_cerca(p);
      pair<Dir,int> enemigo_debil = enemigo_muy_debil(p, u);
      pair<Dir,int> ciudad = ciudad_cercana(p);
      pair<Dir,int> camino = camino_cercano(p);
      pair<Dir,int> mascarilla = mascarilla_muy_cerca(p);

      Dir acc = NONE;
      int prio = 0;
      if (enemigo_ady.second == 1) {
        acc = enemigo_ady.first;
        prio = 100;
      }
      else if (not u.immune and mascarilla.second == 1) {
        acc = mascarilla.first;
        prio = 90;
      }
      else if (enemigo_debil.second > 0 and enemigo_debil.second <= 3) {
        acc = enemigo_debil.first;
        prio = 80;
      }
      else if (ciudad.second > 0 and (camino.second == -1 or ciudad.second <= camino.second)) {
        acc = ciudad.first;
        prio = 60;
      }
      else if (camino.second > 0) {
        acc = camino.first;
        prio = 50;
      }

      NodoAccion na;
      na.id = id;
      na.accion = acc;
      na.prioridad = prio;
      acciones.push(na);
    }

    set<Pos> ocupadas;
    vector<int> mias = my_units(me());
    for (int i = 0; i < (int)mias.size(); ++i) {
      ocupadas.insert(unit(mias[i]).pos);
    }

    while (!acciones.empty()) {
      NodoAccion na = acciones.top();
      acciones.pop();
      Pos actual = unit(na.id).pos;
      Pos destino = actual + na.accion;
      bool moved = false;
      if (na.accion != NONE and pos_ok(destino) and ocupadas.count(destino) == 0) {
        move(na.id, na.accion);
        ocupadas.insert(destino);
        moved = true;
      }
      if (not moved) {
        for (int j = 0; j < 4; ++j) {
          Dir d = direcciones[j];
          Pos np = actual + d;
          if (pos_ok(np) and cell(np).type != WALL and cell(np).unit_id == -1 and ocupadas.count(np) == 0) {
            move(na.id, d);
            ocupadas.insert(np);
            moved = true;
          }
        }
      }
      if (status(me()) >= 0.90) {
        return;
      }
    }
  }
};

RegisterPlayer(PLAYER_NAME);
