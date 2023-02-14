# TEMA 1: BOOT
Requeriments mínims per a bootar un computador:
- CPU
- RAM
- CODI a executar
- BIOS (Basic Input/Output System) (~60/70 KB)

## Proceso de inicio del sistema operativo

Power on ---> BIOS ---> Bootloader ---> Complete OS init 

---> : load

La bios siempre se vuelca a la misma @M al encender
el sistema, y configura el procesador para lanzar las
rutinas del SO.

Tras esto, buscamos discos de memoria botables i los 
leemos, trayendo a memória el MBR (Master Boot Record),
que pesa exactamente 512 Bytes de memoria.

Antiguamente, este espacio era suficiente para poder
bootear el sistema, pero es sistemas operativos actuales,
donde la complejidad es mucho más elevada, esta rutina
carga en memoria otro programa. Al primero, se le llama
__BOOT LOADER__, mientras que este nuevo programa, (que SÍ
puede ocupar más de 512 Bytes), es el que realmente se ocupa
de arrancar el kernel.

## Rutina de inicio del kernel:
__REAL MODE__ {
- __Init estructuras__
- __Reconocimiento HW__: Por tercera vez, este reconocimiento.
es el más exhaustivo (para así poder daber que drivers son necesarios,
además de su configuracion).
- __Puntos de entrada__: Prepara todos los servicios para interactuar
con el usuario.
- __Init__: Proceso padre de todos los procesos/El primer proceso de usuario.
Normalmente se vuelca del disco a la memoria.
}
- __Protected mode__: Protegemos el HW del usuario en si. El usuario no toca
el HW; en cambio, el usuario le pide al SO que toque el HW.
- __Trampoline__: Empieza a ejecutar el init (proceso creado antes).

## Protected mode:
En la arquitectura x86, existe un registro llamado PSW (Process State Word),
que guarda el estado del proceso y el procesador. Dentro de este registro,
existen dos bits que determinan el nivel de privilegios con el que se ha
ejecutado el programa, llamados CPL (Current Privilege Level). Los niveles
de provilegio en x86 son los siguientes:

- 11 -> Menos privilegios
- 10
- 01
- 00 -> Más privilegios

Las instrucciones en x86 también tienen un nivel de privilegio associado.

Cuando una instruccion llega a la etapa Decode dentro del pipeline del
procesador, se compara el CPL con el nivel de privilegios de la instruccion.
Si __iPL <= CPL__, entonces la instruccion sí que se ejecutará si problema.
En caso contrario, el procesador terminal la ejecucion del resto de programa
(presumo que inyecta nops y mueve el CP a una posicion sin instrucciones
de la MI).

La penúltima etapa del booteo activa el mecanismo, por lo tanto nos pone en
dicho modo protegido.

Lo último, és activar la paginacion para poner en marcha la traducción de @M
 y que cada proceso tenga su espacio de memória.
