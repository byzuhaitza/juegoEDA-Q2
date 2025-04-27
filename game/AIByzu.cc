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

  Dir mover_jugadores(int id, Pos p, Unit u) {
    queue<Pos> bfs; //camino
    map<Pos, Dir> camino; //posicion en la que estoy y de que direccion vengo
    camino[p] = NONE; //primera posicion, direccion none (limite)
    bfs.push(p); //primera pos
    bool hay_ciudad = false; //esto aqui o dentro del if??????????????????????????
    bool hay_camino = false; 
    Pos ciudad, cam, inm;
    int inmediato = 0;
    bool pref = false;
    while (not bfs.empty()) {
        Pos ahora = bfs.front();
        bfs.pop();

        //cerr << "+++++++++++++++++++++++++++++++++++++++++++ ACTUAL " << ahora << endl;
        for (int i = 0; i < 4; ++i) { //las 4 direcciones posibles
            Pos nueva = ahora + direcciones[i];
            Cell celda = cell(nueva);
            if (pos_ok(nueva) and celda.type != WALL and camino.find(nueva) == camino.end() and !pref) { //no pared
                //cerr << "+++++++++++++++++++++++++++++++++++++++++++ ACTUAL " << nueva << endl;
                
                if(inmediato < 4) {
                    cerr << "+++++++++++++++++++++++++++++++++++++++++++ INMEDIATO " << inmediato << endl;
                    if (celda.unit_id != -1 and celda.unit_id != me()) { //enemigo
                        inm = nueva;
                        pref = true;
                    }
                    else if (celda.mask and !u.immune and !u.mask) {
                        inm = nueva;
                        pref = true;
                    }
                    ++inmediato;
                }

                if (celda.type == CITY and city_owner(celda.city_id) != me()) { //si es una ciudad y esta conquistado por alguien que no soy yo
                    hay_ciudad = true;
                    ciudad = nueva;
                }
                else if (celda.type == PATH) {
                    Path pat = path(celda.path_id);
                    if (path_owner(celda.path_id) != me() or (city_owner(pat.first.first) != me() or city_owner(pat.first.second))) {
                        //el path no es mio o alguna de las ciudades de los extremos del path no es mia
                        hay_camino = true; 
                        cam = nueva;

                    }
                }
                bfs.push(nueva);
                camino[nueva] = direcciones[i];
            }
        }
        if (pref) {
            return camino_retroceso(camino, p, inm);
        }
        else if (hay_ciudad) {
            //cerr << "--------------------------------------------- OKKKK " << nueva << endl;
            return camino_retroceso(camino, p, ciudad);
        }
        else if (hay_camino) {
            return camino_retroceso(camino, p, cam);
        }
    }
    return NONE;
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
        Dir d = mover_jugadores(id, p, u);
        //cerr << "------------------------ dir " << d << "- posicion " << p <<endl;
        move(id, d);
    }

    

  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
