#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoIngeniero::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisión del agente según el nivel
  switch (sensores.nivel)
  {
  case 0: accion = ComportamientoIngenieroNivel_0(sensores); break;
  case 1: accion = ComportamientoIngenieroNivel_1(sensores); break;
  case 2: accion = ComportamientoIngenieroNivel_2(sensores); break;
  case 3: accion = ComportamientoIngenieroNivel_3(sensores); break;
  case 4: accion = ComportamientoIngenieroNivel_4(sensores); break;
  case 5: accion = ComportamientoIngenieroNivel_5(sensores); break;
  case 6: accion = ComportamientoIngenieroNivel_6(sensores); break;
  }

  return accion;
}




// =========================================================================
// (NIVEL 0 y 1)
// =========================================================================

void ComportamientoIngeniero::InicializarMapaCalor() {
    if (mapaCalor.empty() || mapaCalor.size() < 100) {
        mapaCalor.assign(100, std::vector<int>(100, 0));
    }
}

int ComportamientoIngeniero::ObtenerCalor(int f, int c) {
    if (f >= 0 && f < 100 && c >= 0 && c < 100) return mapaCalor[f][c];
    return 999999; // Valor alto para casillas fuera de rango
}

void ComportamientoIngeniero::ObtenerCoordenadasSensor(int rumbo, int fActual, int cActual, int indiceSensor, int &fRes, int &cRes) {
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

bool ComportamientoIngeniero::EsMovimientoValidoNivel0(int indiceSensor, const Sensores& sensores) {
    char sup = sensores.superficie[indiceSensor];
    char agente = sensores.agentes[indiceSensor];
    int cota = sensores.cota[indiceSensor];

    if (!es_camino(sup)) return false; // Nivel 0: Solo Caminos(C), Zapatillas(D) y Plantas(U) son transitables
    
    if (agente != '_') return false; // Evitamos chocar con el técnico

    int desnivel = cota - sensores.cota[0];
    if (tiene_zapatillas && abs(desnivel) > 2) return false;
    if (!tiene_zapatillas && abs(desnivel) > 1) return false;

    return true;
}

bool ComportamientoIngeniero::EsMovimientoValidoNivel1(int indiceSensor, const Sensores& sensores) {
    char sup = sensores.superficie[indiceSensor];
    char agente = sensores.agentes[indiceSensor];
    int cota = sensores.cota[indiceSensor];

    if (sup != 'C' && sup != 'S' && sup != 'D' && sup != 'U') return false;
    
    if (agente != '_') return false; // Evitamos chocar con el técnico

    int desnivel = cota - sensores.cota[0];
    if (tiene_zapatillas && abs(desnivel) > 2) return false;
    if (!tiene_zapatillas && abs(desnivel) > 1) return false;

    return true;
}

Action ComportamientoIngeniero::DecidirMejorMovimientoNivel0(const Sensores& sensores) {
    bool v1 = EsMovimientoValidoNivel0(1, sensores);
    bool v2 = EsMovimientoValidoNivel0(2, sensores);
    bool v3 = EsMovimientoValidoNivel0(3, sensores);

    if (sensores.superficie[2] == 'U' && v2) return WALK;
    if (sensores.superficie[1] == 'U' && v1) return TURN_SL;
    if (sensores.superficie[3] == 'U' && v3) return TURN_SR;

    if (!tiene_zapatillas) {
        if (sensores.superficie[2] == 'D' && v2) return WALK;
        if (sensores.superficie[1] == 'D' && v1) return TURN_SL;
        if (sensores.superficie[3] == 'D' && v3) return TURN_SR;
    }

    if (sensores.choque) return TURN_SL;

    int f1, c1, f2, c2, f3, c3;
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 1, f1, c1);
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 2, f2, c2);
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 3, f3, c3);

    int minCalor = 999999;
    Action mejorAccion = TURN_SL; // Valor por defecto (girar a la izquierda)

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

