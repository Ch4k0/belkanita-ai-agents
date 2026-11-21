#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoTecnico::think(Sensores sensores) {
  Action accion = IDLE;


  // Decisión del agente según el nivel
  switch (sensores.nivel) {
    case 0: accion = ComportamientoTecnicoNivel_0(sensores); break;
    case 1: accion = ComportamientoTecnicoNivel_1(sensores); break;
    case 2: accion = ComportamientoTecnicoNivel_2(sensores); break;
    case 3: accion = ComportamientoTecnicoNivel_3(sensores); break;
    case 4: accion = ComportamientoTecnicoNivel_4(sensores); break;
    case 5: accion = ComportamientoTecnicoNivel_5(sensores); break;
    case 6: accion = ComportamientoTecnicoNivel_6(sensores); break;
  }

  return accion;
}


// =========================================================================
// (NIVEL 0 y 1)
// =========================================================================

void ComportamientoTecnico::InicializarMapaCalor() {
    if (mapaCalor.empty() || mapaCalor.size() < 100) {
        mapaCalor.assign(100, std::vector<int>(100, 0));
    }
}

int ComportamientoTecnico::ObtenerCalor(int f, int c) {
    if (f >= 0 && f < 100 && c >= 0 && c < 100) return mapaCalor[f][c];
    return 999999; // Valor alto para casillas fuera de rango
}

void ComportamientoTecnico::ObtenerCoordenadasSensor(int rumbo, int fActual, int cActual, int indiceSensor, int &fRes, int &cRes) {
    int r = rumbo;
    if (indiceSensor == 1) r = (rumbo + 7) % 8; // 45 grados izq
    if (indiceSensor == 3) r = (rumbo + 1) % 8; // 45 graods der

    int df = 0, dc = 0;
    switch(r) {
        case 0: df=-1; dc=0; break;  case 1: df=-1; dc=1; break;
        case 2: df=0;  dc=1; break;  case 3: df=1;  dc=1; break;
        case 4: df=1;  dc=0; break;  case 5: df=1;  dc=-1; break;
        case 6: df=0;  dc=-1; break; case 7: df=-1; dc=-1; break;
    }
    fRes = fActual + df;
    cRes = cActual + dc;
}

bool ComportamientoTecnico::EsMovimientoValidoNivel0(int indiceSensor, const Sensores& sensores) {
    char sup = sensores.superficie[indiceSensor];
    char agente = sensores.agentes[indiceSensor];
    int cota = sensores.cota[indiceSensor];

    if (!es_camino(sup)) return false; // Solo 'C', 'D', 'U' son transitables en Nivel 0 
    
    if (agente != '_') return false; // Evitamos chocar con el ingeniero

    int desnivel = abs(cota - sensores.cota[0]);
    if (desnivel > 1) return false; // El Técnico no puede superar desniveles mayores a 1

    return true;
}

bool ComportamientoTecnico::EsMovimientoValidoNivel1(int indiceSensor, const Sensores& sensores) {
    char sup = sensores.superficie[indiceSensor];
    char agente = sensores.agentes[indiceSensor];
    int cota = sensores.cota[indiceSensor];

    bool terrenoPermitido = (sup == 'C' || sup == 'S' || sup == 'D' || sup == 'U'); // El Técnico puede pisar Camino, Sendero, Zapatillas y Meta
    
    if (tiene_zapatillas && sup == 'B') terrenoPermitido = true; // El Técnico con zapatillas puede pisar bosque

    if (!terrenoPermitido) return false;
    if (agente != '_') return false; // Evitamos chocar con el ingeniero

    int desnivel = abs(cota - sensores.cota[0]);
    if (desnivel > 1) return false; // El Técnico no puede superar desniveles mayores a 1

    return true;
}

