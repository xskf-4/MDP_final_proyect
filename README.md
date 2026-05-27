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

- `include/`: Cabeceras (`.h`) con las estructuras de datos e interfaces primarias.

- `main/`: Archivo (`main.c`) principal.

- `examples/`: Archivos (`.txt`) de prueba con modelos de MDPs.

- `bin/`: Directorio para los ejecutables compilados.

## Compilación y Ejecución

### Requisitos

- Compilador: `gcc`.

### Instalación

1. Clona el repositorio:

    ```console
        git clone https://github.com/xskf-4/MDP_solver.git
        cd MDP_solver
    ```

2. Compila el proyecto:

    ```console
        gcc main/main.c src/*.c -Iinclude -o bin/Proyecto_final
    ```

#### Consideraciones Adicionales

- En un entorno Linux, es indispensable añadir la bandera `-lm` al final del comando de compilación.

    ```console
        gcc main/main.c src/*.c -Iinclude -o bin/Proyecto_final -lm
    ```

- El programa utiliza códigos escape ANSI para manipular colores en la terminal. Utilizar el programa en una terminal que no admita true-color o sea incompatible con los códigos escape ANSI, resultará en comportamiento no deseable al menos visualmente (Se está trabajando en ello).

### Uso

Ejecuta el programa pasando uno o más archivos de texto como argumentos:

```console
    ./bin/Proyecto_final archivo1.txt archivo2.txt
```

### Formato de los Archivos de Entrada

Los archivos `.txt` deben seguir una estructura específica para ser considerados consistentes por el programa:

1. Definición de Estados y Acciones:

    Indicar el número total de estados y acciones utilizando el siguiente formato (`$` indica el número).

    ```text
        #decisiones: $
        #estados: $
    ```

2. Matrices de Transición: 

    Indicar la matriz de transición para la decisión `k` de la siguiente forma.

    ```text
        #k: ...
    ```

3. Matriz de Recompenzas o Costos:

    Indicar la matriz de recompenzas o costos de la siguiente forma.

    ```text
        #recompenzas: ...
        #costos: ...
    ```

#### Consideraciones Adicionales

- `...` son los valores de la matriz en formato decimal o fracción (`$/$`) separados por espacios (` `) o saltos de línea (`\n`). La cantidad de valores deben ser por lo menos del mismo tamaño de la matriz correspondiente, valores posteriores serán ignorados.

- En el caso de las matrices de transición, para indicar que una decisión no es válida en cierto estado los valores de ese renglón correspondiente deben ser `-1`.

- Para la matriz de costos o recompenzas, si una decisión no es válida en cierto estado, el valor correspondiente en la matriz será ignorado pero es indispensable que exista para que la matriz esté completa.

- Toda la información (número de estados, número de decisiones y matrices) puede presentarse en cualquier orden.

### Flujo del Programa

1. Inicio: El programa comienza con el primer argumento dado.

2. Carga: El programa lee el archivo en posición.

3. Validación: El programa valida que el modelo leído sea consistente (probabilidades válidas, dimensiones correctas).

4. Menú Interactivo: Permite al usuario la selección de los métodos de resolución, saltar al siguiente archivo en la cola o terminar la ejecución.