Action ComportamientoIngeniero::DecidirMejorMovimientoNivel1(const Sensores& sensores) {
    bool v1 = EsMovimientoValidoNivel1(1, sensores);
    bool v2 = EsMovimientoValidoNivel1(2, sensores);
    bool v3 = EsMovimientoValidoNivel1(3, sensores);

    if (!tiene_zapatillas) {
        if (sensores.superficie[2] == 'D' && v2) return WALK;
        if (sensores.superficie[1] == 'D' && v1) return TURN_SL;
        if (sensores.superficie[3] == 'D' && v3) return TURN_SR;
    }

    if (sensores.choque) return TURN_SL; // El ingeniero gira a la izquierda para intentar desatascarse

    int f1, c1, f2, c2, f3, c3;
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 1, f1, c1);
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 2, f2, c2);
    ObtenerCoordenadasSensor(sensores.rumbo, sensores.posF, sensores.posC, 3, f3, c3);

    int minCalor = 999999;
    Action mejorAccion = TURN_SL;

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


Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores) {
    ActualizarMapa(sensores);
    InicializarMapaCalor();

    if (sensores.superficie[0] == 'D') tiene_zapatillas = true; // Actualizamos el estado del inventario   
    if (sensores.posF >= 0 && sensores.posF < 100 && sensores.posC >= 0 && sensores.posC < 100) {
        mapaCalor[sensores.posF][sensores.posC]++; // Registramos el rastro térmico
    }
    if (sensores.superficie[0] == 'U') return IDLE;

    Action accion = DecidirMejorMovimientoNivel0(sensores);
    
    last_action = accion;
    return accion;
}


bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}


