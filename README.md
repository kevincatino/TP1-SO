# TP1-SO

## Alumnos
- Banfi Malena, 61008
- Catino Kevin, 61643
- Fleischer Lucas, 61153


## SATsolver
El objetivo de este trabajo practico es aprender el uso de distintos IPCs los cuales se encuentran presentes en un sistema de POSIX. Para lograr dicho objetivo, el sistema implementado (aplicacion) distribuirá las tareas de resolución del SAT entre varios procesos. Consiste en los siguientes modulos:
*slave.c*, *app.c*, *masterADT.c*, *view.c*, *sh_mem_ADT.c* y *utilities.c*.

## Compilacion
Se debe compilar haciendo ***make all*** desde la carpeta “TP1-SO”, pudiéndose además hacer un ***make clean*** para borrar los archivos generados y permitir un recompilado del proyecto.

## Ejecucion
Para la ejecucion hay 2 alternativas:
* ##### Alternativa 1
    Escribir en consola: 
      ***./app.out < archivos a procesar > | ./view.out***

* ##### Alternativa 2
    Escribir en consola:  
      ***./app.out < archivos a procesar >***
    y durante la ejecución del programa, ejecutar en otra terminal:  
      ***./view.out < ID >***
    donde < ID > es el número que se imprime en la primera terminal al ejecutar ./app.out
