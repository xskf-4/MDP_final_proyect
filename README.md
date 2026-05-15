# Proyecto final de Procesos Estocásticos

Este programa desarrollado en C permite la resolución a Problemas Markovianos de Decisión(MDP) mediante diversos algoritmos de optimización.

El sistema está diseñado para procesar archivos de configuración de forma secuencial, validando consistencia matemática de los modelos antes de su resolución.

## Características

Este programa implementa los siguientes algoritmos:
- Enumeración Exhaustiva.
- Programación Lineal.
- Mejoramiento de Políticas (con y sin factor de descuento).
- Iteración de Políticas.

## Estructura del Proyecto

- `src/`: Archivos fuente (`.c`) con la lógica de los algoritmos y funciones de utilidad.

- `include/`: Cabeceras (`.h`) con las estructuras de datos primarias.

- `examples/`: Archivos (`.txt`) de prueba con modelos de MDPs.

- `bin/`: Directorio para los ejecutables compilados.

## Compilación y Ejecución

### Requisitos

- Compilador: `gcc`.

### Instalación

1. Clona el repositorio:

```
    git clone https://github.com/xskf-4/MDP_final_proyect.git
    cd MDP_final_proyect
```

2. Compilar el proyecto:

    `gcc src/main.c src/list.c src/matrix_.c src/simplex.c -Iinclude -o bin/Proyecto_final`

#### Consideraciones Adicionales

El programa utiliza códigos escape ANSI para manipular colores en la terminal. Utilizar el programa en una terminal que no admita true-color o sea incompatible con los códigos escape ANSI,
 resultará en comportamiento no deseable al menos visualmente (Se está trabajando en ello).

### Uso

Ejecuta el programa pasando uno o más archivos de texto como argumentos:

    `./bin/Proyecto_final archivo1.txt archivo2.txt`

### Formato de los Archivos de Entrada

Los archivos `.txt` deben seguir una estructura específica para ser considerados consistentes por el programa:

1. Definición de Estados y Acciones:

Indicar el número total de estados y acciones utilizando el siguiente formato (`$` indica el número).

    `#decisiones: $`

    `#estados: $`

2. Matrices de Transición: 

Indicar la matriz de transición para la decisión `k` de la siguiente forma.

```
    #k: ...
```

3. Matriz de Recompenzas o Costos:

Indicar la matriz de recompenzas o costos de la siguiente forma.

```
    #recompenzas: ...
    #costos: ...
```

#### Consideraciones Adicionales

- `...` son los valores de la matriz en formato decimal o fracción (`$/$`) separados por espacios (` `) o saltos de línea (`\n`). La cantidad de valores deben ser por lo menos del mismo tamaño de la matriz correspondiente, valores posteriores serán ignorados.

- En el caso de las matrices de transición para indicar que una decisión no es válida en cierto estado los valores de ese renglón correspondiente deben ser `-1`.

### Flujo del Programa

1. Inicio: El programa comienza con el primer argumento dado.

2. Carga: El programa lee el archivo en posición.

3. Validación: El programa valida que el modelo leído sea consistente (probabilidades válidas, dimensiones correctas).

4. Menú Interactivo: Permite al usuario la selección de los métodos de resolución, saltar al siguiente archivo en la cola o terminar la ejecución.

#### Consideraciones Adicionales

Cada que se termine un proceso, es indispensable presionar enter para continuar con la ejecución del programa.