Action ComportamientoTecnico::DecidirMejorMovimientoNivel0(const Sensores& sensores) {
    bool v1 = EsMovimientoValidoNivel0(1, sensores);
    bool v2 = EsMovimientoValidoNivel0(2, sensores);
    bool v3 = EsMovimientoValidoNivel0(3, sensores);

    if (sensores.superficie[2] == 'U' && v2) return WALK;
    if (sensores.superficie[1] == 'U' && v1) return TURN_SL;
    if (sensores.superficie[3] == 'U' && v3) return TURN_SR;

    if (sensores.choque) return TURN_SR; // El Técnico gira a la derecha para intentar desatascarse

    int f1, c1, f2, c2, f3, c3;
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 1, f1, c1);
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 2, f2, c2);
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 3, f3, c3);

    int minCalor = 999999;
    Action mejorAccion = TURN_SR; // Acción por defecto del Técnico

    if (v2) {
        int calor = ObtenerCalor(f2, c2);
        if (calor < minCalor) { minCalor = calor; mejorAccion = WALK; }
    }
    if (v3) {
        int calor = ObtenerCalor(f3, c3);
        if (calor < minCalor) { minCalor = calor; mejorAccion = TURN_SR; }
    }
    if (v1) {
        int calor = ObtenerCalor(f1, c1);
        if (calor < minCalor) { minCalor = calor; mejorAccion = TURN_SL; }
    }

    return mejorAccion;
}

Action ComportamientoTecnico::DecidirMejorMovimientoNivel1(const Sensores& sensores) {
    bool v1 = EsMovimientoValidoNivel1(1, sensores);
    bool v2 = EsMovimientoValidoNivel1(2, sensores);
    bool v3 = EsMovimientoValidoNivel1(3, sensores);

    if (!tiene_zapatillas) { // El Técnico sin zapatillas prioriza las zapatillas
        if (sensores.superficie[2] == 'D' && v2) return WALK;
        if (sensores.superficie[1] == 'D' && v1) return TURN_SL;
        if (sensores.superficie[3] == 'D' && v3) return TURN_SR;
    }

    if (sensores.choque) return TURN_SL; // El Técnico gira a la izquierda para intentar desatascarse

    int f1, c1, f2, c2, f3, c3;
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 1, f1, c1);
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 2, f2, c2);
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 3, f3, c3);

    int minCalor = 999999;
    Action mejorAccion = TURN_SR;

    if (v2) {
        int calor = ObtenerCalor(f2, c2);
        if (calor < minCalor) { minCalor = calor; mejorAccion = WALK; }
    }
    if (v1) {
        int calor = ObtenerCalor(f1, c1);
        if (calor < minCalor) { minCalor = calor; mejorAccion = TURN_SL; }
    }
    if (v3) {
        int calor = ObtenerCalor(f3, c3);
        if (calor < minCalor) { minCalor = calor; mejorAccion = TURN_SR; }
    }

    return mejorAccion;
}


Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores) {
    ActualizarMapa(sensores);
    InicializarMapaCalor();

    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    if (sensores.posF >= 0 && sensores.posF < 100 && sensores.posC >= 0 && sensores.posC < 100) { 
        mapaCalor[sensores.posF][sensores.posC]++; 
    }
    if (sensores.superficie[0] == 'U') return IDLE; 

    Action accion = DecidirMejorMovimientoNivel0(sensores);
    
    last_action = accion;
    return accion;
}


bool ComportamientoTecnico::es_camino(unsigned char c) const {
  return (c == 'C' || c == 'D' || c == 'U');
}


