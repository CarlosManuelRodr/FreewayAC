# Autor: David Hernández Enríquez.
"""Este codigo agarra bloques de logitud especificada y grafica
funciona con ipc2.dat.dat"""

from matplotlib import *
from pylab import *

orden = input("Escribe el orden de la permutacion: ")
archivo = raw_input("Arrastra el archivo de datos o escribe su nombre: ")
try:
    f = open(archivo, "r")
except:
    print "No existe ningun archivo llamado ", archivo

n = 0
st = []
fecha = []
datolist = []

while True:
    datono = f.readline()
    if not datono:
        break

    datof = datono.split(",")
    fecha.append(float(datof[0]))
    datolist.append(float(datof[1]))
    n += 1

lon = input("Escribe la longitud de los intervalos a calcular: ")
entropias = []
intervalos = []

for rep in range(0, n-lon+1):
    selecf = fecha[rep:lon+rep]
    selecdl = datolist[rep:lon+rep]
    T = len(selecf)-orden+1
    factorial = math.factorial(orden)        # Se puede optimizar la longitud de la lista
    H = 0
    vp = zeros((factorial, 3), float)
    
    for i in range(0, T):
        vecin = selecdl[i:i+orden]
        permutacion = []
        for o in range(0, orden):
            permutacion.append(0)

        permu = arange(1, orden+1)

        # Inicio de ordenamiento burbuja
        intercambios = 1
        pasada = 1 
        while pasada < len(vecin) and intercambios == 1:
            intercambios=0
            for m in range(0, len(vecin)-pasada):
                if vecin[m] > vecin[m+1]:
                    vecin[m], vecin[m+1] = vecin[m+1], vecin[m]
                    permu[m], permu[m+1] = permu[m+1], permu[m]
                    intercambios=1
            pasada += 1

        # Fin ordenamiento
        for j in range(0, orden):
            permutacion[permu[j]-1] = j+1
        
        valorp = 0
        perminv = permutacion.reverse()

        # Ciclo para obtener el valor de la permutacion
        for k in range(0, orden):
            valorp = valorp+(permutacion[k]*(10**k))

        # Ciclo para acumular valores de permutaciones repetidas
        for l in range(0, T):
            if valorp == vp[l, 0]:
                vp[l, 1] = vp[l, 1]+1
                break
            else:
                if valorp != vp[l, 0] and vp[l, 0] == 0:
                    vp[l, 0] = valorp
                    vp[l, 1] = vp[l, 1]+1
                    break

    for j in range(0, factorial):
        if vp[j, 0] == 0:
            continue
        vp[j, 2] = vp[j, 1]/T
        pi = vp[j, 2]*(math.log(vp[j, 2], 2))*(-1)
        H = H+pi

    print ("El valor de la entropia de permutacion de orden "), orden, (" obtenida\npara una seleccion de "), lon, (" datos contados\na partir del elemento "), rep, (" de una serie de tiempo\n de longitud "), n, (" fue de:\n"), H
    entropias.append(H)
    intervalos.append(rep)

title("Entropia de Permutacion de una serie de tiempo")
xlabel("Numero de intervalo")
ylabel("Valor de entropia de permutacion")
grid()
plot(intervalos,entropias)
show()