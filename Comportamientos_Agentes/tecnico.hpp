#ifndef COMPORTAMIENTOTECNICO_H
#define COMPORTAMIENTOTECNICO_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>
#include <vector>

#include "comportamientos/comportamiento.hpp"

// =========================================================================
// DOCUMENTACIÓN PARA ESTUDIANTES
// =========================================================================
/*
 * CLASE: ComportamientoTecnico
 * 
 * DESCRIPCIÓN:
 * Esta clase implementa el comportamiento del agente Técnico en el mundo Belkan.
 * El técnico colabora con el ingeniero para resolver el problema de instalación de tuberías
 */


// =========================================================================
// ESTRUCTURAS (NIVEL 3)
// =========================================================================

struct EstadoT {
    ubicacion site;
    bool zapatillas;

    bool operator==(const EstadoT &st) const {
        return (site.f == st.site.f && site.c == st.site.c && 
                site.brujula == st.site.brujula && zapatillas == st.zapatillas);
    }

    bool operator<(const EstadoT &st) const {
        if (site.f != st.site.f) return site.f < st.site.f;
        if (site.c != st.site.c) return site.c < st.site.c;
        if (site.brujula != st.site.brujula) return site.brujula < st.site.brujula;
        return zapatillas < st.zapatillas;
    }
};

struct NodoT {
    EstadoT estado;
    vector<Action> secuencia;
    int g;
    int h;
    int f;

    bool operator<(const NodoT &node) const {
        if (f == node.f) return h > node.h;
        return f > node.f;
    }
};

struct ComparaCostos {
    bool operator()(const NodoT &a, const NodoT &b) const {
        if (a.f == b.f) return a.h > b.h;
        return a.f > b.f; 
    }
};






class ComportamientoTecnico : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================
  
  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoTecnico(unsigned int size = 0) : Comportamiento(size) {
    // Inicializar Variables de Estado
    last_action = IDLE; 
    tiene_zapatillas = false;
    cuentaGiros = 0;
    giro45Izq = 0;
    if (size > 0) {
        mapaCalor.assign(size, std::vector<int>(size, 0));
    }
  }

  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoTecnico(std::vector<std::vector<unsigned char>> mapaR, 
                       std::vector<std::vector<unsigned char>> mapaC): 
                       Comportamiento(mapaR, mapaC) {
    // Inicializar Variables de Estado

    hayPlan = false;
    tiene_zapatillas = false;
    fase_obra_tec = 0;
    meta_f_tec = 0;
    meta_c_tec = 0;      

  }

  ComportamientoTecnico(const ComportamientoTecnico &comport): Comportamiento(comport) {}
  ~ComportamientoTecnico() {}

  /**
   * @brief Bucle principal de decisión del técnico.
   * Estudia los sensores y decide la siguiente acción.
   * 
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
   * return accion; // El motor ejecutará esta acción
   */
  Action think(Sensores sensores);

  ComportamientoTecnico *clone() {
    return new ComportamientoTecnico(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================
  
/**
 * @brief Comportamiento del técnico para el Nivel 0.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_0(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_1(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_2(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_3(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_4(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_5(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  /**
   * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores.
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Determina si una casilla es transitable para el técnico.
   * NOTA: El técnico puede tener reglas de transitabilidad diferentes al ingeniero.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee las zapatillas.
   * @return true si la casilla es transitable.
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLA PARA TÉCNICO: Desnivel máximo siempre 1 (independiente de zapatillas).
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;

  /**
   * @brief Comprueba si una celda es de tipo transitable por defecto.
   * @param c Carácter que representa el tipo de superficie.
   * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
   */
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

private:
  // =========================================================================
  // VARIABLES DE ESTADO (PUEDEN SER EXTENDIDAS POR EL ALUMNO)
  // =========================================================================

  Action last_action; // Última acción realizada (puede ser útil para algunos niveles)
  bool tiene_zapatillas; // Indica si el agente posee las zapatillas (útil para niveles con desniveles)
  int cuentaGiros; // Contador de giros a la izquierda (puede ser útil para niveles con orientación
  int giro45Izq; // Contador de giros a la izquierda (puede ser útil para niveles con orientación
  std::vector<std::vector<int>> mapaCalor;

  void InicializarMapaCalor();
  int ObtenerCalor(int f, int c);
  void ObtenerCoordenadasSensor(int rumbo, int fActual, int cActual, int indiceSensor, int &fRes, int &cRes);
  
  bool EsMovimientoValidoNivel0(int indiceSensor, const Sensores& sensores);
  bool EsMovimientoValidoNivel1(int indiceSensor, const Sensores& sensores);

  Action DecidirMejorMovimientoNivel0(const Sensores& sensores);
  Action DecidirMejorMovimientoNivel1(const Sensores& sensores);


  // Variables Nivel 3
  bool hayPlan;
  list<Action> plan;

  int DistanciaChebyshev(int f1, int c1, int f2, int c2);
  int CostoBateria(Action accion, const EstadoT &actual, const EstadoT &siguiente, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  bool CasillaAccesibleTecnico(const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  EstadoT NextCasillaTecnico(const EstadoT &st);
  EstadoT applyT(Action accion, const EstadoT &st, const vector<vector<unsigned char>> &terreno,const vector<vector<unsigned char>> &altura);
  list<Action> A_Estrella(const EstadoT &inicio, const EstadoT &final,const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);



  //Nivel 5
  int fase_obra_tec;
  std::list<Action> plan_viaje_tec;
  int meta_f_tec;
  int meta_c_tec;



};

#endif