/**
 * @brief Comportamiento reactivo del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores) {
    ActualizarMapa(sensores);
    InicializarMapaCalor();
 
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    if (sensores.posF >= 0 && sensores.posF < 100 && sensores.posC >= 0 && sensores.posC < 100) {
        mapaCalor[sensores.posF][sensores.posC]++;
    }

    Action accion = DecidirMejorMovimientoNivel1(sensores);
    
    last_action = accion;
    return accion;
}


// =========================================================================
// (NIVEL 2)
// =========================================================================

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores) {
  return IDLE;
}


// =========================================================================
// (NIVEL 3)
// =========================================================================


int ComportamientoTecnico::DistanciaChebyshev(int f1, int c1, int f2, int c2) {
    return max(abs(f1 - f2), abs(c1 - c2));
}

int ComportamientoTecnico::CostoBateria(Action accion, const EstadoT &actual, const EstadoT &siguiente, 
                                        const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
    char tipo = terreno[actual.site.f][actual.site.c];
    
    if (accion == TURN_SR || accion == TURN_SL) {
        switch(tipo) {
            case 'A': return 5;
            case 'H': return 2;
            case 'S': return 1;
            default:  return 1;
        }
    }
    
    if (accion == WALK) {
        int base = 1, inc = 0;
        switch(tipo) {
            case 'A': base = 60; break;
            case 'H': base = 6;  break;
            case 'S': base = 3;  break;
            default:  base = 1;  break;
        }
        
        if (tipo == 'A' || tipo == 'H' || tipo == 'S') {
            int diff = (int)altura[siguiente.site.f][siguiente.site.c] - (int)altura[actual.site.f][actual.site.c];
            if (diff > 0) inc = 5;
            else if (diff < 0) inc = -2;
        }
        return base + inc;
    }
    return 1;
}

bool ComportamientoTecnico::CasillaAccesibleTecnico(const EstadoT &st, const vector<vector<unsigned char>> &terreno, 
                                                   const vector<vector<unsigned char>> &altura) {
    EstadoT next = NextCasillaTecnico(st);
    if (next.site.f < 0 || next.site.f >= terreno.size() || next.site.c < 0 || next.site.c >= terreno[0].size()) return false;

    char tipo = terreno[next.site.f][next.site.c];
    if (tipo == 'M' || tipo == 'P') return false;
    if (tipo == 'B' && !st.zapatillas) return false;

    if (abs((int)altura[next.site.f][next.site.c] - (int)altura[st.site.f][st.site.c]) > 1) return false;
    return true;
}

EstadoT ComportamientoTecnico::NextCasillaTecnico(const EstadoT &st) {
    EstadoT sig = st;
    switch (st.site.brujula) {
        case norte: sig.site.f--; break; case noreste: sig.site.f--; sig.site.c++; break;
        case este: sig.site.c++; break;  case sureste: sig.site.f++; sig.site.c++; break;
        case sur: sig.site.f++; break;   case suroeste: sig.site.f++; sig.site.c--; break;
        case oeste: sig.site.c--; break; case noroeste: sig.site.f--; sig.site.c--; break;
    }
    return sig;
}

EstadoT ComportamientoTecnico::applyT(Action accion, const EstadoT &st, const vector<vector<unsigned char>> &terreno, 
                                     const vector<vector<unsigned char>> &altura) {
    EstadoT next = st;
    if (accion == WALK && CasillaAccesibleTecnico(st, terreno, altura)) {
        next = NextCasillaTecnico(st);
        if (terreno[next.site.f][next.site.c] == 'D') next.zapatillas = true;
    } else if (accion == TURN_SR) next.site.brujula = (Orientacion)((next.site.brujula + 1) % 8);
    else if (accion == TURN_SL) next.site.brujula = (Orientacion)((next.site.brujula + 7) % 8);
    return next;
}

list<Action> ComportamientoTecnico::A_Estrella(const EstadoT &inicio, const EstadoT &final, 
                                               const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
    priority_queue<NodoT> frontier;
    set<EstadoT> explored;
    
    NodoT current;
    current.estado = inicio; current.g = 0;
    current.h = DistanciaChebyshev(inicio.site.f, inicio.site.c, final.site.f, final.site.c);
    current.f = current.g + current.h;
    frontier.push(current);

    while (!frontier.empty()) {
        current = frontier.top(); frontier.pop();
        
        // Si llegamos a la meta, convertimos el vector a lista para que sea compatible
        if (current.estado.site.f == final.site.f && current.estado.site.c == final.site.c) {
            return list<Action>(current.secuencia.begin(), current.secuencia.end());
        }
        
        if (explored.count(current.estado)) continue;
        explored.insert(current.estado);

        Action acciones[] = {WALK, TURN_SR, TURN_SL};
        for (Action a : acciones) {
            NodoT hijo = current;
            hijo.estado = applyT(a, current.estado, terreno, altura);
            
            if (!(hijo.estado == current.estado)) {
                
                // Si este estado ya está cerrado/explorado, se ignora al hijo para no reventar la memoria
                if (explored.count(hijo.estado)) {
                    continue; 
                }
                
                hijo.g += CostoBateria(a, current.estado, hijo.estado, terreno, altura);
                hijo.h = DistanciaChebyshev(hijo.estado.site.f, hijo.estado.site.c, final.site.f, final.site.c);
                hijo.f = hijo.g + hijo.h;
                hijo.secuencia.push_back(a);
                frontier.push(hijo);
            }
        }
    }
    
    //si la Belkanita es inalcanzable, devuelve una lista vacía
    return {}; 
}


/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores) {
  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

    // Si se choca, borra el plan
    if (sensores.choque) {
        hayPlan = false;
        plan.clear();
    }

    // Calcular el plan si no hay
    if (!hayPlan) {
        EstadoT inicio, fin;
        inicio.site = {sensores.posF, sensores.posC, sensores.rumbo};
        inicio.zapatillas = tiene_zapatillas;
        fin.site = {sensores.BelPosF, sensores.BelPosC, norte}; 
        
        plan = A_Estrella(inicio, fin, mapaResultado, mapaCotas);
        
        if (!plan.empty()) {
            VisualizaPlan(inicio.site, plan); // visualiza el plan
            hayPlan = true;
        } else {
            return IDLE;
        }
    }

    // ejecuta el plan
    if (hayPlan && !plan.empty()) {
        
        // si esta el ingeniero justo en la casilla enfrente, espera
        if (plan.front() == WALK && sensores.agentes[2] == 'i') {
            return IDLE; // No borramos el plan, solo pausamos.
        }

        Action a = plan.front(); 
        plan.pop_front();
        
        if (plan.empty()) hayPlan = false; 
        return a;
    }
    return IDLE;
}


// =========================================================================
// (NIVEL 4)
// =========================================================================

/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores) {
  return IDLE;
}


// =========================================================================
// (NIVEL 5)
// =========================================================================

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores) {
    ActualizarMapa(sensores); 
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    
    // Si choca, borra la ruta para forzar rehacerla
    if (sensores.choque) plan_viaje_tec.clear();

    if (fase_obra_tec == 0) {
        if (sensores.venpaca) {
            meta_f_tec = sensores.GotoF;
            meta_c_tec = sensores.GotoC;
            fase_obra_tec = 1;
        } else {
            return IDLE;
        }
    }

    if (fase_obra_tec == 1) {
        if (plan_viaje_tec.empty()) {
            if (sensores.posF == meta_f_tec && sensores.posC == meta_c_tec) {
                fase_obra_tec = 2; 
            } else {
                EstadoT inicio = { {sensores.posF, sensores.posC, sensores.rumbo}, tiene_zapatillas };
                EstadoT fin = { {meta_f_tec, meta_c_tec, norte}, false }; 
                
                //busca al ingeniero y pone un MURO donde él
                int f_i = -1, c_i = -1; 
                unsigned char backup_i = 0;
                
                for(int i = 1; i <= 15; i++) {
                    if(sensores.agentes[i] == 'i') {
                        ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, i, f_i, c_i);
                        //si esta pisando la meta no se pone muro
                        if(f_i != fin.site.f || c_i != fin.site.c) {
                            backup_i = mapaResultado[f_i][c_i];
                            mapaResultado[f_i][c_i] = 'M';
                        }
                        break;
                    }
                }
                
                //calcula ruta obligándole a rodear el ingeniero
                plan_viaje_tec = A_Estrella(inicio, fin, mapaResultado, mapaCotas);
                
                // quita el falso muro del ingeniero para no estropear el mapa para el futuro
                if (backup_i != 0) mapaResultado[f_i][c_i] = backup_i; 
                
                if (plan_viaje_tec.empty()) {
                    int dist = abs(sensores.posF - meta_f_tec) + abs(sensores.posC - meta_c_tec);
                    if (dist == 1 && mapaCotas[meta_f_tec][meta_c_tec] <= mapaCotas[sensores.posF][sensores.posC]) {
                        fase_obra_tec = 2;
                        return IDLE;
                    }
                    // si no es callejón se intenta sin muro
                    plan_viaje_tec = A_Estrella(inicio, fin, mapaResultado, mapaCotas);
                }
                VisualizaPlan(inicio.site, plan_viaje_tec);
            }
        }
        
        if (!plan_viaje_tec.empty()) {
            Action a = plan_viaje_tec.front(); 
            plan_viaje_tec.pop_front(); 
            return a;
        }
    }

    if (fase_obra_tec == 2) {
        if (sensores.enfrente) {
            fase_obra_tec = 0; 
            return INSTALL;    
        } else {
            return TURN_SR; 
        }
    }
    return IDLE;
}


/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores) {
  return IDLE;
}


// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoTecnico::ActualizarMapa(Sensores sensores) {
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo) {
    case norte:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
          mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
        }
      break;
    case noreste:
      mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
      mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
      mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
      mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
      mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
      mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
      mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
      mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
      mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
      mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
      mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
      mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
      mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
      mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
      mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
      mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
      mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
      mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
      mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
      mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
      mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
      mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
      mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
      mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
      mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
      mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
      mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
      mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
      mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
      mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
      break;
    case este:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
          mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
        }
      break;
    case sureste:
      mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
      mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
      mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
      mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
      mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
      mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
      mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
      mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
      mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
      mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
      mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
      mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
      mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
      mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
      mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
      mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
      mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
      mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
      mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
      mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
      mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
      mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
      mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
      mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
      mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
      mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
      mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
      mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
      mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
      mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
      break;
    case sur:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
          mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
        }
      break;
    case suroeste:
      mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
      mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
      mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
      mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
      mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
      mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
      mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
      mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
      mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
      mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
      mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
      mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
      mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
      mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
      mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
      mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
      mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
      mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
      mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
      mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
      mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
      mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
      mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
      mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
      mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
      mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
      mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
      mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
      mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
      mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
      break;
    case oeste:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
          mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
        }
      break;
    case noroeste:
      mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
      mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
      mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
      mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
      mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
      mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
      mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
      mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
      mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
      mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
      mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
      mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
      mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
      mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
      mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
      mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
      mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
      mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
      mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
      mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
      mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
      mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
      mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
      mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
      mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
      mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
      mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
      mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
      mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
      mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
      break;
  }
}



/**
 * @brief Determina si una casilla es transitable para el técnico.
 * En esta práctica, si el técnico tiene zapatillas, el bosque ('B') es transitable.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable.
 */
bool ComportamientoTecnico::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas) {
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
  return es_camino(mapaResultado[f][c]);  // Solo 'C', 'S', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el técnico: desnivel máximo siempre 1.
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoTecnico::EsAccesiblePorAltura(const ubicacion &actual) {
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size()) return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (desnivel > 1) return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoTecnico::Delante(const ubicacion &actual) const {
  ubicacion delante = actual;
  switch (actual.brujula) {
    case 0: delante.f--; break;                        // norte
    case 1: delante.f--; delante.c++; break;     // noreste
    case 2: delante.c++; break;                     // este
    case 3: delante.f++; delante.c++; break;     // sureste
    case 4: delante.f++; break;                        // sur
    case 5: delante.f++; delante.c--; break;     // suroeste
    case 6: delante.c--; break;                     // oeste
    case 7: delante.f--; delante.c--; break;     // noroeste
  }
  return delante;
}


/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}



/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::VisualizaPlan(const ubicacion &st,
                                            const list<Action> &plan)
{
   listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}