/**
 * @brief Comportamiento reactivo del ingeniero para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores) {
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

EstadoI ComportamientoIngeniero::NextCasillaIngeniero(const EstadoI &st) {
    EstadoI siguiente = st;
    switch (st.site.brujula) {
        case norte:    siguiente.site.f = st.site.f - 1; break;
        case noreste:  siguiente.site.f = st.site.f - 1; siguiente.site.c = st.site.c + 1; break;
        case este:     siguiente.site.c = st.site.c + 1; break;
        case sureste:  siguiente.site.f = st.site.f + 1; siguiente.site.c = st.site.c + 1; break;
        case sur:      siguiente.site.f = st.site.f + 1; break;
        case suroeste: siguiente.site.f = st.site.f + 1; siguiente.site.c = st.site.c - 1; break;
        case oeste:    siguiente.site.c = st.site.c - 1; break;
        case noroeste: siguiente.site.f = st.site.f - 1; siguiente.site.c = st.site.c - 1; break;
    }
    return siguiente;
}

bool ComportamientoIngeniero::CasillaAccesibleIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
    EstadoI next = NextCasillaIngeniero(st);
    
    // Evitar salir del mapa
    if (next.site.f < 0 || next.site.f >= terreno.size() || next.site.c < 0 || next.site.c >= terreno[0].size()) return false;

    char tipo = terreno[next.site.f][next.site.c];
    
    // No Muros, Precipicios ni Bosques
    if (tipo == 'M' || tipo == 'P' || tipo == 'B') return false;

    int dif_altura = altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c];
    if (st.zapatillas && abs(dif_altura) <= 2) return true;
    if (!st.zapatillas && abs(dif_altura) <= 1) return true;

    return false;
}


bool ComportamientoIngeniero::CasillaAccesibleIngenieroJump(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
    
    // COMPROBAR CASILLA DE ATERRIZAJE
    EstadoI next = NextCasillaIngenieroJump(st);
    
    if (next.site.f < 0 || next.site.f >= terreno.size() || next.site.c < 0 || next.site.c >= terreno[0].size()) return false;

    char tipo_fin = terreno[next.site.f][next.site.c];
    
    // El aterrizaje tiene las mismas reglas que caminar
    if (tipo_fin == 'M' || tipo_fin == 'P' || tipo_fin == 'B') return false;

    // Altura del salto desde el origen hasta el aterrizaje
    int dif_altura = altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c];
    bool altura_ok = (st.zapatillas && abs(dif_altura) <= 2) || (!st.zapatillas && abs(dif_altura) <= 1);
    if (!altura_ok) return false;


    // Se comprueba casilla del medio del salto
    EstadoI mid = NextCasillaIngeniero(st);
    if (mid.site.f < 0 || mid.site.f >= terreno.size() || mid.site.c < 0 || mid.site.c >= terreno[0].size()) return false;
    char tipo_mid = terreno[mid.site.f][mid.site.c];
    if (tipo_mid == 'M' || tipo_mid == 'P' || tipo_mid == 'B') return false; 

    // Si ha pasado los if, salta
    return true;
}


EstadoI ComportamientoIngeniero::NextCasillaIngenieroJump(const EstadoI &st) {
    EstadoI siguiente = st;
    switch (st.site.brujula) {
        case 0: siguiente.site.f -= 2; break; // N
        case 1: siguiente.site.f -= 2; siguiente.site.c += 2; break; // NE
        case 2: siguiente.site.c += 2; break; // E
        case 3: siguiente.site.f += 2; siguiente.site.c += 2; break; // SE
        case 4: siguiente.site.f += 2; break; // S
        case 5: siguiente.site.f += 2; siguiente.site.c -= 2; break; // SW
        case 6: siguiente.site.c -= 2; break; // O
        case 7: siguiente.site.f -= 2; siguiente.site.c -= 2; break; // NW
    }
    return siguiente;
}


EstadoI ComportamientoIngeniero::applyI(Action accion, const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
    EstadoI next = st;
    switch(accion) {
        case WALK:
            if (CasillaAccesibleIngeniero(st, terreno, altura)) {
                next = NextCasillaIngeniero(st);
                if (terreno[next.site.f][next.site.c] == 'D') next.zapatillas = true;
            }
            break;
        case JUMP:
            if (CasillaAccesibleIngenieroJump(st, terreno, altura)) {
                next = NextCasillaIngenieroJump(st);
                if (terreno[next.site.f][next.site.c] == 'D') next.zapatillas = true;
            }
            break;
        case TURN_SR:
            next.site.brujula = (Orientacion) ((next.site.brujula + 1) % 8);
            break;
        case TURN_SL:
            next.site.brujula = (Orientacion) ((next.site.brujula + 7) % 8);
            break;
    }
    return next;
}


list<Action> ComportamientoIngeniero::B_Anchura(const EstadoI &inicio, const EstadoI &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
    NodoI current_node;
    list<NodoI> frontier;
    set<NodoI> explored;
    list<Action> path;

    current_node.estado = inicio;
    frontier.push_back(current_node);

    bool SolutionFound = (current_node.estado.site.f == final.site.f && current_node.estado.site.c == final.site.c);

    while (!SolutionFound && !frontier.empty()) {
        frontier.pop_front();
        explored.insert(current_node);

        NodoI child_Walk = current_node;
        child_Walk.estado = applyI(WALK, current_node.estado, terreno, altura);
        
        // Si el estado cambió, el mov era valido
        if (!(child_Walk.estado == current_node.estado)) {
            if (child_Walk.estado.site.f == final.site.f && child_Walk.estado.site.c == final.site.c) {
                child_Walk.secuencia.push_back(WALK);
                current_node = child_Walk;
                SolutionFound = true;
            } else if (explored.find(child_Walk) == explored.end()) {
                child_Walk.secuencia.push_back(WALK);
                frontier.push_back(child_Walk);
            }
        }

        if (!SolutionFound) {
            // Se prueba girando a la der
            NodoI child_TurnSR = current_node;
            child_TurnSR.estado = applyI(TURN_SR, current_node.estado, terreno, altura);
            if (explored.find(child_TurnSR) == explored.end()) {
                child_TurnSR.secuencia.push_back(TURN_SR);
                frontier.push_back(child_TurnSR);
            }

            // Se prueba girando a la izq
            NodoI child_TurnSL = current_node;
            child_TurnSL.estado = applyI(TURN_SL, current_node.estado, terreno, altura);
            if (explored.find(child_TurnSL) == explored.end()) {
                child_TurnSL.secuencia.push_back(TURN_SL);
                frontier.push_back(child_TurnSL);
            }

            // Se prueba JUMP
            NodoI child_Jump = current_node;
            child_Jump.estado = applyI(JUMP, current_node.estado, terreno, altura);
            
            // Si el estado cambió, el salto era ok
            if (!(child_Jump.estado == current_node.estado)) {
                if (child_Jump.estado.site.f == final.site.f && child_Jump.estado.site.c == final.site.c) {
                    child_Jump.secuencia.push_back(JUMP);
                    current_node = child_Jump;
                    SolutionFound = true;
                } else if (explored.find(child_Jump) == explored.end()) {
                    child_Jump.secuencia.push_back(JUMP);
                    frontier.push_back(child_Jump);
                }
            }
        }

        // Elegir el siguiente nodo que no haya visitado
        if (!SolutionFound && !frontier.empty()) {
            current_node = frontier.front();
            while (explored.find(current_node) != explored.end() && !frontier.empty()) {
                frontier.pop_front();
                if (!frontier.empty()) current_node = frontier.front();
            }
        }
    }

    if (SolutionFound) {
        path = current_node.secuencia;
    }
    
    return path;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 2 (búsqueda).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores)
{
    Action accion = IDLE;

    //Si encuentra zapatillas, las recoge
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

    //Si no plan, se calcula
    if (!hayPlan) {
        EstadoI inicio;
        inicio.site.f = sensores.posF;
        inicio.site.c = sensores.posC;
        inicio.site.brujula = sensores.rumbo;
        inicio.zapatillas = tiene_zapatillas;

        EstadoI objetivo;
        objetivo.site.f = sensores.BelPosF;
        objetivo.site.c = sensores.BelPosC;

        //Se llama al algoritmo de búsqueda
        plan = B_Anchura(inicio, objetivo, mapaResultado, mapaCotas);
        
        //Visualizamos plan
        VisualizaPlan(inicio.site, plan); 
        hayPlan = plan.size() > 0;
    }

    //Si hay plan, ejecuta paso a paso
    if (hayPlan && plan.size() > 0) {
        accion = plan.front();
        plan.pop_front();
    }

    if (plan.size() == 0) {
        hayPlan = false;
    }

    last_action = accion;
    return accion;
}


// =========================================================================
// (NIVEL 3)
// =========================================================================

bool ComportamientoIngeniero::VeoAlTecnico(const Sensores& sensores) {
    //Se comprueba si 't' aparece en cono de visión cercano
    for (int i = 1; i <= 8; i++) {
        if (sensores.agentes[i] == 't') {
            return true;
        }
    }
    return false;
}


/**
 * @brief Comportamiento del ingeniero para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores)
{
bool veo_tecnico = false;
    
    //se busca al tecnico
    for (int i = 1; i < 16; i++) {
        if (sensores.agentes[i] == 't') {
            veo_tecnico = true;
            break;
        }
    }

    if (veo_tecnico) {
        char sup = sensores.superficie[2];
        
        // Si chocamos, hay un obstáculo, o el Técnico está justo delante taponando, giramos
        if (sensores.choque || sensores.agentes[2] != '_' || 
            sup == 'M' || sup == 'P' || sup == 'A' || sup == 'B') {
            return TURN_SR;
        }
        
        // Si tenemos vía libre, caminamos para despejar el área
        return WALK;
    }

    // Si no vemos al Técnico, nos quedamos quietos para no salirnos del mapa
    return IDLE;
}


// =========================================================================
// NIVEL 4
// =========================================================================

//calcula el impacto segun la tabla
int ComportamientoIngeniero::CalcularImpacto(char terreno, int op) {
    int impacto = 0;
    
    // Coste base de INSTALL por tipo de terreno
    if (terreno == 'A') impacto = 50;
    else if (terreno == 'H') impacto = 45;
    else if (terreno == 'S') impacto = 25;
    else if (terreno == 'C' || terreno == 'U') impacto = 15;
    else impacto = 30; // Bosque, Tierra y Resto de casillas

    // Penalizaciones extra por modificar la altura (RAISE o DIG)
    if (op == 1) { // RAISE (Subir terreno)
        if (terreno == 'H') impacto += 55;
        else if (terreno == 'S') impacto += 30;
        else if (terreno == 'C' || terreno == 'U') impacto += 10;
        else impacto += 40; // Resto de casillas
    } else if (op == -1) { // DIG (Bajar terreno)
        if (terreno == 'H') impacto += 65;
        else if (terreno == 'S') impacto += 40;
        else if (terreno == 'C' || terreno == 'U') impacto += 25;
        else impacto += 50; // Resto de casillas
    }
    
    return impacto;
}


list<Paso> ComportamientoIngeniero::PlanificarRed(int f_ini, int c_ini, const vector<vector<unsigned char>> &mapa, const vector<vector<unsigned char>> &alturas, int max_eco) {
    priority_queue<NodoTuberia> abierta;
    map<EstadoTuberia, int> cerrados;

    for (int op = -1; op <= 1; op++) {

        if (mapa[f_ini][c_ini] == 'M' || mapa[f_ini][c_ini] == 'P' || mapa[f_ini][c_ini] == 'B') continue;
        
        // No se puede hacer RAISE ni DIG en agua
        if (mapa[f_ini][c_ini] == 'A' && op != 0) continue; 
        
        if (op == 1 && alturas[f_ini][c_ini] >= 9) continue; 
        if (op == -1 && alturas[f_ini][c_ini] <= 1) continue; 

        NodoTuberia inicio;
        inicio.estado = {f_ini, c_ini, op};
        inicio.eco = CalcularImpacto(mapa[f_ini][c_ini], op) - CalcularImpacto(mapa[f_ini][c_ini], 0);
        inicio.g = 1;
        inicio.secuencia.push_back({f_ini, c_ini, op});
        
        if (inicio.eco <= max_eco) abierta.push(inicio);
    }

    int df[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    while (!abierta.empty()) {
        NodoTuberia actual = abierta.top();
        abierta.pop();

        if (mapa[actual.estado.f][actual.estado.c] == 'U') {
            return list<Paso>(actual.secuencia.begin(), actual.secuencia.end());
        }

        if (cerrados.count(actual.estado) && cerrados[actual.estado] <= actual.eco) continue;
        cerrados[actual.estado] = actual.eco;

        int h_salida = alturas[actual.estado.f][actual.estado.c] + actual.estado.op;

        for (int i = 0; i < 4; i++) {
            int nf = actual.estado.f + df[i], nc = actual.estado.c + dc[i];

            if (nf >= 0 && nf < (int)mapa.size() && nc >= 0 && nc < (int)mapa[0].size()) {
                
                // Permitimos el agua, bloqueamos el resto
                if (mapa[nf][nc] == 'M' || mapa[nf][nc] == 'P' || mapa[nf][nc] == 'B') continue;

                int h_destino_antes_de_obra = alturas[nf][nc];
                int desnivel_viaje = abs(h_destino_antes_de_obra - h_salida);
                if (desnivel_viaje > 2) continue; 

                for (int n_op = -1; n_op <= 1; n_op++) {
                    
                    // ni RAISE ni DIG en agua
                    if (mapa[nf][nc] == 'A' && n_op != 0) continue;
                    
                    if (n_op == 1 && alturas[nf][nc] >= 9) continue;
                    if (n_op == -1 && alturas[nf][nc] <= 1) continue;
                    
                    int h_llegada = alturas[nf][nc] + n_op;

                    if (h_salida == h_llegada || h_salida == h_llegada + 1) {
                        int install_origen = CalcularImpacto(mapa[actual.estado.f][actual.estado.c], 0);
                        int costo_destino = CalcularImpacto(mapa[nf][nc], n_op);
                        int nuevo_eco = actual.eco + install_origen + costo_destino;
                        
                        if (nuevo_eco <= max_eco) {
                            EstadoTuberia est_hijo = {nf, nc, n_op};
                            if (!cerrados.count(est_hijo) || cerrados[est_hijo] > nuevo_eco) {
                                NodoTuberia hijo = actual;
                                hijo.estado = est_hijo;
                                hijo.eco = nuevo_eco;
                                hijo.g++;
                                hijo.secuencia.push_back({nf, nc, n_op});
                                abierta.push(hijo);
                            }
                        }
                    }
                }
            }
        }
    }
    return {};
}


/**
 * @brief Comportamiento del ingeniero para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores)
{
if (!hayPlan) {
        //lee el límite dinámico de cada prueba
        int umbral = sensores.max_ecologico;
        
        list<Paso> resultado = PlanificarRed(sensores.BelPosF, sensores.BelPosC, mapaResultado, mapaCotas, umbral);
        
        if (!resultado.empty()) {
            VisualizaRedTuberias(resultado); 
        } else {
            cout << "No hay plan válido por debajo del umbral: " << umbral << endl;
        }
        
        hayPlan = true;
    }
    return IDLE;
}


/**
 * @brief Comportamiento del ingeniero para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores) {
    ActualizarMapa(sensores);
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    if (sensores.choque) plan_viaje_ing.clear();

    if (fase_obra_ing == 0) {
        plan_tuberia = PlanificarRed(sensores.BelPosF, sensores.BelPosC, mapaResultado, mapaCotas, sensores.max_ecologico);
        if (!plan_tuberia.empty()) {
            VisualizaRedTuberias(plan_tuberia);
            tramo_origen_ing = plan_tuberia.front(); plan_tuberia.pop_front();
            tramo_destino_ing = plan_tuberia.front(); plan_tuberia.pop_front();
            fase_obra_ing = 1;
        } else {
            return IDLE;
        }
    }

    if (fase_obra_ing == 1) {
        if (plan_viaje_ing.empty()) {
            if (sensores.posF == tramo_origen_ing.fil && sensores.posC == tramo_origen_ing.col) {
                fase_obra_ing = 2; 
            } else {
                EstadoI inicio = { {sensores.posF, sensores.posC, sensores.rumbo}, tiene_zapatillas };
                EstadoI fin = { {tramo_origen_ing.fil, tramo_origen_ing.col, norte}, false };
                plan_viaje_ing = B_Anchura(inicio, fin, mapaResultado, mapaCotas);
                VisualizaPlan(inicio.site, plan_viaje_ing);
            }
        }
        if (!plan_viaje_ing.empty()) {
            Action a = plan_viaje_ing.front(); 
            //ingeniero quieto si ve al tecnico justo delante para no taponar
            if ((a == WALK || a == JUMP) && sensores.agentes[2] == 't') return IDLE;
            plan_viaje_ing.pop_front(); 
            return a;
        }
    }

    if (fase_obra_ing == 2) {
        if (tramo_origen_ing.op == 1) { tramo_origen_ing.op = 0; return RAISE; }
        if (tramo_origen_ing.op == -1) { tramo_origen_ing.op = 0; return DIG; }
        fase_obra_ing = 3; 
    }

    if (fase_obra_ing == 3) {
        fase_obra_ing = 4;
        return COME; 
    }

    if (fase_obra_ing == 4) {
        if (plan_viaje_ing.empty() || sensores.choque) {
            if (sensores.posF == tramo_destino_ing.fil && sensores.posC == tramo_destino_ing.col) {
                fase_obra_ing = 5; 
            } else {
                EstadoI inicio = { {sensores.posF, sensores.posC, sensores.rumbo}, tiene_zapatillas };
                EstadoI fin = { {tramo_destino_ing.fil, tramo_destino_ing.col, norte}, false };
                plan_viaje_ing = B_Anchura(inicio, fin, mapaResultado, mapaCotas);
                VisualizaPlan(inicio.site, plan_viaje_ing);
            }
        }
        if (!plan_viaje_ing.empty()) {
            Action a = plan_viaje_ing.front(); 
            //ingeniero quieto
            if ((a == WALK || a == JUMP) && sensores.agentes[2] == 't') return IDLE;
            plan_viaje_ing.pop_front(); 
            return a;
        }
    }

    if (fase_obra_ing == 5) {
        if (tramo_destino_ing.op == 1) { tramo_destino_ing.op = 0; return RAISE; }
        if (tramo_destino_ing.op == -1) { tramo_destino_ing.op = 0; return DIG; }
        
        if (tramo_origen_ing.fil < tramo_destino_ing.fil) orientacion_deseada_ing = 0;
        else if (tramo_origen_ing.fil > tramo_destino_ing.fil) orientacion_deseada_ing = 4; 
        else if (tramo_origen_ing.col > tramo_destino_ing.col) orientacion_deseada_ing = 2; 
        else orientacion_deseada_ing = 6;
        
        fase_obra_ing = 6; 
    }

    if (fase_obra_ing == 6) {
        if (sensores.rumbo != orientacion_deseada_ing) return TURN_SR;
        fase_obra_ing = 7; 
    }

    if (fase_obra_ing == 7) {
        if (sensores.enfrente) {
            tramo_origen_ing = tramo_destino_ing;
            if (!plan_tuberia.empty()) {
                tramo_destino_ing = plan_tuberia.front();
                plan_tuberia.pop_front();
                fase_obra_ing = 3; 
            } else {
                fase_obra_ing = 8;
            }
            return INSTALL; 
        }
        return IDLE; 
    }
    return IDLE;
}


/**
 * @brief Comportamiento del ingeniero para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{
  return IDLE;
}

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoIngeniero::ActualizarMapa(Sensores sensores)
{
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo)
  {
  case norte:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
 * @brief Determina si una casilla es transitable para el ingeniero.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable (no es muro ni precipicio).
 */
bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el ingeniero: desnivel máximo 1 sin zapatillas, 2 con zapatillas.
 * @param actual Estado actual del agente (fila, columna, orientacion, zap).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2)
    return false;
  if (!zap && desnivel > 1)
    return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0:
    delante.f--;
    break; // norte
  case 1:
    delante.f--;
    delante.c++;
    break; // noreste
  case 2:
    delante.c++;
    break; // este
  case 3:
    delante.f++;
    delante.c++;
    break; // sureste
  case 4:
    delante.f++;
    break; // sur
  case 5:
    delante.f++;
    delante.c--;
    break; // suroeste
  case 6:
    delante.c--;
    break; // oeste
  case 7:
    delante.f--;
    delante.c--;
    break; // noroeste
  }
  return delante;
}

/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
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
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 *
 * @param plan  Lista de pasos (fila, columna, operación),
 *              donde operacion = -1 (DIG), operación = 1 (RAISE).
 */
void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
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
void ComportamientoIngeniero::VisualizaPlan(const ubicacion &st,
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

/**
 * @brief Convierte un plan de tubería en la lista de casillas usada
 *        por el sistema de visualización.
 *
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tubería.
 */
void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
