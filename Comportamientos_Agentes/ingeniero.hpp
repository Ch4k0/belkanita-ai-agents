#ifndef COMPORTAMIENTOINGENIERO_H
#define COMPORTAMIENTOINGENIERO_H

#include <chrono>
#include <list>
#include <map>
#include <set>
#include <thread>
#include <time.h>
#include <vector>

#include "comportamientos/comportamiento.hpp"




  // =========================================================================
  // NIVEL 4 (TUBERÍAS)
  // =========================================================================

struct EstadoTuberia {
      int f, c, op;
      
      bool operator<(const EstadoTuberia &st) const {
          if (f != st.f) return f < st.f;
          if (c != st.c) return c < st.c;
          return op < st.op;
      }
      
      bool operator==(const EstadoTuberia &st) const {
          return f == st.f && c == st.c && op == st.op;
      }
  };

  struct NodoTuberia {
      EstadoTuberia estado;
      std::vector<Paso> secuencia; 
      int eco; // Impacto ecológico acumulado
      int g;   // Longitud del camino (Pasos)

      bool operator<(const NodoTuberia &n) const {
          if (g == n.g) return eco > n.eco;
          return g > n.g; 
      }
  };




  // =========================================================================
  // ESTRUCTURAS (NIVEL 2)
  // =========================================================================

struct EstadoI {
    ubicacion site;
    bool zapatillas;

    bool operator==(const EstadoI &st) const {
        return site == st.site && zapatillas == st.zapatillas;
    }
};

struct NodoI {
    EstadoI estado;
    list<Action> secuencia;

    bool operator==(const NodoI &node) const {
        return estado == node.estado;
    }

    bool operator<(const NodoI &node) const {
        if (estado.site.f < node.estado.site.f) return true;
        if (estado.site.f == node.estado.site.f && estado.site.c < node.estado.site.c) return true;
        if (estado.site.f == node.estado.site.f && estado.site.c == node.estado.site.c && estado.site.brujula < node.estado.site.brujula) return true;
        if (estado.site.f == node.estado.site.f && estado.site.c == node.estado.site.c && estado.site.brujula == node.estado.site.brujula && !estado.zapatillas && node.estado.zapatillas) return true;
        return false;
    }
};


class ComportamientoIngeniero : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================
  
  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoIngeniero(unsigned int size = 0) : Comportamiento(size) {
    // Inicializar Variables de Estado
    last_action = IDLE; 
    tiene_zapatillas = false;
    if (size > 0) {
        mapaCalor.assign(size, std::vector<int>(size, 0));
    }
  }

  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoIngeniero(std::vector<std::vector<unsigned char>> mapaR, 
                         std::vector<std::vector<unsigned char>> mapaC): 
                         Comportamiento(mapaR, mapaC) {
    // Inicializar Variables de Estado
          hayPlan = false;
          fase_obra_ing = 0;

    
  }

  ComportamientoIngeniero(const ComportamientoIngeniero &comport)
      : Comportamiento(comport) {}
  ~ComportamientoIngeniero() {}

  /**
   * @brief Bucle principal de decisión del agente.
   * Estudia los sensores y decide la siguiente acción.
   * 
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
   * return accion; // El motor ejecutará esta acción
   */
  Action think(Sensores sensores);

  ComportamientoIngeniero *clone() {
    return new ComportamientoIngeniero(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  // Funciones específicas para cada nivel (para ser implementadas por el alumno)
  
  /**
   * @brief Implementación del Nivel 0.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_0(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 1.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_1(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 2.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */ 
  Action ComportamientoIngenieroNivel_2(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 3.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_3(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 4.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_4(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 5.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_5(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 6.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  /**
   * @brief Actualiza la información del mapa interno basándose en los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores (casilla actual + 15 casillas alrededor).
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Comprueba si una casilla es transitable.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee zapatillas.
   * @return true si la casilla es transitable (no es muro ni precipicio).
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLAS: Desnivel máximo 1 sin zapatillas, 2 con zapatillas.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual, bool zap);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;

  bool es_camino(unsigned char c) const;

  /**
 * @brief Imprime por consola la secuencia de acciones de un plan para un agente.
 * @param plan  Lista de acciones del plan.
 */
  void PintaPlan(const list<Action> &plan);


/**
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 * @param plan  Lista de pasos (fila, columna, operación).
 */
  void PintaPlan(const list<Paso> &plan);


  /**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa gráfico.
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
  void VisualizaPlan(const ubicacion &st, const list<Action> &plan);

  /**
 * @brief Convierte un plan de tubería en la lista de casillas usada
 *        por el sistema de visualización.
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tubería.
 */
  void VisualizaRedTuberias(const list<Paso> &plan);



private:
  // =========================================================================
  // VARIABLES DE ESTADO (PUEDEN SER EXTENDIDAS POR EL ALUMNO)
  // =========================================================================

  Action last_action; // Última acción realizada (puede ser útil para algunos niveles)
  bool tiene_zapatillas; // Indica si el agente posee las zapatillas (útil para niveles con desniveles)
  std::vector<std::vector<int>> mapaCalor; // Mapa de calor para registrar las casillas visitadas (puede ser útil para niveles de exploración)

  void InicializarMapaCalor();
  int ObtenerCalor(int f, int c);
  void ObtenerCoordenadasSensor(int rumbo, int fActual, int cActual, int indiceActual, int &fRes, int &cRes);
  
  bool EsMovimientoValidoNivel0(int indicadorSensor, const Sensores& sensores);
  bool EsMovimientoValidoNivel1(int indicadorSensor, const Sensores& sensores);

  Action DecidirMejorMovimientoNivel0(const Sensores& sensores);
  Action DecidirMejorMovimientoNivel1(const Sensores& sensores);


  // Variables Nivel 2
  bool hayPlan;
  list<Action> plan;

  // Funciones de Búsqueda
  bool CasillaAccesibleIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  EstadoI NextCasillaIngeniero(const EstadoI &st);
  EstadoI applyI(Action accion, const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  list<Action> B_Anchura(const EstadoI &inicio, const EstadoI &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  
  // Funciones de para el salto
  bool CasillaAccesibleIngenieroJump(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  EstadoI NextCasillaIngenieroJump(const EstadoI &st);


  // Nivel 3
  bool VeoAlTecnico(const Sensores& sensores);

  //Nivel 4
  int CalcularImpacto(char terreno, int op);
  std::list<Paso> PlanificarRed(int f_ini, int c_ini, const std::vector<std::vector<unsigned char>> &mapa, const std::vector<std::vector<unsigned char>> &alturas, int max_eco);


  //Nivel 5 
  int fase_obra_ing;
  std::list<Paso> plan_tuberia;
  Paso tramo_origen_ing;
  Paso tramo_destino_ing;
  std::list<Action> plan_viaje_ing; 
  int orientacion_deseada_ing;

};
#endif