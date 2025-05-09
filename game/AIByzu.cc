#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Byzuhaitza


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

  Dir vuelta(Dir d) { //si tengo up me devuelve down
    if (d == BOTTOM) return TOP;
    if (d == TOP) return BOTTOM;
    if (d == LEFT) return RIGHT;
    else return LEFT;
  }

  Dir camino_retroceso(map<Pos,Dir> path, Pos ini, Pos act) { //calcula el camino de vuelta
    Dir d = path[act]; 
    //cerr << "---------------------------------- dir " << d << endl;
    if (act == ini+d) return d;
    else return camino_retroceso(path, ini, act + vuelta(d));
  }

  pair<Dir, int> ciudad_cercana(int id, Pos p) {
    queue<Pos> bfs;
    map<Pos, Dir> camino;
    camino[p] = NONE;
    bfs.push(p);

    Pos ciudad;
    //bool hay_ciudad = false;

    int distancia = 0;  // Nivel (distancia en pasos)

    while (not bfs.empty()) {
        int nivel_size = bfs.size();  // Número de nodos en este nivel

        for (int i = 0; i < nivel_size; ++i) {
            Pos ahora = bfs.front();
            bfs.pop();

            for (int j = 0; j < 4; ++j) {
                Pos nueva = ahora + direcciones[j];
                Cell celda = cell(nueva);
                if (pos_ok(nueva) and celda.type != WALL and camino.find(nueva) == camino.end()) {
                    if (celda.type == CITY and city_owner(celda.city_id) != me()) {
                        //hay_ciudad = true;
                        ciudad = nueva;
                        // Al encontrar, devolvemos inmediatamente: dirección inicial y distancia +1 (porque estamos a punto de entrar en esa casilla)
                        return {camino_retroceso(camino, p, ciudad), distancia + 1};
                    }
                    bfs.push(nueva);
                    camino[nueva] = direcciones[j];
                }
            }
        }

        distancia++;  // Avanzamos al siguiente nivel
    }

    return {NONE, -1};  // Si no se encontró ciudad, devolvemos -1 o lo que prefieras como valor inválido
 }

 pair<Dir, int> camino_cercano(int id, Pos p) {
    queue<Pos> bfs;
    map<Pos, Dir> camino;
    camino[p] = NONE;
    bfs.push(p);

    Pos path;
    //bool hay_ciudad = false;

    int distancia = 0;  // Nivel (distancia en pasos)

    while (not bfs.empty()) {
        int nivel_size = bfs.size();  // Número de nodos en este nivel

        for (int i = 0; i < nivel_size; ++i) {
            Pos ahora = bfs.front();
            bfs.pop();

            for (int j = 0; j < 4; ++j) {
                Pos nueva = ahora + direcciones[j];
                Cell celda = cell(nueva);
                if (pos_ok(nueva) and celda.type != WALL and camino.find(nueva) == camino.end()) {
                    if (celda.type == PATH and path_owner(celda.path_id) != me()) {
                        //hay_ciudad = true;
                        path = nueva;
                        // Al encontrar, devolvemos inmediatamente: dirección inicial y distancia +1 (porque estamos a punto de entrar en esa casilla)
                        return {camino_retroceso(camino, p, path), distancia + 1};
                    }
                    bfs.push(nueva);
                    camino[nueva] = direcciones[j];
                }
            }
        }

        distancia++;  // Avanzamos al siguiente nivel
    }

    return {NONE, -1};
 }

 pair<Dir, int> enemigo_muy_debil(int id, Pos p, Unit u) {
    queue<Pos> bfs;
    map<Pos, Dir> camino;
    camino[p] = NONE;
    bfs.push(p);

    Pos enemigo;
    //bool hay_ciudad = false;

    int distancia = 0;  // Nivel (distancia en pasos)

    while (not bfs.empty()) {
        int nivel_size = bfs.size();  // Número de nodos en este nivel

        for (int i = 0; i < nivel_size; ++i) {
            Pos ahora = bfs.front();
            bfs.pop();

            for (int j = 0; j < 4; ++j) {
                Pos nueva = ahora + direcciones[j];
                Cell celda = cell(nueva);
                if (pos_ok(nueva) and celda.type != WALL and camino.find(nueva) == camino.end()) {
                    if (celda.unit_id != -1 and celda.unit_id != me()) {
                        //hay_ciudad = true;
                        Unit e = unit(celda.unit_id);
                        if(e.health <= 40 and u.health > e.health+40) {
                            enemigo = nueva;
                            // Al encontrar, devolvemos inmediatamente: dirección inicial y distancia +1 (porque estamos a punto de entrar en esa casilla)
                            return {camino_retroceso(camino, p, enemigo), distancia + 1};
                        }
                    }
                    bfs.push(nueva);
                    camino[nueva] = direcciones[j];
                }
            }
        }

        distancia++;  // Avanzamos al siguiente nivel
    }

    return {NONE, -1};
 }

 pair<Dir, int> enemigo_muy_cerca(int id, Pos p) {
    queue<Pos> bfs;
    map<Pos, Dir> camino;
    camino[p] = NONE;
    bfs.push(p);

    Pos enemigo;
    //bool hay_ciudad = false;

    int distancia = 0;  // Nivel (distancia en pasos)

    while (not bfs.empty()) {
        int nivel_size = bfs.size();  // Número de nodos en este nivel

        for (int i = 0; i < nivel_size; ++i) {
            Pos ahora = bfs.front();
            bfs.pop();

            for (int j = 0; j < 4; ++j) {
                Pos nueva = ahora + direcciones[j];
                Cell celda = cell(nueva);
                if (pos_ok(nueva) and celda.type != WALL and camino.find(nueva) == camino.end()) {
                    if (celda.unit_id != -1 and celda.unit_id != me()) {
                        //hay_ciudad = true;
                        enemigo = nueva;
                         // Al encontrar, devolvemos inmediatamente: dirección inicial y distancia +1 (porque estamos a punto de entrar en esa casilla)
                        return {camino_retroceso(camino, p, enemigo), distancia + 1};
                    }
                    bfs.push(nueva);
                    camino[nueva] = direcciones[j];
                }
            }
        }

        distancia++;  // Avanzamos al siguiente nivel
    }

    return {NONE, -1};
 }

 pair<Dir, int> mascarilla_muy_cerca(int id, Pos p) {
    queue<Pos> bfs;
    map<Pos, Dir> camino;
    camino[p] = NONE;
    bfs.push(p);

    Pos mask;

    int distancia = 0;  // Nivel (distancia en pasos)

    while (not bfs.empty()) {
        int nivel_size = bfs.size();  // Número de nodos en este nivel

        for (int i = 0; i < nivel_size; ++i) {
            Pos ahora = bfs.front();
            bfs.pop();

            for (int j = 0; j < 4; ++j) {
                Pos nueva = ahora + direcciones[j];
                Cell celda = cell(nueva);
                if (pos_ok(nueva) and celda.type != WALL and camino.find(nueva) == camino.end()) {
                    if (celda.mask) {
                        //hay_ciudad = true;
                        mask = nueva;
                        // Al encontrar, devolvemos inmediatamente: dirección inicial y distancia +1 (porque estamos a punto de entrar en esa casilla)
                        return {camino_retroceso(camino, p, mask), distancia + 1};
                    }
                    bfs.push(nueva);
                    camino[nueva] = direcciones[j];
                }
            }
        }

        distancia++;  // Avanzamos al siguiente nivel
    }

    return {NONE, -1};
 }


  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    double st = status(me());
    if (st >= 0.9) return;

    vector<int> ids = my_units(me());

    for (int id : ids){

        Unit u = unit(id);
        Pos p = u.pos;
        cerr << "+++++++++++++++++++++++++++++++++++++++++++ enemigo antes " << endl;
        pair<Dir, int> enemigo_adyacente = enemigo_muy_cerca(id, p); //compararlo con 1, que es adyacente
        cerr << "+++++++++++++++++++++++++++++++++++++++++++ enemigo despues" << endl;
        
        if (enemigo_adyacente.second == 1) move(id, enemigo_adyacente.first);
        else {
            pair<Dir, int> ciudad_mas_cercana = ciudad_cercana(id, p);
            pair<Dir, int> camino_mas_cercano = camino_cercano(id, p);
            pair<Dir, int> enemigo_debil = enemigo_muy_debil(id, p, u);

            Dir d1 = ciudad_mas_cercana.first;
            Dir d2 = camino_mas_cercano.first;
            Dir d4 = enemigo_debil.first;

            int dis1 = ciudad_mas_cercana.second;
            int dis2 = camino_mas_cercano.second;
            int dis4 = enemigo_debil.second; 

            if (not u.immune) {
                pair<Dir, int> mascarilla_adyacente = mascarilla_muy_cerca(id, p);  //compararlo con 1, que es adyacente
                if (mascarilla_adyacente.second == 1) move(id, mascarilla_adyacente.first);
            }
            else {
                if (dis4 <= 3) move(id, d4);
                else {
                    if (dis1 <= dis2) move(id, d1);
                    else move(id, d2);
                } 
            }


        }
        cerr << "--------------------------------- unidad terminada"<< endl;
    }

  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);